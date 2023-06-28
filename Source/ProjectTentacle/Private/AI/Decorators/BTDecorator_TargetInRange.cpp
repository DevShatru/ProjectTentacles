// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Decorators/BTDecorator_TargetInRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_TargetInRange::UBTDecorator_TargetInRange()
{
	NodeName = "Target in Range";
	//Ensure selected key is a vector
	TargetBlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_TargetInRange, TargetBlackboardKey), AActor::StaticClass());
}

// Check if attack target is in range
bool UBTDecorator_TargetInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(!OwningPawn) return false;

	UObject* TargetObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKey.SelectedKeyName);
	if(!TargetObject) return false;

	const float SquareDistance = FVector::DistSquared2D(OwningPawn->GetActorLocation(), Cast<AActor>(TargetObject)->GetActorLocation());
	return SquareDistance <= Range * Range;
}

