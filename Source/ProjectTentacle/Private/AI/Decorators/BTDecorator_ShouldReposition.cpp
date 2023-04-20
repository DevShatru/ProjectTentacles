// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Decorators/BTDecorator_ShouldReposition.h"

#include "BehaviorTree/BlackboardComponent.h"

bool UBTDecorator_ShouldReposition::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                               uint8* NodeMemory) const
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	const FVector OldSurroundPointLocation = Blackboard->GetValueAsVector(OldSurroundPoint.SelectedKeyName);
	const FVector NewSurroundPointLocation = Blackboard->GetValueAsVector(NewSurroundPoint.SelectedKeyName);
	const bool bResult = FVector::DistSquared(OldSurroundPointLocation, NewSurroundPointLocation) > powf(Precision, 2);

	if(bResult)
	{
		Blackboard->SetValueAsVector(OldSurroundPoint.SelectedKeyName, NewSurroundPointLocation);
	}

	return bResult;
}
