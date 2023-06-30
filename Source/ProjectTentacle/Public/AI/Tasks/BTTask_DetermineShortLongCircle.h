// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DetermineShortLongCircle.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_DetermineShortLongCircle : public UBTTaskNode
{
	GENERATED_BODY()
protected:

	UBTTask_DetermineShortLongCircle();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	bool DetermineCloseOrLongCircle();
	
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=ValuesToOverwrite)
	FBlackboardKeySelector BBKey_CircleRangeIsClose;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=ModifyingValue, meta=(ClampMin=0, ClampMax=1))
	float ChanceToBeCloseCircle = 0.5;
};
