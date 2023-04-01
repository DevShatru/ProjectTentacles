// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Requeue.generated.h"

/**
 * Task to clear relevant blackboard flags after attack completion, allowing unit to re-enter attack queue
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_Requeue : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Requeue();

protected:
	// Flags to clear
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Blackboard)
	FBlackboardKeySelector IsQueuedKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Blackboard)
	FBlackboardKeySelector IsAttackingKey;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
