// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Base/AttackTargetTester.h"
#include "Characters/Base/BaseCharacter.h"
#include "PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class EPlayerAttackType: uint8
{
	ShortFlipKick = 0 UMETA(DisplayName = "ShortFlipKick"),
	FlyingKick = 1 UMETA(DisplayName = "FlyingKick"),
	FlyingPunch = 2 UMETA(DisplayName = "FlyingPunch"),
	SpinKick = 3 UMETA(DisplayName = "SpinKick"),
	DashingDoubleKick = 4 UMETA(DisplayName = "FlyingKick"),
};

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

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

private:
	void InitializeTimeLineComp();

	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void LookUpAtRate(float Rate);
	
	virtual void TurnAtRate(float Rate);

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	// ================================================== Melee Attack ================================================
	UFUNCTION()
	void TryMeleeAttack();
	
	void BeginMeleeAttack();

	void FinishEnemy();
	
	void SetAttackMovementPositions(FVector TargetPos);
	
	UFUNCTION()
	void MovingAttackMovement(float Alpha);
	
	EPlayerAttackType GetAttackTypeByRndNum(int32 RndNum);
	
	void StartAttackMovementTimeline(EPlayerAttackType CurrentAttackType);
	
	// ====================================================== Evade ===================================================
	UFUNCTION()
	void TryEvade();

	void BeginEvade();

	// ================================================== Counter ======================================================
	void BeginCounterAttack(AActor* CounteringTarget);

	// ================================================== Dodge ========================================================
	void TryDodge();

	void BeginDodge();

	FVector DecideDodgingDirection(FVector PlayerFaceDir);

	UAnimMontage* DecideDodgingMontage(FVector PlayerDodgingDirection);
	
	// ================================================== Utility ======================================================
	TArray<AAttackTargetTester*> GetAllOpponentAroundSelf();

	 void InstantRotation(FVector RotatingVector);

	AAttackTargetTester* GetTargetEnemy(TArray<AAttackTargetTester*> OpponentsAroundSelf);
	


	
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovingAttackCurve)
	UCurveFloat* ShortFlipKickCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovingAttackCurve)
	UCurveFloat* FlyingKickCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovingAttackCurve)
	UCurveFloat* FlyingPunchCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovingAttackCurve)
	UCurveFloat* SpinKickCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovingAttackCurve)
	UCurveFloat* DashingDoubleKickCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MovingAttackCurve)
	UCurveFloat* CloseToPerformFinisherCurve;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputKey)
	FKey MovingForwardKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputKey)
	FKey MovingBackKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputKey)
	FKey MovingLeftKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputKey)
	FKey MovingRightKey;


	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float MaxAngleForFacingEnemy = 45.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float DetectionRange = 400.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting)
	TArray<TEnumAsByte<EObjectTypeQuery>> FilterType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting)
	UClass* FilteringClass;

	UPROPERTY()
	AAttackTargetTester* TargetActor;

	UPROPERTY()
	EPlayerAttackType CurrentAttackType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=FinisherMontages)
	UAnimMontage* FinisherAnimMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DamageReceiveMontages)
	UAnimMontage* ReceiveDamageMontage;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= DamageReceiveMontages)
	UAnimMontage* CounterAttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DodgeSetting)
	float DodgeDistance = 250.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DodgeSetting)
	UCurveFloat* DodgeLerpingCurve;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= DodgeSetting)
	UAnimMontage* BackFlipMontage;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= DodgeSetting)
	UAnimMontage* FrontRollingMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FInputDirection InputDirection;

	FTimerHandle ForwardInputTimerHandle;
	FTimerHandle RightInputTimerHandle;

	
	FVector MovingStartPos;
	
	FVector MovingEndPos;

	
	

	// Attack Animation Timeline
	// Timeline
	FTimeline ShortFlipKickTimeLine;
	FTimeline FlyingKickTimeLine;
	FTimeline FlyingPunchTimeLine;
	FTimeline SpinKickTimeLine;
	FTimeline DashingDoubleKickTimeLine;
	FTimeline CloseToPerformFinisherTimeLine;
	FTimeline DodgeLerpingTimeLine;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting)
	FVector CurrentInputForward = FVector(0,0,0);

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting)
	FVector CurrentInputRight = FVector(0,0,0);
	
public:

	APlayerCharacter();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION()
	virtual void DamagingTarget_Implementation() override;
};
