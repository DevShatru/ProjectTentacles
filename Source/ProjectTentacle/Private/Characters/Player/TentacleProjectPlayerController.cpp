// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Player/TentacleProjectPlayerController.h"

ATentacleProjectPlayerController::ATentacleProjectPlayerController()
{
	
}

void ATentacleProjectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PossessedPlayerRef = Cast<APlayerCharacter>(GetPawn());
}

void ATentacleProjectPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();


	InputComponent->BindAxis("MoveForward", this, &ATentacleProjectPlayerController::OnMoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATentacleProjectPlayerController::OnMoveRight);
	InputComponent->BindAction("Evade", IE_Pressed, this, &ATentacleProjectPlayerController::OnEvade);
	InputComponent->BindAction("MeleeAttack", IE_Pressed, this, &ATentacleProjectPlayerController::OnMeleeAttack);
	InputComponent->BindAction("Dodge", IE_Pressed, this, &ATentacleProjectPlayerController::OnDodge);
	//
	// // We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// // "turn" handles devices that provide an absolute delta, such as a mouse.
	// // "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &ATentacleProjectPlayerController::OnAddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ATentacleProjectPlayerController::OnTurnAtRate);
	InputComponent->BindAxis("LookUp", this, &ATentacleProjectPlayerController::OnAddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ATentacleProjectPlayerController::OnLookUpAtRate);
}

void ATentacleProjectPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


// =================================================== Input Functions =================================================
void ATentacleProjectPlayerController::OnMoveForward(float Value)
{
	if (PossessedPlayerRef)
	{
		PossessedPlayerRef->MoveForward(Value);
	}
}

void ATentacleProjectPlayerController::OnMoveRight(float Value)
{
	if (PossessedPlayerRef)
	{
		PossessedPlayerRef->MoveRight(Value);
	}
}

void ATentacleProjectPlayerController::OnEvade()
{
	if (PossessedPlayerRef)
	{
		PossessedPlayerRef->TryEvade();
	}
}

void ATentacleProjectPlayerController::OnMeleeAttack()
{
	if (PossessedPlayerRef)
	{
		PossessedPlayerRef->TryMeleeAttack();
	}
}

void ATentacleProjectPlayerController::OnDodge()
{
	if (PossessedPlayerRef)
	{
		PossessedPlayerRef->TryDodge();
	}
}

void ATentacleProjectPlayerController::OnAddControllerYawInput(float Value)
{
	if (PossessedPlayerRef)
	{
		PossessedPlayerRef->AddControllerYawInput(Value);
	}
}

void ATentacleProjectPlayerController::OnTurnAtRate(float Rate)
{
	if (PossessedPlayerRef)
	{
		PossessedPlayerRef->TurnAtRate(Rate);
	}
}

void ATentacleProjectPlayerController::OnAddControllerPitchInput(float Value)
{
	if (PossessedPlayerRef)
	{
		PossessedPlayerRef->AddControllerPitchInput(Value);
	}
}

void ATentacleProjectPlayerController::OnLookUpAtRate(float Rate)
{
	if (PossessedPlayerRef)
	{
		PossessedPlayerRef->LookUpAtRate(Rate);
	}
}

