// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Characters/Base/BaseCharacter.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCharacter.generated.h"


USTRUCT(BlueprintType)
struct FInputDirection
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float InputDirectionX;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float InputDirectionY;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PreviousInputDirectionX;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PreviousInputDirectionY;
	
	
public:
	
	FInputDirection()
	{
		PreviousInputDirectionX = 0;
		PreviousInputDirectionY = 0;
		InputDirectionX = 0;
		InputDirectionY = 0;
	}

	float GetInputDirectionX() const {return InputDirectionX;}
	float GetInputDirectionY() const {return InputDirectionY;}
	void SetInputDirectionX(float InputX) {InputDirectionX = InputX;}
	void SetInputDirectionY(float InputY) {InputDirectionY = InputY;}

	float GetPreviousInputDirectionX() const {return PreviousInputDirectionX;}
	float GetPreviousInputDirectionY() const {return PreviousInputDirectionY;}
	void SetPreviousInputDirectionX(float NewInputX) {PreviousInputDirectionX = NewInputX;}
	void SetPreviousInputDirectionY(float NewInputY) {PreviousInputDirectionY = NewInputY;}
};


DECLARE_DYNAMIC_DELEGATE_OneParam(FOnExecutingPlayerAction, EActionState, ExecutingAction);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnReceivingIncomingDamage, int32, DamageAmount, AActor*, DamageCauser, EEnemyAttackType, ReceivingAttackType);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTriggeringCounter, AActor*, DamageCauser);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnEnteringPreCounterState, AActor*, CounterTarget);

DECLARE_DYNAMIC_DELEGATE(FOnActionFinish);


/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API APlayerCharacter : public ABaseCharacter, public IGenericTeamAgentInterface, public IDamageInterface
{
	GENERATED_BODY()

private:
	static FGenericTeamId TeamId;
	virtual FGenericTeamId GetGenericTeamId() const override;

	void StopRegenerateStamina();
	void WaitToRegenStamina();
	void BeginRegenerateStamina();
	void RegeneratingStamina();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;


	// Current playing reference to be check if valid
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* CurrentPlayingMontage;

	// Actor variables to set targeting and damaging actor for the incoming damage 
	UPROPERTY()
	AEnemyBase* TargetActor;

	UPROPERTY()
	AEnemyBase* DamagingActor;

	UPROPERTY()
	AEnemyBase* CounteringVictim;

	bool IsPlayerCounterable = false;
	
	
	UPROPERTY()
	EPlayerAttackType CurrentAttackType;

	// Animation montages
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= AnimMontages)
	UAnimMontage* EvadeAnimMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= AnimMontages)
	TArray<UAnimMontage*> MeleeAttackMontages;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= StaminaSetting)
	float CurrentStamina = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= StaminaSetting)
	float MaxStamina = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= StaminaSetting)
	float CostForEachDodge = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= StaminaSetting)
	float MinTimeToStartRegen = 3.0f;

	float StaminaRegenTickTime = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= StaminaSetting)
	float StaminaRegenPerSecond = 10.0f;

	// Timer handle for waiting to regen and regening
	FTimerHandle RegenWaitingTimerHandle;
	FTimerHandle RegenStaminaTimerHandle;
	
	
	// input key variables to check
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputKey)
	FKey MovingForwardKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputKey)
	FKey MovingBackKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputKey)
	FKey MovingLeftKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputKey)
	FKey MovingRightKey;

	// Current Input direction structure	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInputDirection InputDirection;
	
	
	// Register as visual stimulus for enemies
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UAIPerceptionStimuliSourceComponent* StimuliSource;

public:

	// Delegate signature
	FOnExecutingPlayerAction OnExecutePlayerAction;
	FOnReceivingIncomingDamage OnReceivingIncomingDamage;
	FOnTriggeringCounter OnTriggeringCounter;
	FOnEnteringPreCounterState OnEnteringPreCounterState;
	FOnActionFinish OnActionFinish;
	
	APlayerCharacter();

	// ================================================= 
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	

	// ================================================= Input ===================================================
	void LookUpAtRate(float Rate);
	
	virtual void TurnAtRate(float Rate);

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void TryMeleeAttack();
	
	void TryEvade();
	
	void TryDodge();

	bool CheckCanPerformAction();

	// ================================================= Utility Functions ================================================

	void UnsetCurrentTarget();

	
	// ================================================= Get And Set Functions ============================================
	FInputDirection GetPlayerInputDir() const {return InputDirection;}
	

	float GetCurrentStamina() const {return CurrentStamina;}
	void SetStamina(float NewStamina) {CurrentStamina = NewStamina;}
	
	AEnemyBase* GetTargetActor() const {return TargetActor;}
	void SetTargetActor(AEnemyBase* NewTargetActor);

	AEnemyBase* GetDamagingActor() const {return DamagingActor;}
	void SetDamagingActor(AEnemyBase* NewDamagingActor) {DamagingActor = NewDamagingActor;}

	bool GetIsPlayerCounterable() const {return IsPlayerCounterable;}
	void TryTurnCounterCapable(bool IsOn) {if(IsPlayerCounterable != IsOn) IsPlayerCounterable = IsOn;}

	
	AEnemyBase* GetCounteringTarget() const {return CounteringVictim;}
	void SetCounteringTarget(AEnemyBase* NewCounterTarget) {if(CounteringVictim != NewCounterTarget) CounteringVictim = NewCounterTarget;}
	void ClearCounteringTarget(AEnemyBase* RemovingCounterTarget) {if(CounteringVictim == RemovingCounterTarget) CounteringVictim = nullptr;}

	EPlayerAttackType GetCurrentAttackType() const {return CurrentAttackType;}
	void SetCurrentAttackType(EPlayerAttackType NewAttackType) {CurrentAttackType = NewAttackType;}

	int32 GetCurrentCharacterHealth() const {return CharacterCurrentHealth;}
	void HealthReduction(int32 ReducingAmount) {CharacterCurrentHealth = UKismetMathLibrary::Clamp((CharacterCurrentHealth - ReducingAmount),0, CharacterMaxHealth);}

	// ================================================= Interface implementation =========================================
	
	UFUNCTION()
	virtual void DamagingTarget_Implementation() override;

	// UFUNCTION()
	// virtual void ReceiveAttackInCounterState_Implementation(AActor* CounteringTarget) override;
	
	UFUNCTION()
	virtual void TryStoreCounterTarget_Implementation(AEnemyBase* CounterTarget) override;

	UFUNCTION()
	virtual void TryRemoveCounterTarget_Implementation(AEnemyBase* CounterTarget) override;
	
	UFUNCTION()
	virtual void ReceiveDamageFromEnemy_Implementation(int32 DamageAmount, AActor* DamageCauser, EEnemyAttackType EnemyAttackType) override;

	UFUNCTION()
	virtual void ActionEnd_Implementation(bool BufferingCheck) override;

	virtual void DetachEnemyTarget_Implementation() override;
	

	
};
