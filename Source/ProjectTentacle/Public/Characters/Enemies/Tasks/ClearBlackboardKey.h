// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_GameplayTaskBase.h"
#include "ClearBlackboardKey.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UClearBlackboardKey : public UBTTask_GameplayTaskBase
{
	GENERATED_BODY()

public:
	UClearBlackboardKey();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	// Blackboard Key to clear
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Blackboard)
	FBlackboardKeySelector BlackboardKey;
};
