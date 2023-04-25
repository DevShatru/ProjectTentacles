// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "PlayerActionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class PROJECTTENTACLE_API UPlayerActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerActionComponent();

private:

	void StopSpecificMovingTimeline(EPlayerAttackType CurrentPlayerAttack);

	void PauseComboResetTimer();
	void ResumeComboResetTimer();

	void ClearSpecialAbilityCDTimer();
	void StartSpecialAbilityCDTimer();
	
	// Attack Animation Timeline
	// Timeline
	FTimeline ShortFlipKickTimeLine;
	FTimeline FlyingKickTimeLine;
	FTimeline FlyingPunchTimeLine;
	FTimeline SpinKickTimeLine;
	FTimeline DashingDoubleKickTimeLine;
	FTimeline CloseToPerformFinisherTimeLine;
	FTimeline DodgeLerpingTimeLine;
	FTimeline TurnRotationTimeline;
	
	// Stored positions for later lerp usage
	FVector MovingStartPos;
	FVector MovingEndPos;

	FVector CounterMoveStartPos;
	FVector CounterMoveEndPos;
	
	float StartTurnRotationZ;
	float EndTurnRotationZ;

	
	
protected:


	// Owner Ref
	UPROPERTY()
	APlayerCharacter* PlayerOwnerRef;
	
	UPROPERTY()
	UAnimMontage* CurrentPlayingMontage;

	// ================================================= Attack Variable Setting ================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack_MovingCurve)
	UCurveFloat* ShortFlipKickCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack_MovingCurve)
	UCurveFloat* FlyingKickCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack_MovingCurve)
	UCurveFloat* FlyingPunchCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack_MovingCurve)
	UCurveFloat* SpinKickCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack_MovingCurve)
	UCurveFloat* DashingDoubleKickCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack_MovingCurve)
	UCurveFloat* CloseToPerformFinisherCurve;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	float MaxDistanceToBeClose = 200.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack_Setting)
	float MaxAngleForFacingEnemy = 45.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack_Setting)
	float DetectionRange = 400.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	TArray<TEnumAsByte<EObjectTypeQuery>> FilterType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	UClass* FilteringClass;


	
	// ================================================= Combo Setting ==========================================================
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_ComboSetting)
	int32 CurrentComboCount = 0;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_ComboSetting)
	float ComboSpeedMultiplier = 0.3f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_ComboSetting)
	float MaxComboSpeedBonus = 1.5f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_ComboSetting)
	float ComboCountExistTime = 5.0f;	

	FTimerHandle ComboResetTimerHandle;

	// ================================================= Combat Variable Setting ================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Combat_AnimMontages)
	UAnimMontage* EvadeAnimMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Combat_AnimMontages)
	TArray<UAnimMontage*> MeleeAttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Combat_AnimMontages)
	TArray<UAnimMontage*> CloseMeleeAttackMontages;

	UPROPERTY()
	UAnimMontage* LastMeleeMontage;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Combat_AnimMontages)
	UAnimMontage* FinisherAnimMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat_AnimMontages)
	UAnimMontage* ReceiveDamageMontage;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Combat_AnimMontages)
	UAnimMontage* CounterAttackMontages;


	// ================================================= PreCounter =============================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= PreCounterSetting)
	UCurveFloat* CounterRotationCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= PreCounterSetting)
	UAnimMontage* RotateAnimationLeft;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= PreCounterSetting)
	UAnimMontage* RotateAnimationRight;
	
	// ================================================= Dodge Variable Setting ================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dodge_Setting)
	float DodgeDistance = 250.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dodge_Setting)
	UCurveFloat* DodgeLerpingCurve;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Dodge_Setting)
	UAnimMontage* BackFlipMontage;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Dodge_Setting)
	UAnimMontage* FrontRollingMontage;

	// ================================================= Damage Receiving Setting =============================================

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= DamageReceive_Setting)
	float DamageReceiveAnimDistance = 100.0f;


	// ================================================ Special Ability Setting =======================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpecialAbilitySetting)
	float SpecialAbilityCooldown1 = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpecialAbilitySetting)
	float SpecialAbilityCooldown2 = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpecialAbilitySetting)
	float SpecialAbilityCooldown3 = 15.0f;

	UPROPERTY(BlueprintReadOnly)
	float CurrentSpecialMeter1 = 100.0f;

	UPROPERTY(BlueprintReadOnly)
	float CurrentSpecialMeter2 = 100.0f;

	UPROPERTY(BlueprintReadOnly)
	float CurrentSpecialMeter3 = 100.0f;

	float MaxSpecialMeter = 100.0f;
	
	FTimerHandle SpecialAbilityCDTimerHandle;
	
	
	
	// ================================================== Melee Attack ================================================
	void BeginMeleeAttack();
	
	void PerformLongRangeMelee(AEnemyBase* RegisteredTarget);

	void PerformCloseRangeMelee(AEnemyBase* RegisteredTarget);

	int32 GetDifferentCloseMeleeMontage(TArray<UAnimMontage*> ListOfMeleeMontages);
	
	bool TargetDistanceCheck(AEnemyBase* Target);

	void ComboCountIncrement();

	void FinishEnemy();
	
	void SetAttackMovementPositions(FVector TargetPos);

	void UpdateTargetPosition();
	
	UFUNCTION()
	void MovingAttackMovement(float Alpha);

	UFUNCTION()
	void DodgeMovement(float Alpha);
	
	EPlayerAttackType GetAttackTypeByRndNum(int32 RndNum);
	
	void StartAttackMovementTimeline(EPlayerAttackType CurrentAttackType);

	float CalculateCurrentComboSpeed();

	UFUNCTION()
	void OnStartingComboResetTimer();
	
	void WaitToResetComboCount();

	UFUNCTION()
	void ResetComboCount() {CurrentComboCount = 0;}

	void ClearComboResetTimer();
	
	// ====================================================== Evade ===================================================
	void BeginEvade();

	// ================================================ Pre Counter ====================================================
	void EnterPreCounterState();

	UFUNCTION()
	void EnterCounterAttackState();
	
	// ================================================== Counter ======================================================
	void BeginCounterAttack();

	FVector GetCounterPos(AEnemyBase* CounterVictim);

	UFUNCTION()
	void TurnRotationUpdate(float Alpha);
	
	// ================================================== Dodge ========================================================
	void BeginDodge();

	FVector DecideDodgingDirection(FVector PlayerFaceDir);

	UAnimMontage* DecideDodgingMontage(FVector PlayerDodgingDirection);

	// =============================================== Special Ability ===================================================

	void ExecuteSpecialAbility(int32 AbilityIndex);

	void ResetAbilityMeters();

	UFUNCTION()
	void RestoreAbilitiesInTick();
	
	// ================================================== Utility ======================================================
	void TryToUpdateTarget();
	
	TArray<AEnemyBase*> GetAllOpponentAroundSelf();
	
	void InstantRotation(FVector RotatingVector);

	AEnemyBase* GetTargetEnemy(TArray<AEnemyBase*> OpponentsAroundSelf);

	static bool IsPlayerCountering(EActionState PlayerCurrentAction, EEnemyAttackType ReceivingAttackType);
	static bool IsPlayerCanBeDamaged(EActionState PlayerCurrentAction, EEnemyAttackType ReceivingAttackType);

	void MakePlayerEnemyFaceEachOther(AEnemyBase* TargetEnemyRef);

	// ================================ Functions called when the game starts ==========================================
	virtual void BeginPlay() override;
	void InitializeOwnerRef();
	void InitializeTimelineComp();

	
	// ============================================= Tick function =====================================================
	
	
	
	
public:

	

	// ================================================= Delegate Functions =================================================
	UFUNCTION()
	void ExecutePlayerAction(EActionState ExecutingAction);
	
	UFUNCTION()
	void ReceivingDamage(int32 DamageAmount, AActor* DamageCauser, EEnemyAttackType ReceivingAttackType);
	
	UFUNCTION()
	void TriggerPreCounter(AActor* CounteringTarget);

	
	UFUNCTION()
	void TriggerCounterAttack(AActor* CounteringTarget);
	
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	

};
