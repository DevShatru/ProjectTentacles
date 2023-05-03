// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Base/BaseCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
}

void ABaseCharacter::Heal(float Amount)
{
	const int8 AttemptedHeal = CharacterCurrentHealth + Amount;
	CharacterCurrentHealth = (AttemptedHeal > CharacterMaxHealth) ? CharacterMaxHealth : AttemptedHeal;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharacterCurrentHealth = CharacterMaxHealth;
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseCharacter::ActionEnd_Implementation(bool BufferingCheck)
{
	ICharacterActionInterface::ActionEnd_Implementation(BufferingCheck);

	CurrentActionState = EActionState::Idle;
	
}

void ABaseCharacter::StartWaitForCombo_Implementation()
{
	ICharacterActionInterface::StartWaitForCombo_Implementation();

	CurrentActionState = EActionState::PreAction;
}

