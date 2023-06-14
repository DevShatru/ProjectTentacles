// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_OnFinishMoveOutShack.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_OnFinishMoveOutShack::UBTTask_OnFinishMoveOutShack()
{
	NodeName = "On Finish Move Out Shack";
}

EBTNodeResult::Type UBTTask_OnFinishMoveOutShack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if(!BBComp) return EBTNodeResult::Failed;

	BBComp->SetValueAsBool(HasNotMovingOut.SelectedKeyName, false);
	
	return EBTNodeResult::Succeeded;
}
