// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Player/PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "UI/UserWidget_HitIndicator.h"

FGenericTeamId APlayerCharacter::TeamId = FGenericTeamId(1);
// ==================================================== Constructor =========================================

void APlayerCharacter::ShowHitIndicator(const float CounterTime, const FVector HitLocation) const
{
	HUDRef->PopIndicator(CounterTime, HitLocation);
}

void APlayerCharacter::CollapseHitIndicator() const
{
	HUDRef->CollapseIndicator();
}

APlayerCharacter::APlayerCharacter()
{
	CreatCameraComponents();
	
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimuli Source"));
	StimuliSource->bAutoRegister = true;
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
}

FGenericTeamId APlayerCharacter::GetGenericTeamId() const
{
	return TeamId;
}

void APlayerCharacter::CreatCameraComponents()
{
	USkeletalMeshComponent* PlayerSkeletonMeshComp = GetMesh();
	if(!PlayerSkeletonMeshComp) return;

	UCapsuleComponent* PlayerCapsule = GetCapsuleComponent();
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(PlayerSkeletonMeshComp, "Spine");
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	ActionSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("ActionSpringArmComp"));
	ActionSpringArmComp->SetupAttachment(PlayerSkeletonMeshComp, "Spine2");
	ActionSpringArmComp->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	ActionSpringArmComp->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	
	// Create a combat camera
	CombatCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CombatCamera"));
	CombatCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	CombatCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	CombatCameraChild = CreateDefaultSubobject<UChildActorComponent>(TEXT("CombatCameraChild"));
	CombatCameraChild->SetupAttachment(CombatCamera, USpringArmComponent::SocketName);
	
	// Create a execution camera
	ExecutionCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ExecutionCamera"));
	ExecutionCamera->SetupAttachment(ActionSpringArmComp, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	ExecutionCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	ExecutionCameraChild = CreateDefaultSubobject<UChildActorComponent>(TEXT("ExecutionCameraChild"));
	ExecutionCameraChild->SetupAttachment(ExecutionCamera, USpringArmComponent::SocketName);
}


// =================================== Begin Play, Set up InputComponent, Tick ==============================
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharacterCurrentHealth = CharacterMaxHealth;

	FOnTimelineFloat CameraRotationUpdate;
	CameraRotationUpdate.BindDynamic(this, &APlayerCharacter::OnUpdatingCameraMovement);
	
	FOnTimelineEvent CameraRotationFinish;
	CameraRotationFinish.BindDynamic(this, &APlayerCharacter::OnFinishCameraMovement);
	
	CameraSwitchingTimeline.AddInterpFloat(CameraRotationCurve, CameraRotationUpdate);
	CameraSwitchingTimeline.SetTimelineFinishedFunc(CameraRotationFinish);

	
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CameraSwitchingTimeline.TickTimeline(DeltaSeconds);

	const APlayerController* PlayerControl = GetWorld()->GetFirstPlayerController();
	// reset input vector
	if(PlayerControl->WasInputKeyJustReleased(MovingForwardKey) || PlayerControl->WasInputKeyJustReleased(MovingBackKey))
	{
		InputDirection.SetPreviousInputDirectionY(InputDirection.GetInputDirectionY());
		InputDirection.SetInputDirectionY(0.0f);
	}
	
	if(PlayerControl->WasInputKeyJustReleased(MovingLeftKey) || PlayerControl->WasInputKeyJustReleased(MovingRightKey))
	{
		InputDirection.SetPreviousInputDirectionX(InputDirection.GetInputDirectionX());
		InputDirection.SetInputDirectionX(0.0f);
	}

	
}


// ==================================================== Movement ==============================================
void APlayerCharacter::LookUpAtRate(float Value)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::TurnAtRate(float Value)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// Set input direction Y value
		InputDirection.SetPreviousInputDirectionY(InputDirection.GetInputDirectionY());
		InputDirection.SetInputDirectionY(Value);
		
		if(CurrentActionState == EActionState::Idle) AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		InputDirection.SetPreviousInputDirectionX(InputDirection.GetInputDirectionX());
		InputDirection.SetInputDirectionX(Value);
		
		// add movement in that direction
		if(CurrentActionState == EActionState::Idle) AddMovementInput(Direction, Value);
	}
}


// // ====================================================== Attack ==============================================
void APlayerCharacter::TryMeleeAttack()
{
	// if player is recovering from action or is dodging, return
	if(CanPerformAttack())
		bool bExecuted = OnExecutePlayerAction.ExecuteIfBound(EActionState::Attack);
	
}

