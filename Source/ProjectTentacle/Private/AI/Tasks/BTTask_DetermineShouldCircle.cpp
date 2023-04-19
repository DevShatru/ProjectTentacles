// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_DetermineShouldCircle.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_DetermineShouldCircle::UBTTask_DetermineShouldCircle()
{
	NodeName = "Determine Should Circle";

	DeterminedShouldCircleKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DetermineShouldCircle, DeterminedShouldCircleKey));
	ShouldCircleKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DetermineShouldCircle, ShouldCircleKey));
}

EBTNodeResult::Type UBTTask_DetermineShouldCircle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const float Percentage = FMath::FRandRange(0.f, 100.0f);
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(ShouldCircleKey.SelectedKeyName, Percentage <= CirclePercentage);;
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(DeterminedShouldCircleKey.SelectedKeyName, true);
	
	return EBTNodeResult::Succeeded;
}
