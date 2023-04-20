// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_SetFocus.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetFocus::UBTTask_SetFocus()
{
	NodeName = "Set Focus";
}

// In progress until aborted
EBTNodeResult::Type UBTTask_SetFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* Controller = OwnerComp.GetAIOwner();
	if(!Controller) return EBTNodeResult::Failed;

	const APawn* Pawn = Controller->GetPawn();
	if(!Pawn) return EBTNodeResult::Failed;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	AActor* AsActor = Cast<AActor>(Blackboard->GetValueAsObject("Target"));
	if(!AsActor) return EBTNodeResult::Failed;

	OwnerComp.GetAIOwner()->SetFocus(AsActor);
	
	return EBTNodeResult::Succeeded;
}

/*
float UBTTask_WaitForTrigger::CalculateFlatAngle(const FVector& VectorA, const FVector& VectorB)
{
	return (VectorA.IsNearlyZero() || VectorB.IsNearlyZero())
		? 0.f
		: FMath::Abs(FMath::RadiansToDegrees(acosf(VectorA.CosineAngle2D(VectorB))));
}
*/