// ====================================================== Evade ===============================================
void APlayerCharacter::TryEvade()
{
	// if player is able to dodge, make dodge
	//if(CheckCanPerformAction())
	if(IsPlayerCounterable && CounteringVictim && (CurrentActionState != EActionState::SpecialAttack && CurrentActionState != EActionState::Dodge))
		bool bExecuted = OnExecutePlayerAction.ExecuteIfBound(EActionState::Evade);
}


void APlayerCharacter::TryDodge()
{
	// if player is able to dodge, make dodge
	if(CanPerformDodge() && CurrentStamina > CostForEachDodge)
	{
		StopAnimMontage();	
		StopRegenerateStamina();
		bool bExecuted = OnExecutePlayerAction.ExecuteIfBound(EActionState::Dodge);

		CurrentStamina -= CostForEachDodge;
		
		WaitToRegenStamina();
	}
}

bool APlayerCharacter::CanPerformAttack()
{
	return CurrentActionState == EActionState::Idle || CurrentActionState == EActionState::PreAction;
}

bool APlayerCharacter::CanPerformDodge()
{
	return CurrentActionState == EActionState::BeforeAttack || CurrentActionState == EActionState::Idle || CurrentActionState == EActionState::PreAction;
}

void APlayerCharacter::OnFinishCameraMovement()
{
	if(CurrentCameraType == EPlayerCameraType::InCombat)
	{
		// Enable Camera control
		AbleRotateVision = true;
	}
}

void APlayerCharacter::OnUpdatingCameraMovement(float Alpha)
{
	APlayerController* PlayerControl = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(!PlayerControl) return;

	const FRotator SettingRotation = UKismetMathLibrary::RLerp(CurrentCameraRotation, ExecutionCameraRotation, Alpha, true);

	PlayerControl->SetControlRotation(SettingRotation);
	
}

void APlayerCharacter::TrySpecialAbility1()
{
	// if player is recovering from action or is dodging, return
	if(CanPerformAttack())
		bool bExecuted = OnExecutePlayerAction.ExecuteIfBound(EActionState::SpecialAbility1);
}

void APlayerCharacter::TrySpecialAbility2()
{
	// if player is recovering from action or is dodging, return
	if(CanPerformAttack())
		bool bExecuted = OnExecutePlayerAction.ExecuteIfBound(EActionState::SpecialAbility2);
}

// void APlayerCharacter::TrySpecialAbility3()
// {
// 	// if player is recovering from action or is dodging, return
// 	if(CheckCanPerformAction())
// 		bool bExecuted = OnExecutePlayerAction.ExecuteIfBound(EActionState::SpecialAbility3);
// }

// ================================================ Utility ===========================================================
void APlayerCharacter::UnsetCurrentTarget()
{
	if(TargetActor != nullptr)
	{
		// if target actor is not null ptr, unshow its target icon, and clear the reference of target actor
		if(TargetActor->GetClass()->ImplementsInterface(UEnemyWidgetInterface::StaticClass()))
		{
			IEnemyWidgetInterface::Execute_UnShowPlayerTargetIndicator(TargetActor);
			TargetActor = nullptr;
		}
	}

	
}

void APlayerCharacter::DebugTestFunction()
{
	if(!DebugingBool)
	{
		OnSwitchingToExecutionCamera_Implementation();
		DebugingBool = true;
	}
	else
	{
		OnSwitchingBackToDefaultCamera_Implementation();
		DebugingBool = false;
	}
}

void APlayerCharacter::SetRangeAimingEnemy(AEnemyBase* NewRegisteringActor, float HUDRemainTime)
{
	if(RangeAimingEnemy != NewRegisteringActor)
		RangeAimingEnemy = NewRegisteringActor;

	IndicatorHUDRemainTime = HUDRemainTime;
}

void APlayerCharacter::SetTargetActor(AEnemyBase* NewTargetActor)
{
	UnsetCurrentTarget();
	
	if(NewTargetActor->GetClass()->ImplementsInterface(UEnemyWidgetInterface::StaticClass()))
		IEnemyWidgetInterface::Execute_ShowPlayerTargetIndicator(NewTargetActor);
	
	TargetActor = NewTargetActor;
}

