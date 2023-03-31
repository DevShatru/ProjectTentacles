// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_WaitForTrigger.h"

UBTTask_WaitForTrigger::UBTTask_WaitForTrigger()
{
	NodeName = "Wait for Trigger";

	// Tick for this task
	bNotifyTick = true;
}

// In progress until aborted
EBTNodeResult::Type UBTTask_WaitForTrigger::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}