// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_GetStrafeStrafeLocation.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Characters/Enemies/EnemyBase.h"


UBTTask_GetStrafeStrafeLocation::UBTTask_GetStrafeStrafeLocation()
{
	NodeName = "Strafe Target";
	
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetStrafeStrafeLocation, TargetKey), AActor::StaticClass());
	TargetKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetStrafeStrafeLocation, TargetKey));
	CircleDirectionKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetStrafeStrafeLocation, CircleDirectionKey));
	StrafeLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_GetStrafeStrafeLocation, StrafeLocationKey));
}

void UBTTask_GetStrafeStrafeLocation::InitializeFromAsset(UBehaviorTree& Asset)
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

// Determine point to strafe to based on circle direction and target location
EBTNodeResult::Type UBTTask_GetStrafeStrafeLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Resolve Target
	bIsObject = TargetKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass();
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	bShouldMoveRight = Blackboard->GetValueAsBool(CircleDirectionKey.SelectedKeyName);
	OwnPawn = Cast<AEnemyBase>(OwnerComp.GetAIOwner()->GetCharacter());
	if(bIsObject)
	{
		TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
		if(!TargetActor) return EBTNodeResult::Failed;
	} else
	{
		TargetLocation = Blackboard->GetValueAsVector(TargetKey.SelectedKeyName);
		if(TargetLocation == FVector::ZeroVector) return EBTNodeResult::Failed;
	}
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(StrafeLocationKey.SelectedKeyName, GetStrafeLocation(bIsObject ? TargetActor->GetActorLocation() : TargetLocation));

	return EBTNodeResult::Succeeded;
}

FVector UBTTask_GetStrafeStrafeLocation::GetStrafeLocation(const FVector Location) const
{
	const FVector CharLocation = OwnPawn->GetActorLocation();
	const FVector ForwardToTarget = Location - CharLocation;
	const FVector RightDirection = FVector::CrossProduct(ForwardToTarget, FVector::UpVector);
	const FVector Direction = StrafeDistance * (RightDirection * (bShouldMoveRight ? 1 : -1)).GetSafeNormal();
	
	return CharLocation + Direction;
}
