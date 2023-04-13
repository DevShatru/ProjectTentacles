// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBaseController.h"
#include "EnemyType.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Characters/Base/CharacterActionInterface.h"
#include "Characters/Base/DamageInterface.h"
#include "Characters/Base/EnemyWidgetInterface.h"
#include "Characters/Base/Widget_EnemyAttackIndicator.h"
#include "Characters/Base/Widget_EnemyTargetIconWidget.h"
#include "Components/TimelineComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUpdatingEnemyAttackType, EEnemyAttackType, NewAttackType);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnFinishAttackingTask, UBehaviorTreeComponent*, BehaviorTreeReference, bool, bIsSuccess, bool, DoesGetInterupted);

UCLASS()
class PROJECTTENTACLE_API AEnemyBase : public ACharacter, public ICharacterActionInterface, public IDamageInterface, public IEnemyWidgetInterface
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitializeWidgetComponents();
	
	void InitializeEnemyControllerRef();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat)
	EEnemyType UnitType = EEnemyType::Melee;

	UPROPERTY(EditDefaultsOnly, Category=Combat)
	float AttackCompletionTime = 2.0f;
	
	UPROPERTY(EditDefaultsOnly, Category=Combat)
	float AttackCounterableTime = 1.4f;
	

	// Updating Enemy Attack Delegate Signature
	FOnUpdatingEnemyAttackType OnUpdatingEnemyAttackIndicator;
	
	// User Widget Components
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* EnemyAttackIndicatorWidgetComponent;
	
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* EnemyTargetedIconWidgetComponent;

	// Indicator Widget references
	UPROPERTY()
	UWidget_EnemyAttackIndicator* AttackIndicatorRef;

	UPROPERTY()
	UWidget_EnemyTargetIconWidget* EnemyTargetWidgetRef;


	// Enemy Controller, BT, and BB Component reference 
	UPROPERTY()
	AEnemyBaseController* CurrentEnemyBaseController;

	UPROPERTY()
	UBehaviorTreeComponent* BTComponent;
	
	UPROPERTY()
	UBlackboardComponent* BBComponent;


	FTimerHandle GettingUpTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setting_Lying)
	float TimeToGetUp = 3.0f;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setting_Lying)
	UAnimMontage* GetUpMontage;
	
	// Enemy Property variable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnemyProperty)
	int32 Health = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnemyProperty)
	int32 MaxHealth = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnemyProperty)
	EEnemyCurrentState CurrentEnemyState = EEnemyCurrentState::Standing;
	
	// Receiving Damage Animations
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ReceiveDamageAnimations)
	UAnimMontage* FinishedAnimation;


	// Attacking variable to set up or adjust in blueprint
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AttackSetting)
	UCurveFloat* AttackMovingCurve;

	bool IsAttacking = false;
	
	// Attacking Animation
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Animations)
	UAnimMontage* CounterableAttackMontage;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Animations)
	UAnimMontage* CounterVictimMontage;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Animations)
	UAnimMontage* NotCounterableAttackMontage;



	// ===================================================== Attack ====================================================
	EEnemyAttackAnimMontages SetAttackType();

	FVector CalculateDestinationForAttackMoving(FVector PlayerPos);
	
	TArray<AActor*> GetActorsInFrontOfEnemy();

	// ===================================================== Stunning ===========================================

	void BeginLyingCountDown();

	void PlayLyingMontage();

	void RecoverFromLying();
	
	
public:

	EEnemyType GetType() const;

	// public delegate signature for finishing attacking task
	FOnFinishAttackingTask OnFinishAttackingTask;
	
	// Sets default values for this character's properties
	AEnemyBase();

	// Register encounters and targets with owning controller
	void RegisterOwningEncounter(class AEncounterVolume* NewOwningEncounter);
	void EngageTarget(AActor* Target);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float GetAttackCompletionTime() const;
	float GetAttackCounterableTime() const;


	void OnHideAttackIndicator();
	

	UFUNCTION(BlueprintCallable)
	void ExecuteAttack();
	void StartCounterAttackAnimation();
	
	// Instantly rotate to desired direction
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
	UBlackboardComponent* GetBBComponent() const {return BBComponent;}
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EEnemyAttackType GetEnemyStoredAttackType() const {return CurrentAttackType;}

	UFUNCTION(BlueprintCallable)
	UBehaviorTreeComponent* GetBehaviourTreeComponent() const {return BTComponent;}
	void SetBehaviourTreeComponent(UBehaviorTreeComponent* NewBehaviourTreeComponent) {BTComponent = NewBehaviourTreeComponent;}
	
	// ================================================== Interface Functions ============================================

	virtual void ActionEnd_Implementation(bool BufferingCheck) override;
	
	virtual void TryToDamagePlayer_Implementation() override;

	virtual void TryTriggerPlayerCounter_Implementation() override;

	virtual void ReceiveDamageFromPlayer_Implementation(int32 DamageAmount, AActor* DamageCauser, EPlayerAttackType PlayerAttackType) override;

	virtual void StartLyingOnTheGround_Implementation() override;

	virtual void RepeatLyingOnTheGround_Implementation() override;
	
	virtual void ShowEnemyAttackIndicator_Implementation() override;

	virtual void UnShowEnemyAttackIndicator_Implementation() override;

	virtual void ShowPlayerTargetIndicator_Implementation() override;
	
	virtual void UnShowPlayerTargetIndicator_Implementation() override;
	

private:
	
	// Timeline for enemy attack movement
	FTimeline AttackMovingTimeline;

	// Class variables for timeline function usage
	FVector AttackMovingDestination;
	FVector SelfAttackStartPos;
	
	class AEnemyBaseController* OwnController;
	void TryGetOwnController();
};
