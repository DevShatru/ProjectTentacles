// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Enemies/Tasks/SetKeyValueAsBool.h"

#include "BehaviorTree/BlackboardComponent.h"

USetKeyValueAsBool::USetKeyValueAsBool()
{
	NodeName = "Set Key Value as Boolean";

	//Ensure selected key is a bool
	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(USetKeyValueAsBool, BlackboardKey));
}

EBTNodeResult::Type USetKeyValueAsBool::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Save bool to blackboard
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(BlackboardKey.SelectedKeyName, Value);
	return EBTNodeResult::Succeeded;
}
