// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Base/EnemyRangeInterface.h"
#include "Characters/Enemies/EnemyBase.h"
#include "EnemyRanged.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API AEnemyRanged : public AEnemyBase, public IEnemyRangeInterface
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting_Range)
	float AimTimeToShoot = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting_Range)
	float AimingRange = 1500.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting_Range)
	UAnimMontage* KneelDownToAimAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting_Range)
	UAnimMontage* KneelAimingLoopAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting_Range)
	UAnimMontage* KneelFireAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting_Range)
	UAnimMontage* StandUpAnim;

	
	FTimerHandle AimingTimerHandle;
	FTimerHandle CheckInSightTimerHandle;

	float CheckInSightTick = 0.01f; 
	

	UFUNCTION()
	void BeginFire();

	UFUNCTION()
	void InSightConditionUpdate();

	virtual void OnDeath() override;

public:


	virtual void ExecuteAttack() override;

	// ================================================== Interface Functions ============================================

	virtual void OnRifleBeginAiming_Implementation() override;

	virtual void OnRifleFinishFiring_Implementation() override;
	
	virtual void TryToDamagePlayer_Implementation() override;

	virtual void ReceiveDamageFromPlayer_Implementation(float DamageAmount, AActor* DamageCauser, EPlayerAttackType PlayerAttackType) override;

	

private:

	ACharacter* PlayerRef = nullptr; 

	AActor* GetDamageActorByLineTrace();

	void StopAimingTimer();

	void StopCheckInSightTimer();

	bool TryCachePlayerRef();
	
	void SpawnOrCollapsePlayerHUD(bool Spawn);
	
	void ShowOrHidePlayerHUD(bool Show);

	bool CheckCanSeePlayer();
	
};
