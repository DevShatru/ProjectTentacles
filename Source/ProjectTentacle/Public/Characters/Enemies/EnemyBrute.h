// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Components/TimelineComponent.h"
#include "EnemyBrute.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECTTENTACLE_API AEnemyBrute : public AEnemyBase
{
	GENERATED_BODY()


private:

	// Class variables for timeline function usage
	FVector AttackMovingDestination;
	FVector SelfAttackStartPos;

	void InitializeTimelineComp();
	void TryStopAttackMovement();


	void PlaySpecificAttackMovingTimeline(EEnemyAttackType EnemyAttack);
	void UpdateAttackDestination();

	FTimerHandle StunningTimerHandle;
	
	
	void PlayReceiveCounterAnimation();
	

	// bool to check if enemy is in counter state
	bool IsCountered = false;

	//
	int32 CounteredTime = 0; 
	

	// Timeline for enemy attack movement
	FTimeline UnCounterMovingTimeline;
	FTimeline CounterableMovingTimeline;

	float RemainAttackDistance = 0;
	float TravelDistancePerTick = 0;
	
protected:

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	float CounterableAttackMovingDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float OffsetFromPlayer = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float ChargingDistance = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float TotalTravelTime_UnCounterable = 1.1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float ToTalTravelTime_Counterable = 0.9;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float CounterTriggerRadius = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float CounterTriggerHeight = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float DamageTriggerRadius = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float DamageTriggerHeight = 10.0f;

	
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float AttackTravelDistance_Counterable = 200.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float AttackTravelDistance_UnCounterable = 800.0f;


	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StunSetting)
	float TotalStunDuration = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StunSetting)
	UAnimMontage* StunAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StunSetting)
	UAnimMontage* RecoverFromStun;

	UPROPERTY(BlueprintReadOnly)
	bool IsStunned = false;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting_Movement)
	UCurveFloat* UncounterableAttackMovingCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting_Movement)
	UCurveFloat* CounterableAttackMovingCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting_Animation)
	UAnimMontage* CouterableAttackSecond;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting_Animation)
	UAnimMontage* UnableCounterAttackSecond;

	

	
	
	virtual void ExecuteAttack() override;

	FVector CalculateDestinationForAttackMoving(FVector PlayerCurrentPos, float CurrentTimelineAlpha);
	
	UFUNCTION()
	void UpdateAttackingPosition(float Alpha);


	
	void OnContinueSecondAttackMontage_Implementation() override;

	UFUNCTION()
	void RecoverFromStunState();
	
	TArray<AActor*> GetActorsInFrontOfEnemy(bool IsDamaging);

	
public:


	// ================================== Get And Set =================================
	void SetIsCountered(bool NewIsCountered) {IsCountered = NewIsCountered;}


	
	UFUNCTION(BlueprintCallable)
	void TestFunction();

	UFUNCTION(BlueprintImplementableEvent)
	void DebugCircle(FVector PlaceToDraw);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;


	virtual void TryToDamagePlayer_Implementation() override;

	virtual void TryTriggerPlayerCounter_Implementation() override;

	virtual void OnCounterTimeEnd_Implementation() override;

	virtual void OnSetIsCountered_Implementation(bool Countered) override;

	virtual void OnStartCounteredAnimation_Implementation() override;

	virtual void ReceiveDamageFromPlayer_Implementation(int32 DamageAmount, AActor* DamageCauser, EPlayerAttackType PlayerAttackType) override;
};
