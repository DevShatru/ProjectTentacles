// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RunAttackMelee.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_RunAttackMelee : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_RunAttackMelee();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// Testing methods to ensure queue timing is working
	// TODO: replace with anim notifies when set up
	UFUNCTION()
	void AttackCounterTimeLimit();
	UFUNCTION()
	void AttackCompletionTimeLimit();
	
private:
	UPROPERTY()
	UBehaviorTreeComponent* OwningComp;

	class AEnemyBase* OwnPawn;

	FTimerManager* WorldTimerManager;
};
