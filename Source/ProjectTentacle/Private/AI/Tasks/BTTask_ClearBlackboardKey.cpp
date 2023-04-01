// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_ClearBlackboardKey.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ClearBlackboardKey::UBTTask_ClearBlackboardKey()
{
	NodeName = "Clear Blackboard Key";
}

EBTNodeResult::Type UBTTask_ClearBlackboardKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetBlackboardComponent()->ClearValue(BlackboardKey.SelectedKeyName);
	return EBTNodeResult::Succeeded;
}
