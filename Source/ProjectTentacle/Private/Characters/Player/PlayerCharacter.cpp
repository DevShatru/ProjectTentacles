// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Player/PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

FGenericTeamId APlayerCharacter::TeamId = FGenericTeamId(1);
// ==================================================== Constructor =========================================

APlayerCharacter::APlayerCharacter()
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimuli Source"));
	StimuliSource->bAutoRegister = true;
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
}

FGenericTeamId APlayerCharacter::GetGenericTeamId() const
{
	return TeamId;
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
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

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
void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
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
	if(CurrentActionState == EActionState::Idle || CurrentActionState == EActionState::WaitForCombo)
		bool bExecuted = OnExecutePlayerAction.ExecuteIfBound(EActionState::Attack);
	
}

// ====================================================== Evade ===============================================
void APlayerCharacter::TryEvade()
{
	// if player is able to dodge, make dodge
	if(CurrentActionState == EActionState::Idle)
		bool bExecuted = OnExecutePlayerAction.ExecuteIfBound(EActionState::Evade);
}


void APlayerCharacter::TryDodge()
{
	// if player is able to dodge, make dodge
	if(CurrentActionState == EActionState::Idle && CurrentStamina > CostForEachDodge)
	{
		StopAnimMontage();	
		StopRegenerateStamina();
		bool bExecuted = OnExecutePlayerAction.ExecuteIfBound(EActionState::Dodge);

		CurrentStamina -= CostForEachDodge;
		
		WaitToRegenStamina();
	}
}

void APlayerCharacter::SetTargetActor(AEnemyBase* NewTargetActor)
{
	// if target actor is not null ptr, unshow its target icon
	if(TargetActor != nullptr)
	{
		if(TargetActor->GetClass()->ImplementsInterface(UEnemyWidgetInterface::StaticClass()))
			IEnemyWidgetInterface::Execute_UnShowPlayerTargetIndicator(TargetActor);
	}
	
	
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



// =============================================== Utility ================================================




// =========================================== Interface Function =========================================
void APlayerCharacter::DamagingTarget_Implementation()
{
	Super::DamagingTarget_Implementation();

	if(DamagingActor == nullptr) return;

	IDamageInterface::Execute_ReceiveDamageFromPlayer(DamagingActor, 1, this, CurrentAttackType);
}

void APlayerCharacter::ReceiveDamageFromEnemy_Implementation(int32 DamageAmount, AActor* DamageCauser,
	EEnemyAttackType EnemyAttackType)
{
	IDamageInterface::ReceiveDamageFromEnemy_Implementation(DamageAmount, DamageCauser, EnemyAttackType);

	bool bExecuted = OnReceivingIncomingDamage.ExecuteIfBound(DamageAmount, DamageCauser, EnemyAttackType);
}

void APlayerCharacter::ActionEnd_Implementation(bool BufferingCheck)
{
	Super::ActionEnd_Implementation(BufferingCheck);

	const bool bExecuted = OnClearingComboCount.ExecuteIfBound();
}





