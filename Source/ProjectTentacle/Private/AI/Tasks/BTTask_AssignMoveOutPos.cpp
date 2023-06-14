// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_AssignMoveOutPos.h"

EBTNodeResult::Type UBTTask_AssignMoveOutPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Succeeded;
}

UBTTask_AssignMoveOutPos::UBTTask_AssignMoveOutPos()
{
	NodeName = "Assign Move Out Pos";
}
