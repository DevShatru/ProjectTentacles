// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_StrafeTarget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_StrafeTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_StrafeTarget();
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector TargetKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector CircleDirectionKey;
private:
	void StrafeAroundLocation(FVector Location) const;

	FVector TargetLocation;
	AActor* TargetActor;

	AAIController* OwnController;
	ACharacter* OwnCharacter;
	unsigned int bShouldMoveRight:1;
	unsigned int bIsObject:1;
};
