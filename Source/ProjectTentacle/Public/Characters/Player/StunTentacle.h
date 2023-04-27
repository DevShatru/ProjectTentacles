// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/BaseTentacle.h"
#include "StunTentacle.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API AStunTentacle : public ABaseTentacle
{
	GENERATED_BODY()

protected:

	FTimerHandle AbsorbingEnemyTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeToExecuteStun = 5.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AbsorbingRadius = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AbsorbingPower = 1000.0f;

	float CurrentExecutionTime = 0;
	
	float EachAbsorbPerTick = 0.05f;


	virtual void LifeCycleBegin() override;

	UFUNCTION()
	void AbsorbEnemy();
	
	UFUNCTION()
	void StunEnemy();

	virtual void OnLifeCycleEnd() override;	

public:
	
	
};
