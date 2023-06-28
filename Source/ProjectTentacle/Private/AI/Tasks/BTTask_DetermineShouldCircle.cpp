// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_DetermineShouldCircle.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_DetermineShouldCircle::UBTTask_DetermineShouldCircle()
{
	NodeName = "Determine Should Circle";

	DeterminedShouldCircleKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DetermineShouldCircle, DeterminedShouldCircleKey));
	ShouldCircleKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DetermineShouldCircle, ShouldCircleKey));
	ShouldCircleRightKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DetermineShouldCircle, ShouldCircleRightKey));
}

// Determine whether to hold position or circle (and in which direction) based on a given weight
EBTNodeResult::Type UBTTask_DetermineShouldCircle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const float Percentage = FMath::FRandRange(0.f, 100.0f);
	const bool bShouldCircle = Percentage <= CirclePercentage;
	
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	Blackboard->SetValueAsBool(ShouldCircleKey.SelectedKeyName, bShouldCircle);
	Blackboard->SetValueAsBool(DeterminedShouldCircleKey.SelectedKeyName, true);
	if(bShouldCircle) Blackboard->SetValueAsBool(ShouldCircleRightKey.SelectedKeyName, FMath::RandBool());
	
	return EBTNodeResult::Succeeded;
}
