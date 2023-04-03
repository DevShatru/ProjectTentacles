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

private:

	// Attack Animation Timeline
	// Timeline
	FTimeline ShortFlipKickTimeLine;
	FTimeline FlyingKickTimeLine;
	FTimeline FlyingPunchTimeLine;
	FTimeline SpinKickTimeLine;
	FTimeline DashingDoubleKickTimeLine;
	FTimeline CloseToPerformFinisherTimeLine;
	FTimeline DodgeLerpingTimeLine;

	// Stored positions for later lerp usage
	FVector MovingStartPos;
	FVector MovingEndPos;
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack_Setting)
	float MaxAngleForFacingEnemy = 45.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack_Setting)
	float DetectionRange = 400.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	TArray<TEnumAsByte<EObjectTypeQuery>> FilterType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	UClass* FilteringClass;
	
	// ================================================= Combat Variable Setting ================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Combat_AnimMontages)
	UAnimMontage* EvadeAnimMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Combat_AnimMontages)
	TArray<UAnimMontage*> MeleeAttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Combat_AnimMontages)
	UAnimMontage* FinisherAnimMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat_AnimMontages)
	UAnimMontage* ReceiveDamageMontage;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Combat_AnimMontages)
	UAnimMontage* CounterAttackMontages;

	// ================================================= Dodge Variable Setting ================================================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dodge_Setting)
	float DodgeDistance = 250.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dodge_Setting)
	UCurveFloat* DodgeLerpingCurve;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Dodge_Setting)
	UAnimMontage* BackFlipMontage;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Dodge_Setting)
	UAnimMontage* FrontRollingMontage;
	
	
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
	void TryToUpdateTarget();
	
	TArray<AAttackTargetTester*> GetAllOpponentAroundSelf();

	void InstantRotation(FVector RotatingVector);

	AAttackTargetTester* GetTargetEnemy(TArray<AAttackTargetTester*> OpponentsAroundSelf);

	static bool IsPlayerCountering(EActionState PlayerCurrentAction, EEnemyAttackType ReceivingAttackType);
	static bool IsPlayerCanBeDamaged(EActionState PlayerCurrentAction, EEnemyAttackType ReceivingAttackType);


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
	
	
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	

};
