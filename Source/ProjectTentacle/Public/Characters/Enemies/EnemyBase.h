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
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnInterruptStrafe);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUpdatingEnemyAttackType, EEnemyAttackType, NewAttackType);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnFinishAttackingTask, UBehaviorTreeComponent*, BehaviorTreeReference, bool, bIsSuccess, bool, DoesGetInterupted);

UCLASS()
class PROJECTTENTACLE_API AEnemyBase : public ACharacter, public ICharacterActionInterface, public IDamageInterface, public IEnemyWidgetInterface
{
	GENERATED_BODY()

protected:
	virtual void ReceiveDamageFromPlayer_Implementation(float DamageAmount, AActor* DamageCauser, EPlayerAttackType PlayerAttackType) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitializeWidgetComponents();
	
	void InitializeEnemyControllerRef();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement")
	float WalkSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement")
	float StrafeSpeed = 300.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= DebugSetting)
	bool EnableAttackMovement = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= DebugSetting)
	bool EnableEnemyAttackTracking = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= DebugSetting, meta=(ClampMin=0, ClampMax=1))
	float AttackTrackingLimitInAlpha = 0.5f; 
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat)
	EEnemyType UnitType = EEnemyType::Melee;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Combat, meta=(ClampMin=0, ClampMax=100))
	float DamageReductionPercentage = 0;
	
	UPROPERTY(EditDefaultsOnly, Category=Combat)
	float AttackCompletionTime = 2.0f;
	
	UPROPERTY(EditDefaultsOnly, Category=Combat)
	float AttackCounterableTime = 1.4f;
	
	UPROPERTY(EditDefaultsOnly, Category=Combat)
	float AttackTimeoutDuration = 10.f;
	
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

	AEnemyBaseController* OwnController;
	
	// Enemy Property variable
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnemyProperty)
	float Health = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnemyProperty)
	float MaxHealth = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnemyProperty)
	EEnemyCurrentState CurrentEnemyState = EEnemyCurrentState::WaitToAttack;

	bool IsDead = false;

	// bool to check if enemy is in attack task
	bool AttackTaskOn = false;
	
	// Receiving Damage Animations
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatSetting_Animation)
	UAnimMontage* EnemyReceiveLargeDamageAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatSetting_Animation)
	UAnimMontage* EnemyReceiveSmallDamageAnim;


	FTimerHandle StunningTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StunSetting)
	UAnimMontage* OnGettingStunnedAnimation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StunSetting)
	float TotalStunDuration = 4.0f;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ReceiveDamageAnimations)
	// UAnimMontage* ReceiveShortFlipKick;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ReceiveDamageAnimations)
	// UAnimMontage* ReceiveFlyingKick;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ReceiveDamageAnimations)
	// UAnimMontage* ReceiveFlyingPunch;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ReceiveDamageAnimations)
	// UAnimMontage* ReceiveSpinKick;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ReceiveDamageAnimations)
	// UAnimMontage* ReceiveDashingDoubleKick;

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

	
	
	// Attacking Animation
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Animations)
	UAnimMontage* CounterableAttackMontage;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Animations)
	UAnimMontage* NotCounterableAttackMontage;


	
	// ===================================================== Task ====================================================
	void TryFinishAttackTask(EEnemyCurrentState SwitchingState);
	

	// ===================================================== Attack ====================================================
	void SetAttackType();
	
	// ===================================================== Receive Damage =================================================
	void PlayReceiveDamageAnimation(EPlayerAttackType ReceivedAttackType);
	
	void HealthReduction(float DamageAmount);
	
	void RagDollPhysicsOnDead();

	UFUNCTION()
	void TimeoutAttack();

	// ===================================================== Stun =======================================================

	
	UFUNCTION()
	void RecoverFromStunState();
	
	
public:
	// ===================================================== On Death =======================================================
	virtual void OnDeath();
	
	void OnStunned();

	void StartAttackTimeout();
	void EnableStrafe(bool bStrafe = true) const;
	void ExecuteRangedAttack(AActor* Target);

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

	FOnInterruptStrafe OnInterruptStrafe;
	void OnHideAttackIndicator();
	

	UFUNCTION(BlueprintCallable)
	virtual void ExecuteAttack();
	
	
	// Instantly rotate to desired direction
	void InstantRotation(FVector RotatingVector);
	
	void PlayFinishedAnimation();


	void TryStopMoving();
	void TryResumeMoving();
	


	// ============================================= Utility Functions ====================================================
	void TrySwitchEnemyState(EEnemyCurrentState NewState);

	

	// ============================================= Get and Set functions ================================================
	bool GetIsDead() const { return IsDead;}
	
	int32 GetEnemyHealth() const { return Health;}
	void SetEnemyHealth(int32 NewHealth) {Health = NewHealth;}

	EEnemyType GetUnitType() const {return UnitType;}

	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UBlackboardComponent* GetBBComponent() const {return BBComponent;}
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EEnemyAttackType GetEnemyStoredAttackType() const {return CurrentAttackType;}

	EEnemyCurrentState GetCurrentEnemyState() const {return CurrentEnemyState;}
	
	UFUNCTION(BlueprintCallable)
	UBehaviorTreeComponent* GetBehaviourTreeComponent() const {return BTComponent;}
	void SetBehaviourTreeComponent(UBehaviorTreeComponent* NewBehaviourTreeComponent) {BTComponent = NewBehaviourTreeComponent;}

	
	// ================================================== Interface Functions ============================================
	
	virtual void OnResumeMovement_Implementation() override;

	virtual void OnResetEnemyCurrentState_Implementation() override;
	
	virtual void TryToDamagePlayer_Implementation() override;

	virtual void OnPullingEnemy_Implementation(FVector PullingDest, float PullingPower, float WorldDeltaSec) override;

	
	virtual void ShowEnemyAttackIndicator_Implementation() override;

	virtual void UnShowEnemyAttackIndicator_Implementation() override;

	virtual void ShowPlayerTargetIndicator_Implementation() override;
	
	virtual void UnShowPlayerTargetIndicator_Implementation() override;

	virtual void OnBeginStun_Implementation() override;

	virtual void OnResumeFromStunTimerCountDown_Implementation() override;
	
	void TryGetOwnController();
	
private:
	void TryClearFromPlayerTarget();

	void TurnCollisionOffOrOn(bool TurnCollisionOff);
	
	FTimerHandle AttackTimeoutHandle;
	
};