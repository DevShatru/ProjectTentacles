// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/BaseTentacle.h"
#include "AttackingTentacle.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECTTENTACLE_API AAttackingTentacle : public ABaseTentacle
{
	GENERATED_BODY()

private:
	void StopAttacking();

	AActor* FindClosestEnemyInRange(TArray<AActor*> EnemiesInRange);

	void DamageInRange(FVector ClosestEnemyPos);
	
protected:

	FTimerHandle AttackEnemyTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttackSetting")
	float AttackingGap = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttackSetting")
	float SearchingRadius = 250.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttackSetting")
	float AttackingRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AttackSetting")
	int32 DamageAmount = 5;
	
	UFUNCTION(BlueprintImplementableEvent)
	void DrawDebugAttackRange(FVector StartPos, float Radius);

	UFUNCTION(BlueprintCallable)
	void AttackEnemy();
	
	virtual void LifeCycleBegin() override;

	virtual void LifeSpanExpired() override;
	
public:
	
	
};
