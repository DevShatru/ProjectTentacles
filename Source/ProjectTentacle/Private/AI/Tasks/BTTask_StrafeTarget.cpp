// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_StrafeTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "GameFramework/Character.h"


UBTTask_StrafeTarget::UBTTask_StrafeTarget()
{
	NodeName = "Strafe Target";
	
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_StrafeTarget, TargetKey), AActor::StaticClass());
	TargetKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_StrafeTarget, TargetKey));
}

void UBTTask_StrafeTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (BBAsset)
	{
		TargetKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("Can't initialize task: %s, make sure that behavior tree specifies blackboard asset!"), *GetName());
	}
}

EBTNodeResult::Type UBTTask_StrafeTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Resolve Target
	bIsObject = TargetKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass();
	if(bIsObject)
	{
		TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName));
		if(!TargetActor) return EBTNodeResult::Failed;
	} else
	{
		TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetKey.SelectedKeyName);
		if(TargetLocation == FVector::ZeroVector) return EBTNodeResult::Failed;
	}
	// Randomly pick between left and right
	bShouldMoveRight = FMath::RandBool();
	// Disable controller yaw
	OwnController = OwnerComp.GetAIOwner();
	OwnCharacter = OwnController->GetCharacter();
	if(OwnCharacter) OwnCharacter->bUseControllerRotationYaw = false;
	StrafeAroundLocation(bIsObject ? TargetActor->GetActorLocation() : TargetLocation);

	// TODO Change direction if it hits something
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_StrafeTarget::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Re-enable controller yaw
	if(OwnCharacter) OwnCharacter->bUseControllerRotationYaw = true;
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_StrafeTarget::StrafeAroundLocation(const FVector Location) const
{
	const FVector CharLocation = OwnCharacter->GetActorLocation();
	const FVector ForwardToTarget = Location - CharLocation;
	const FVector RightDirection = FVector::CrossProduct(ForwardToTarget, FVector::UpVector);
	const FVector Direction = RightDirection * (bShouldMoveRight ? 1 : -1);
	OwnController->MoveToLocation(CharLocation + Direction);
}
