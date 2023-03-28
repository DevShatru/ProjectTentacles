// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Enemies/Decorators/TargetInRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UTargetInRange::UTargetInRange()
{
	NodeName = "Target in Range";
	//Ensure selected key is a vector
	TargetBlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UTargetInRange, TargetBlackboardKey), AActor::StaticClass());
}

bool UTargetInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(!OwningPawn) return false;

	UObject* TargetObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKey.SelectedKeyName);
	if(!TargetObject) return false;

	const float SquareDistance = FVector::DistSquared2D(OwningPawn->GetActorLocation(), Cast<AActor>(TargetObject)->GetActorLocation());
	return SquareDistance <= Range * Range;
}

