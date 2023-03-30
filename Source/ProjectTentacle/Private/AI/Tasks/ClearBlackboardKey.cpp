// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/ClearBlackboardKey.h"
#include "BehaviorTree/BlackboardComponent.h"

UClearBlackboardKey::UClearBlackboardKey()
{
	NodeName = "Clear Blackboard Key";
}

EBTNodeResult::Type UClearBlackboardKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerComp.GetBlackboardComponent()->ClearValue(BlackboardKey.SelectedKeyName);
	return EBTNodeResult::Succeeded;
}
