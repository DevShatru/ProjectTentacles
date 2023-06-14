// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_AssignMoveOutPos.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_AssignMoveOutPos : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = ValueToAssign)
	FBlackboardKeySelector AssigningMoveOutPosition;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	
	UBTTask_AssignMoveOutPos();
};
