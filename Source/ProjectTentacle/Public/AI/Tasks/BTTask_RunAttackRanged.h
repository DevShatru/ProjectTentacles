// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/Tasks/BTTask_RunAttackMelee.h"
#include "BTTask_RunAttackRanged.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_RunAttackRanged : public UBTTask_RunAttackMelee
{
	GENERATED_BODY()
	
public:
	UBTTask_RunAttackRanged();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void AttackCounterTimeLimit() override;
	virtual void AttackCompletionTimeLimit() override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackRange = 1500.f;
	
private:
	class APlayerCharacter* Target;
};
