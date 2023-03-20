// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_GameplayTaskBase.h"
#include "FindPatrolPoint.generated.h"

/**
 * 
 */
class UNavigationSystemV1;

UCLASS()
class PROJECTTENTACLE_API UFindPatrolPoint : public UBTTask_GameplayTaskBase
{
	GENERATED_BODY()
	
public:
	UFindPatrolPoint();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	// Radius within which to find a location
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FindRadius = 100.0f;

	// Blackboard key to write found location to
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Blackboard)
	FBlackboardKeySelector BlackboardKey;

private:
	// Cache nav system and whether cache attempt has been made
	static UNavigationSystemV1* NavSystem;
	static unsigned int bAttemptNavCache;
};
