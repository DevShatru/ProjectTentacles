// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToNavMesh.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_MoveToNavMesh : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_MoveToNavMesh();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Tolerance = 10.f;

private:
	class UNavigationSystemV1* NavSystem;
	ACharacter* Character;
	AAIController* Controller;
	FVector TargetLocation;
	FVector TargetDirection;
};
