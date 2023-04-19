// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WaitForTrigger.generated.h"

/**
 * TickTask function that never completes, need to be aborted by decorator
 * TODO: Needs to timeout at some point, otherwise there's a danger of an endless wait
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_WaitForTrigger : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_WaitForTrigger();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Precision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float CirclePercentage = 75.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector ShouldCircleKey;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	// static float CalculateFlatAngle(const FVector& VectorA, const FVector& VectorB);
};
