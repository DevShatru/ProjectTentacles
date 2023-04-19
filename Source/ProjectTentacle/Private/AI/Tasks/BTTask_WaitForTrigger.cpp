// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_WaitForTrigger.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_WaitForTrigger::UBTTask_WaitForTrigger()
{
	NodeName = "Wait for Trigger";

	// Tick for this task
	bNotifyTick = false;
	ShouldCircleKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_WaitForTrigger, ShouldCircleKey));
}

// In progress until aborted
EBTNodeResult::Type UBTTask_WaitForTrigger::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* Controller = OwnerComp.GetAIOwner();
	if(!Controller) return EBTNodeResult::Failed;

	const APawn* Pawn = Controller->GetPawn();
	if(!Pawn) return EBTNodeResult::Failed;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	AActor* AsActor = Cast<AActor>(Blackboard->GetValueAsObject("Target"));
	if(!AsActor) return EBTNodeResult::Failed;

	OwnerComp.GetAIOwner()->SetFocus(AsActor);

	// Determine on weight whether to circle or idle
	const float Percentage = FMath::FRandRange(0.f, 100.0f);
	Blackboard->SetValueAsBool(ShouldCircleKey.SelectedKeyName, Percentage <= CirclePercentage);
	
	return EBTNodeResult::InProgress;
}

/*
float UBTTask_WaitForTrigger::CalculateFlatAngle(const FVector& VectorA, const FVector& VectorB)
{
	return (VectorA.IsNearlyZero() || VectorB.IsNearlyZero())
		? 0.f
		: FMath::Abs(FMath::RadiansToDegrees(acosf(VectorA.CosineAngle2D(VectorB))));
}
*/