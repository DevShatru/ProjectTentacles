// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CheckShouldQueue.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_CheckShouldQueue : public UBTTaskNode
{
	GENERATED_BODY()
	UBTTask_CheckShouldQueue();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	FVector LastSurroundPoint;
};
