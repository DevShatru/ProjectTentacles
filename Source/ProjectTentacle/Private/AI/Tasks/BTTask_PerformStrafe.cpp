// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_PerformStrafe.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Characters/Enemies/EnemyBase.h"

UBTTask_PerformStrafe::UBTTask_PerformStrafe()
{
	NodeName = "Perform Strafe";
	
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_PerformStrafe, TargetKey), AActor::StaticClass());
	TargetKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_PerformStrafe, TargetKey));
	CircleDirectionKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_PerformStrafe, CircleDirectionKey));

	bNotifyTick = true;
}

void UBTTask_PerformStrafe::InitializeFromAsset(UBehaviorTree& Asset)
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

EBTNodeResult::Type UBTTask_PerformStrafe::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* OwnController = OwnerComp.GetAIOwner();
	if(!OwnController) return EBTNodeResult::Failed;

	OwnPawn = Cast<AEnemyBase>(OwnController->GetPawn());
	if(!OwnPawn) return EBTNodeResult::Failed;

	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	bStrafeRight = Blackboard->GetValueAsBool(CircleDirectionKey.SelectedKeyName);
	
	bIsTargetActor = TargetKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass();
	if(bIsTargetActor)
	{
		StrafeActorTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	} else
	{
		StrafeLocationTarget = Blackboard->GetValueAsVector(TargetKey.SelectedKeyName);
	}

	LoopDistSqr = DistancePerLoop * DistancePerLoop;
	LoopIterationsComplete = 0;
	
	OwnPawn->EnableStrafe(bUseStrafeSpeed);
	StrafeLoopStartLocation = OwnPawn->GetActorLocation();
	StrafeLoopDirection = GetStrafeDirection();
	
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_PerformStrafe::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnPawn->EnableStrafe(false);
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_PerformStrafe::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	OwnPawn->EnableStrafe(false);
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_PerformStrafe::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if(!IsValid(OwnPawn)) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	
	// Check distance from starting points
	if(FVector::DistSquared(OwnPawn->GetActorLocation(), StrafeLoopStartLocation) >= LoopDistSqr)
	{
		++LoopIterationsComplete;
		if(LoopIterationsComplete >= StrafeLoopsAttempted)
		{
			// Disable strafe speed on finish
			OwnPawn->EnableStrafe(false);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
		StrafeLoopStartLocation = OwnPawn->GetActorLocation();
		StrafeLoopDirection = GetStrafeDirection();
	}

	OwnPawn->AddMovementInput(StrafeLoopDirection, 1.f);
}

FVector UBTTask_PerformStrafe::GetStrafeDirection() const
{
	FVector OwnLocation = OwnPawn->GetActorLocation();
	FVector TargetLocation = bIsTargetActor ? StrafeActorTarget->GetActorLocation() : StrafeLocationTarget;
	FVector ToTarget = TargetLocation - OwnLocation;
	FVector TravelDirection = (bStrafeRight ? 1 : -1) * FVector::CrossProduct(ToTarget, FVector::UpVector);

	// Calculate right direction from current position and target location, then multiply by +/- 1 depending on bStrafeRight
	return TravelDirection.GetSafeNormal();
}
