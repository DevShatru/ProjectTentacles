// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Components/TimelineComponent.h"
#include "EnemyMelee.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API AEnemyMelee : public AEnemyBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Healing")
	bool bShouldHealOnDeath = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Healing")
	float HealAmount = 15.f;

	virtual void OnDeath() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	float AttackMovingDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float OffsetFromPlayer = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float CounterTriggerRadius = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float CounterTriggerHeight = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float DamageTriggerRadius = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float DamageTriggerHeight = 10.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting_Movement)
	UCurveFloat* UncounterableAttackMovingCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting_Movement)
	UCurveFloat* CounterableAttackMovingCurve;


	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting_Countered)
	UAnimMontage* CounterVictimMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting_Countered)
	float TimeToGetUp = 3.0f;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting_Countered)
	UAnimMontage* GetUpMontage;
	
	


	// ===================================================== Attack ====================================================

	FVector CalculateDestinationForAttackMoving(FVector PlayerPos);

	void PlaySpecificAttackMovingTimeline(EEnemyAttackType AttackType);

	// ===================================================== Stunning ===========================================

	
	
	void BeginLyingCountDown();

	void PlayLyingMontage();

	void RecoverFromLying();

	// ===================================================== Utility ===================================================
	TArray<AActor*> GetActorsInFrontOfEnemy(bool IsDamaging);

	

public:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void ExecuteAttack() override;
	void StartCounteredAnimation();
	
	// ============================================= Timeline function ====================================================
	UFUNCTION()
	void UpdateAttackingPosition(float Alpha);
	

	// ============================================= Get and Set functions ================================================
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsCountered() const {return IsCountered;}
	void SetIsCountered(bool NewIsCountered) {IsCountered = NewIsCountered;}

	
	

	// ================================================== Interface Functions ============================================
	virtual void TryToDamagePlayer_Implementation() override;
	
	virtual void TryTriggerPlayerCounter_Implementation() override;

	virtual void OnCounterTimeEnd_Implementation() override;

	virtual void OnSetIsCountered_Implementation(bool Countered) override;
	
	virtual void OnStartCounteredAnimation_Implementation() override;

	virtual void StartLyingOnTheGround_Implementation() override;

	virtual void RepeatLyingOnTheGround_Implementation() override;

	
	virtual void ReceiveDamageFromPlayer_Implementation(float DamageAmount, AActor* DamageCauser, EPlayerAttackType PlayerAttackType) override;
	
private:
	void InitializeTimelineComp();
	void TryStopAttackMovement();


	// bool to check if enemy is in counter state
	bool IsCountered = false;
	

	// Timeline for enemy attack movement
	FTimeline UnCounterMovingTimeline;
	FTimeline CounterableMovingTimeline;

	// Class variables for timeline function usage
	FVector AttackMovingDestination;
	FVector SelfAttackStartPos;
	
	
};
