// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyBaseController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API AEnemyBaseController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyBaseController();
	virtual void BeginPlay() override;

protected:
	// Base behavior tree, run on start
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBehaviorTree* BehaviorTree;
};
