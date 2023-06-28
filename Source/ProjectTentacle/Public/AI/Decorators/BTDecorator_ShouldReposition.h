// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_ShouldReposition.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UBTDecorator_ShouldReposition : public UBTDecorator
{
	GENERATED_BODY()

public:
	// Checks the old EQS result vs the new one, and decides whether to move or not based on the distance between them
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector OldSurroundPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector NewSurroundPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Precision = 100.f;
};
