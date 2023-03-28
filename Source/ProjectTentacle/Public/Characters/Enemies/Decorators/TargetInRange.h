// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "TargetInRange.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UTargetInRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UTargetInRange();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Range = 1000.f;

	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector TargetBlackboardKey;
};