// =============================================== Stamina Regen ================================================
void APlayerCharacter::WaitToRegenStamina()
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;
		
	World->GetTimerManager().SetTimer(RegenWaitingTimerHandle,this, &APlayerCharacter::BeginRegenerateStamina, MinTimeToStartRegen, false, -1);
}

void APlayerCharacter::BeginRegenerateStamina()
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;
	
	World->GetTimerManager().SetTimer(RegenStaminaTimerHandle,this, &APlayerCharacter::RegeneratingStamina, StaminaRegenTickTime, true, -1);

	// 	return;
	// }
	//
	// const bool bIsTimerPaused = World->GetTimerManager().IsTimerPaused(RegenStaminaTimerHandle);
	//
	// if(bIsTimerPaused)
	// {
	// 	World->GetTimerManager().UnPauseTimer(RegenStaminaTimerHandle);
	// }
	//
}

void APlayerCharacter::RegeneratingStamina()
{
	const float StaminaRegenPerCustomTick = StaminaRegenPerSecond * StaminaRegenTickTime;

	CurrentStamina = UKismetMathLibrary::FClamp(CurrentStamina + StaminaRegenPerCustomTick, 0 ,MaxStamina);
	
	if(CurrentStamina >= MaxStamina)
		StopRegenerateStamina();
}


void APlayerCharacter::StopRegenerateStamina()
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;
	
	World->GetTimerManager().ClearTimer(RegenWaitingTimerHandle);
	World->GetTimerManager().ClearTimer(RegenStaminaTimerHandle);
}

/*
// Determine the desired camera distance and height
float CameraDistance = 200.0f;
float CameraHeight = 50.0f;

// Calculate the desired camera offset
FVector CameraOffset = FVector(-CameraDistance, 0.0f, CameraHeight);

// Rotate the camera offset by the player's rotation
FVector RotatedOffset = PlayerPawn->GetActorRotation().RotateVector(CameraOffset);

// Get the Spring Arm component and set its target arm length and offset
USpringArmComponent* SpringArm = PlayerController->FindComponentByClass<USpringArmComponent>();
SpringArm->TargetArmLength = CameraDistance;
SpringArm->SocketOffset = RotatedOffset;

// Set the Spring Arm's rotation to the player's rotation
SpringArm->SetWorldRotation(PlayerPawn->GetActorRotation());
 * 
 */





// =============================================== Camera ===================================================


void APlayerCharacter::SetDifferentViewTarget(APlayerController* PlayerControl, AActor* NewCameraActor)
{
	PlayerControl->SetViewTargetWithBlend(NewCameraActor, CameraMoveTime, EViewTargetBlendFunction::VTBlend_EaseInOut, 1.0, false);
	
	// const FAttachmentTransformRules AttachRule = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);
	// CombatCamera->AttachToComponent(DestinationSpringArm, AttachRule);
	//
	// const FVector RelativeLoc = FVector(0,0,0);
	// const FRotator RelativeRot = FRotator(0,ChangedRelativeRotation,0);
	// const TEnumAsByte<EMoveComponentAction::Type> CameraMovement = EMoveComponentAction::Move;
	// FLatentActionInfo LatentActionInfo;
	// LatentActionInfo.CallbackTarget = this;
	// LatentActionInfo.ExecutionFunction = FName("OnFinishCameraMovement");
	// LatentActionInfo.Linkage = 0;
	// LatentActionInfo.UUID = 0;
	//
	// UKismetSystemLibrary::MoveComponentTo(CombatCamera, RelativeLoc, RelativeRot, CameraMoveEaseOut, CameraMoveEaseIn, CameraMoveTime, true, CameraMovement, LatentActionInfo);

}






// =============================================== Utility ================================================




// =========================================== Interface Function =========================================
void APlayerCharacter::DamagingTarget_Implementation()
{
	Super::DamagingTarget_Implementation();

	if(DamagingActor == nullptr) return;
	
	IDamageInterface::Execute_ReceiveDamageFromPlayer(DamagingActor, 1, this, CurrentAttackType);

	if(CurrentAttackType == EPlayerAttackType::CounterAttack) UnsetCurrentTarget();
}

void APlayerCharacter::EnterUnableCancelAttack_Implementation()
{
	Super::EnterUnableCancelAttack_Implementation();

	CurrentActionState = EActionState::Attack;

}

void APlayerCharacter::TryStoreCounterTarget_Implementation(AEnemyBase* CounterTarget)
{
	Super::TryStoreCounterTarget_Implementation(CounterTarget);

	SetCounteringTarget(CounterTarget);

	TryTurnCounterCapable(true);
}

