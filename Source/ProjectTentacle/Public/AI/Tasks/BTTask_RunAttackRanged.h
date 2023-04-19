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
	virtual void AttackCounterTimeLimit() override;
	
};
