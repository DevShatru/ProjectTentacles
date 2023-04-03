// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterActionInterface.h"
#include "Characters/Base/Widget_EnemyAttackIndicator.h"
#include "DamageInterface.h"
#include "EnemyWidgetInterface.h"
#include "Widget_EnemyTargetIconWidget.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "AttackTargetTester.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUpdatingEnemyAttackType, EEnemyAttackType, NewAttackType);

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

	
	FOnUpdatingEnemyAttackType OnUpdatingEnemyAttackIndicator;

	UPROPERTY()
	UWidget_EnemyAttackIndicator* AttackIndicatorRef;

	UPROPERTY()
	UWidget_EnemyTargetIconWidget* EnemyTargetWidgetRef;
	
	UFUNCTION(BlueprintCallable)
	void ExecuteAttack();
	
	void SetAttackType();
	

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

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting)
	TArray<TEnumAsByte<EObjectTypeQuery>> FilterType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting)
	UClass* FilteringClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting)
	TSubclassOf<UDamageType> DamageType;

	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting)
	EEnemyAttackType CurrentAttackType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting)
	int32 BaseDamageAmount = 2;


	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackAnimations)
	UAnimMontage* CounterableAttackMontage;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackAnimations)
	UAnimMontage* NotCounterableAttackMontage;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void InstantRotation(FVector RotatingVector);

	void PlayDamageReceiveAnimation(int32 AttackTypIndex);

	void PlayFinishedAnimation();
	


	// ============================================= Get and Set functions ================================================
	int32 GetEnemyHealth() const { return Health;}
	void SetEnemyHealth(int32 NewHealth) {Health = NewHealth;}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EEnemyAttackType GetEnemyStoredAttackType() const {return CurrentAttackType;}

	// ================================================== Interface Functions ============================================
	virtual void TryToDamagePlayer_Implementation() override;

	virtual void ReceiveDamageFromPlayer_Implementation(int32 DamageAmount, AActor* DamageCauser, EPlayerAttackType PlayerAttackType) override;

	virtual void ShowEnemyAttackIndicator_Implementation() override;

	virtual void UnShowEnemyAttackIndicator_Implementation() override;

	virtual void ShowPlayerTargetIndicator_Implementation() override;
	
	virtual void UnShowPlayerTargetIndicator_Implementation() override;
	
	// ==================================== Testing Functions ======================================================

	
};

