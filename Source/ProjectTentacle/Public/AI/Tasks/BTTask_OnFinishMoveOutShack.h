// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_OnFinishMoveOutShack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_OnFinishMoveOutShack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_OnFinishMoveOutShack();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ValueToAssign)
	FBlackboardKeySelector HasNotMovingOut;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
