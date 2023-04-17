// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_WaitForTrigger.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_WaitForTrigger::UBTTask_WaitForTrigger()
{
	NodeName = "Wait for Trigger";

	// Tick for this task
	bNotifyTick = true;
}

// In progress until aborted
EBTNodeResult::Type UBTTask_WaitForTrigger::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Controller = OwnerComp.GetAIOwner();
	if(!Controller) return EBTNodeResult::Failed;
	
	Pawn = Controller->GetPawn();
	if(!Pawn) return EBTNodeResult::Failed;

	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	AActor* AsActor = Cast<AActor>(Blackboard->GetValueAsObject("Target"));
	if(!AsActor) return EBTNodeResult::Failed;

	FocalActor = AsActor;
	OwnerComp.GetAIOwner()->SetFocus(FocalActor);
	
	float FlatAngle = CalculateFlatAngle(Pawn->GetActorForwardVector(),
	                                     (FocalActor->GetActorLocation() - Pawn->GetActorLocation()).GetSafeNormal());	
	
	return EBTNodeResult::InProgress;
}

float UBTTask_WaitForTrigger::CalculateFlatAngle(const FVector& VectorA, const FVector& VectorB)
{
	return (VectorA.IsNearlyZero() || VectorB.IsNearlyZero())
		? 0.f
		: FMath::Abs(FMath::RadiansToDegrees(acosf(VectorA.CosineAngle2D(VectorB))));
}
