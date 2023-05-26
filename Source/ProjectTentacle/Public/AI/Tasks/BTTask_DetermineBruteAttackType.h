// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DetermineBruteAttackType.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_DetermineBruteAttackType : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_DetermineBruteAttackType();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

// protected:
	// UPROPERTY(EditAnywhere, BlueprintReadOnly)
	// FBlackboardKeySelector AttackTypeKey;
};
