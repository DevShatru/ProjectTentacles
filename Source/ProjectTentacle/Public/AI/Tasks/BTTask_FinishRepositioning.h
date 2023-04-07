// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FinishRepositioning.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_FinishRepositioning : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=ValuesToModify)
	FBlackboardKeySelector BBKey_bNeedToReposition;


	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
public:
	UBTTask_FinishRepositioning();
	
};
