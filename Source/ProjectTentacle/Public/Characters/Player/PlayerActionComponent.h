// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "Components/ActorComponent.h"
#include "PlayerActionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class PROJECTTENTACLE_API UPlayerActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerActionComponent();

protected:
	
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
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* CurrentPlayingMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AnimMontages)
	UAnimMontage* EvadeAnimMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AnimMontages)
	TArray<UAnimMontage*> MeleeAttackMontages;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float MaxAngleForFacingEnemy = 45.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float DetectionRange = 400.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting)
	TArray<TEnumAsByte<EObjectTypeQuery>> FilterType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting)
	UClass* FilteringClass;
	

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
	
	FVector MovingStartPos;
	
	FVector MovingEndPos;

	
	// ================================================== Melee Attack ================================================
	void BeginMeleeAttack();

	void FinishEnemy();
	
	void SetAttackMovementPositions(FVector TargetPos);
	
	UFUNCTION()
	void MovingAttackMovement(float Alpha);
	
	EPlayerAttackType GetAttackTypeByRndNum(int32 RndNum);
	
	void StartAttackMovementTimeline(EPlayerAttackType CurrentAttackType);
	
	// ====================================================== Evade ===================================================
	void BeginEvade();

	// ================================================== Counter ======================================================
	void BeginCounterAttack(AActor* CounteringTarget);

	// ================================================== Dodge ========================================================

	void BeginDodge();

	FVector DecideDodgingDirection(FVector PlayerFaceDir);

	UAnimMontage* DecideDodgingMontage(FVector PlayerDodgingDirection);
	
	// ================================================== Utility ======================================================
	TArray<AAttackTargetTester*> GetAllOpponentAroundSelf();

	void InstantRotation(FVector RotatingVector);

	AAttackTargetTester* GetTargetEnemy(TArray<AAttackTargetTester*> OpponentsAroundSelf);
	
	
	

	// Attack Animation Timeline
	// Timeline
	FTimeline ShortFlipKickTimeLine;
	FTimeline FlyingKickTimeLine;
	FTimeline FlyingPunchTimeLine;
	FTimeline SpinKickTimeLine;
	FTimeline DashingDoubleKickTimeLine;
	FTimeline CloseToPerformFinisherTimeLine;
	FTimeline DodgeLerpingTimeLine;


	// Called when the game starts
	virtual void BeginPlay() override;
	void InitializeOwnerRef();
	void InitializeTimelineComp();

	UPROPERTY()
	APlayerCharacter* PlayerOwnerRef;

	
public:


	UFUNCTION()
	void ExecutePlayerAction(EActionState ExecutingAction);
	
	UFUNCTION()
	void ReceivingDamage(int32 DamageAmount, AActor* DamageCauser, EEnemyAttackType ReceivingAttackType);
	
	
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	

};