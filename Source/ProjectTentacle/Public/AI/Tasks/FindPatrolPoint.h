// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "FindPatrolPoint.generated.h"

/**
 * 
 */
class UNavigationSystemV1;

UCLASS()
class PROJECTTENTACLE_API UFindPatrolPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UFindPatrolPoint();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	// Radius within which to find a location
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FindRadius = 100.0f;

private:
	// Cache nav system and whether cache attempt has been made
	UNavigationSystemV1* NavSystem;
};
