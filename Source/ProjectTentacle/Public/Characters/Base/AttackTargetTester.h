// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterActionInterface.h"
#include "Characters/Base/Widget_EnemyAttackIndicator.h"
#include "DamageInterface.h"
#include "EnemyWidgetInterface.h"
#include "Widget_EnemyTargetIconWidget.h"
#include "Components/TimelineComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "AttackTargetTester.generated.h"


DECLARE_DYNAMIC_DELEGATE_TwoParams(FOldOnUpdatingEnemyAttackType, EEnemyAttackType, NewAttackType, EEnemyAttackAnimMontages, NewAttackAnim);

UCLASS()
class PROJECTTENTACLE_API AAttackTargetTester : public ACharacter, public ICharacterActionInterface, public IDamageInterface, public IEnemyWidgetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAttackTargetTester();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	FOldOnUpdatingEnemyAttackType OnUpdatingEnemyAttackIndicator;

	UPROPERTY()
	UWidget_EnemyAttackIndicator* AttackIndicatorRef;

	UPROPERTY()
	UWidget_EnemyTargetIconWidget* EnemyTargetWidgetRef;
	
	UFUNCTION(BlueprintCallable)
	void ExecuteAttack();
	
	EEnemyAttackAnimMontages SetAttackType();

	FVector CalculateDestinationForAttackMoving(FVector PlayerPos);
	
	// 
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* EnemyAttackIndicatorWidgetComponent;

	// 
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* EnemyTargetedIconWidgetComponent;


	


	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ReceiveDamageAnimations)
	UAnimMontage* ReceiveShortFlipKick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ReceiveDamageAnimations)
	UAnimMontage* ReceiveFlyingKick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ReceiveDamageAnimations)
	UAnimMontage* ReceiveFlyingPunch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ReceiveDamageAnimations)
	UAnimMontage* ReceiveSpinKick;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ReceiveDamageAnimations)
	UAnimMontage* ReceiveDashingDoubleKick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FinishedAnimations)
	UAnimMontage* FinishedAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnemyProperty)
	int32 Health = 3;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	TArray<TEnumAsByte<EObjectTypeQuery>> FilterType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	UClass* FilteringClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	TSubclassOf<UDamageType> DamageType;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	EEnemyAttackType CurrentAttackType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	int32 BaseDamageAmount = 2;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	float AttackMovingDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	float OffsetFromPlayer = 50.0f;
	

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Animations)
	UAnimMontage* CounterableAttackMontage;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Animations)
	UAnimMontage* NotCounterableAttackMontage;

	FTimeline AttackMovingTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	UCurveFloat* AttackMovingCurve;

	FVector AttackMovingDestination;

	FVector SelfAttackStartPos;

	
	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void InstantRotation(FVector RotatingVector);

	void PlayDamageReceiveAnimation(int32 AttackTypIndex);

	void PlayFinishedAnimation();
	

	// ============================================= Timeline function ====================================================

	UFUNCTION()
	void UpdateAttackingPosition(float Alpha);
	

	// ============================================= Get and Set functions ================================================
	int32 GetEnemyHealth() const { return Health;}
	void SetEnemyHealth(int32 NewHealth) {Health = NewHealth;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EEnemyAttackType GetEnemyStoredAttackType() const {return CurrentAttackType;}

	// ================================================== Interface Functions ============================================
	virtual void TryToDamagePlayer_Implementation() override;

	virtual void ReceiveDamageFromPlayer_Implementation(float DamageAmount, AActor* DamageCauser, EPlayerAttackType PlayerAttackType) override;

	virtual void ShowEnemyAttackIndicator_Implementation() override;

	virtual void UnShowEnemyAttackIndicator_Implementation() override;

	virtual void ShowPlayerTargetIndicator_Implementation() override;
	
	virtual void UnShowPlayerTargetIndicator_Implementation() override;
	
	// ==================================== Testing Functions ======================================================

	
};

