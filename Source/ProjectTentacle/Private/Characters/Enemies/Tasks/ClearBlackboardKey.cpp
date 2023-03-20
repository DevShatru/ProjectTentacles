// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemies/Tasks/ClearBlackboardKey.h"

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