void APlayerCharacter::TryRemoveCounterTarget_Implementation(AEnemyBase* CounterTarget)
{
	Super::TryRemoveCounterTarget_Implementation(CounterTarget);

	ClearCounteringTarget(CounterTarget);

	TryTurnCounterCapable(false);
}


// void APlayerCharacter::ReceiveAttackInCounterState_Implementation(AActor* CounteringTarget)
// {
// 	Super::ReceiveAttackInCounterState_Implementation(CounteringTarget);
//
// 	if(CurrentActionState == EActionState::Evade)
// 		bool bExecuted = OnEnteringPreCounterState.ExecuteIfBound(CounteringTarget);
// 		
// 	// // if player is in evade state, it means player will trigger counter action
// 	// if(CurrentActionState == EActionState::Evade)
// 	// 	bool bExecuted = OnTriggeringCounter.ExecuteIfBound(CounteringTarget);
// }


void APlayerCharacter::ReceiveDamageFromEnemy_Implementation(int32 DamageAmount, AActor* DamageCauser,
                                                             EEnemyAttackType EnemyAttackType)
{
	IDamageInterface::ReceiveDamageFromEnemy_Implementation(DamageAmount, DamageCauser, EnemyAttackType);

	bool bExecuted = OnReceivingIncomingDamage.ExecuteIfBound(DamageAmount, DamageCauser, EnemyAttackType);
}

void APlayerCharacter::OnSwitchingToExecutionCamera_Implementation()
{
	IPlayerCameraInterface::OnSwitchingToExecutionCamera_Implementation();

	// try to get valid references
	APlayerController* PlayerControl = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(!PlayerControl) return;

	USkeletalMeshComponent* PlayerMesh = GetMesh();
	if(!PlayerMesh) return;
	
	// set enum to execution
	CurrentCameraType = EPlayerCameraType::Execution;

	
	CurrentCameraRotation = PlayerControl->PlayerCameraManager->GetCameraRotation(); 
	ExecutionCameraRotation = UKismetMathLibrary::MakeRotFromX(PlayerMesh->GetRightVector());

	
	PlayerControl->SetViewTargetWithBlend(ExecutionCameraChild->GetChildActor(), CameraMoveTime, EViewTargetBlendFunction::VTBlend_EaseInOut, 1.0, false);
	//CameraSwitchingTimeline.PlayFromStart();
	AbleRotateVision = false;
}

void APlayerCharacter::OnSwitchingBackToDefaultCamera_Implementation()
{
	IPlayerCameraInterface::OnSwitchingBackToDefaultCamera_Implementation();

	// try to get valid references
	APlayerController* PlayerControl = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(!PlayerControl) return;
	
	CurrentCameraType = EPlayerCameraType::InCombat;
	PlayerControl->SetViewTargetWithBlend(CombatCameraChild->GetChildActor(), CameraMoveTime, EViewTargetBlendFunction::VTBlend_EaseInOut, 1.0, false);
	//CameraSwitchingTimeline.ReverseFromEnd();
}

void APlayerCharacter::ActionEnd_Implementation(bool BufferingCheck)
{
	Super::ActionEnd_Implementation(BufferingCheck);
}

void APlayerCharacter::OnActivateComboResetTimer_Implementation()
{
	Super::OnActivateComboResetTimer_Implementation();

	const bool bExecuted = OnEnableComboResetTimer.ExecuteIfBound();
}

void APlayerCharacter::DetachEnemyTarget_Implementation()
{
	Super::DetachEnemyTarget_Implementation();

	// Unset Target
	UnsetCurrentTarget();

}

void APlayerCharacter::OnShowPlayerIndicatorHUD_Implementation(bool Show)
{
	Super::OnShowPlayerIndicatorHUD_Implementation(Show);

	if(Show)
	{
		if(!RangeAimingEnemy) return;
		ShowHitIndicator(IndicatorHUDRemainTime, RangeAimingEnemy->GetActorLocation());
	}
	else
		CollapseHitIndicator();
	
	
}


void APlayerCharacter::OnChangePlayerIndicatorHUD_Visibility_Implementation(bool IsVisible)
{
	Super::OnChangePlayerIndicatorHUD_Visibility_Implementation(IsVisible);

	HUDRef->ChangeVisibility(IsVisible);
}





