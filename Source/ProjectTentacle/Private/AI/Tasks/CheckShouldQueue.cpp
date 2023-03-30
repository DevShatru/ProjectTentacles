// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/CheckShouldQueue.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UCheckShouldQueue::UCheckShouldQueue()
{
	NodeName = "Check if should queue";
}

EBTNodeResult::Type UCheckShouldQueue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	// If this hasn't run before, auto fail
	if(LastSurroundPoint == FVector::ZeroVector)
	{
		LastSurroundPoint = Blackboard->GetValueAsVector("SurroundPoint");
		return EBTNodeResult::Succeeded;
	}

	// If we haven't moved since the last check, start queuing
	if(LastSurroundPoint == Blackboard->GetValueAsVector("SurroundPoint"))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("bIsQueued", true);
		return EBTNodeResult::Succeeded;
	}
	
	OwnerComp.GetBlackboardComponent()->ClearValue("bIsQueued");
	return EBTNodeResult::Succeeded;
}
