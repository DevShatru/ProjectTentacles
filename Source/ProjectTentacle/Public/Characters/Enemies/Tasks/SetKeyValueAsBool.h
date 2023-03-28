// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SetKeyValueAsBool.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API USetKeyValueAsBool : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	USetKeyValueAsBool();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool Value;
	
};
