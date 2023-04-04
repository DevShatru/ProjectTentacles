// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Characters/Base/AttackTargetTester.h"
#include "Characters/Base/BaseCharacter.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Components/TimelineComponent.h"
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
	
	
public:
	
	FInputDirection()
	{
		InputDirectionX = 0;
		InputDirectionY = 0;
	}

	float GetInputDirectionX() const {return InputDirectionX;}
	float GetInputDirectionY() const {return InputDirectionY;}
	void SetInputDirectionX(float InputX) {InputDirectionX = InputX;}
	void SetInputDirectionY(float InputY) {InputDirectionY = InputY;}
};


DECLARE_DYNAMIC_DELEGATE_OneParam(FOnExecutingPlayerAction, EActionState, ExecutingAction);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnReceivingIncomingDamage, int32, DamageAmount, AActor*, DamageCauser, EEnemyAttackType, ReceivingAttackType);


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
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* CurrentPlayingMontage;

	UPROPERTY()
	AEnemyBase* TargetActor;

	UPROPERTY()
	AEnemyBase* DamagingActor;
	

	UPROPERTY()
	EPlayerAttackType CurrentAttackType;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= AnimMontages)
	UAnimMontage* EvadeAnimMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= AnimMontages)
	TArray<UAnimMontage*> MeleeAttackMontages;

	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputKey)
	FKey MovingForwardKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputKey)
	FKey MovingBackKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputKey)
	FKey MovingLeftKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputKey)
	FKey MovingRightKey;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInputDirection InputDirection;
	
	// Register as visual stimulus for enemies
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UAIPerceptionStimuliSourceComponent* StimuliSource;

public:

	FOnExecutingPlayerAction OnExecutePlayerAction;

	FOnReceivingIncomingDamage OnReceivingIncomingDamage;

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

	// ================================================= Utility Functions ================================================



	
	// ================================================= Get And Set Functions ============================================
	FInputDirection GetPlayerInputDir() const {return InputDirection;}

	AEnemyBase* GetTargetActor() const {return TargetActor;}
	void SetTargetActor(AEnemyBase* NewTargetActor);

	AEnemyBase* GetDamagingActor() const {return DamagingActor;}
	void SetDamagingActor(AEnemyBase* NewDamagingActor) {DamagingActor = NewDamagingActor;}

	EPlayerAttackType GetCurrentAttackType() const {return CurrentAttackType;}
	void SetCurrentAttackType(EPlayerAttackType NewAttackType) {CurrentAttackType = NewAttackType;}

	int32 GetCurrentCharacterHealth() const {return CharacterCurrentHealth;}
	void HealthReduction(int32 ReducingAmount) {CharacterCurrentHealth = UKismetMathLibrary::Clamp((CharacterCurrentHealth - ReducingAmount),0, CharacterMaxHealth);}

	// ================================================= Interface implementation =========================================
	
	UFUNCTION()
	virtual void DamagingTarget_Implementation() override;

	UFUNCTION()
	virtual void ReceiveDamageFromEnemy_Implementation(int32 DamageAmount, AActor* DamageCauser, EEnemyAttackType EnemyAttackType) override;
};
