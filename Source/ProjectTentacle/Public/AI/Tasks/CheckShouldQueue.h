// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CheckShouldQueue.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UCheckShouldQueue : public UBTTaskNode
{
	GENERATED_BODY()
	UCheckShouldQueue();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	FVector LastSurroundPoint;
};
