// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "ClearBlackboardKey.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UClearBlackboardKey : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UClearBlackboardKey();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
