// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WaitForTrigger.generated.h"

/**
 * TickTask function that never completes, need to be aborted by decorator
 * TODO: Needs to timeout at some point, otherwise there's a danger of an endless wait
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_WaitForTrigger : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_WaitForTrigger();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Precision;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
private:
	
	float TurnSpeed;
	AActor* FocalActor;
	AAIController* Controller;
	APawn* Pawn;

	float CalculateFlatAngle(const FVector& VectorA, const FVector& VectorB);
};
