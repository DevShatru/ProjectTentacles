// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_QueueForAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_QueueForAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_QueueForAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	
};
