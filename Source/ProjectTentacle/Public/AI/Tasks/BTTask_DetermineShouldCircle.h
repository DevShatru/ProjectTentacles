// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DetermineShouldCircle.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_DetermineShouldCircle : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_DetermineShouldCircle();
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
	float CirclePercentage = 75.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector DeterminedShouldCircleKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector ShouldCircleKey;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
