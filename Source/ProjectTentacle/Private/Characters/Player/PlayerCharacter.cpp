// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Player/PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


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
}

// =================================== Begin Play, Set up InputComponent, Tick ==============================
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAction("Evade", IE_Pressed, this, &APlayerCharacter::TryDodge);
	PlayerInputComponent->BindAction("MeleeAttack", IE_Pressed, this, &APlayerCharacter::TryMeleeAttack);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharacterCurrentHealth = CharacterMaxHealth;

	InitializeTimeLineComp();
}

void APlayerCharacter::InitializeTimeLineComp()
{
	FOnTimelineFloat MovingAttackPosUpdate;
	MovingAttackPosUpdate.BindDynamic(this, &APlayerCharacter::MovingAttackMovement);
	ShortFlipKickTimeLine.AddInterpFloat(ShortFlipKickCurve, MovingAttackPosUpdate);
	FlyingKickTimeLine.AddInterpFloat(FlyingKickCurve, MovingAttackPosUpdate);
	FlyingPunchTimeLine.AddInterpFloat(FlyingPunchCurve, MovingAttackPosUpdate);
	SpinKickTimeLine.AddInterpFloat(SpinKickCurve, MovingAttackPosUpdate);
	DashingDoubleKickTimeLine.AddInterpFloat(DashingDoubleKickCurve, MovingAttackPosUpdate);
	CloseToPerformFinisherTimeLine.AddInterpFloat(DashingDoubleKickCurve, MovingAttackPosUpdate);
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const APlayerController* PlayerControl = GetWorld()->GetFirstPlayerController();
	
	// // reset input vector
	if(PlayerControl->WasInputKeyJustReleased(MovingForwardKey) || PlayerControl->WasInputKeyJustReleased(MovingBackKey))
		InputDirection.SetInputDirectionY(0.0f);
	
	if(PlayerControl->WasInputKeyJustReleased(MovingLeftKey) || PlayerControl->WasInputKeyJustReleased(MovingRightKey))
		InputDirection.SetInputDirectionX(0.0f);

	
	ShortFlipKickTimeLine.TickTimeline(DeltaSeconds);
	FlyingKickTimeLine.TickTimeline(DeltaSeconds);
	FlyingPunchTimeLine.TickTimeline(DeltaSeconds);
	SpinKickTimeLine.TickTimeline(DeltaSeconds);
	DashingDoubleKickTimeLine.TickTimeline(DeltaSeconds);
	CloseToPerformFinisherTimeLine.TickTimeline(DeltaSeconds);
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if(CurrentActionState == EActionState::Evade)
	{
		StopAnimMontage();
		BeginCounterAttack(DamageCauser);
	}

	if(CurrentActionState == EActionState::Idle || CurrentActionState == EActionState::Attack)
	{
		if(CurrentActionState == EActionState::Attack)
		{
			StopAnimMontage();	
		}

		// TODO: Damage player

		// Player Play receiving damage montage
		if(ReceiveDamageMontage == nullptr) return DamageAmount;

		CurrentActionState = EActionState::Recovering;
	
		CurrentPlayingMontage = ReceiveDamageMontage;
		
		PlayAnimMontage(CurrentPlayingMontage, 1, NAME_None);
	}
	
	return DamageAmount;
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

		InputDirection.SetInputDirectionX(Value);
		
		// add movement in that direction
		if(CurrentActionState == EActionState::Idle) AddMovementInput(Direction, Value);
	}
}


// ====================================================== Attack ==============================================
void APlayerCharacter::TryMeleeAttack()
{
	// if player is recovering from action or is dodging, return
	if(CurrentActionState == EActionState::Idle || CurrentActionState == EActionState::WaitForCombo)
		BeginMeleeAttack();
}

void APlayerCharacter::BeginMeleeAttack()
{
	// Get max number of attack animation montage array
	const int32 MAttackMontagesNum = MeleeAttackMontages.Num();

	// Get random number from 0 to max number of attack animation montage array
	const int32 MAttackRndIndex = UKismetMathLibrary::RandomIntegerInRange(0, MAttackMontagesNum - 1);
	
	// Validation Check
	if(MeleeAttackMontages[MAttackRndIndex] == nullptr) return;

	// 
	const EPlayerAttackType SelectedAttackType = GetAttackTypeByRndNum(MAttackRndIndex);

	// Get All enemy around player
	TArray<AAttackTargetTester*> OpponentAroundSelf = GetAllOpponentAroundSelf();

	// if there is no opponent around, simply return
	if(OpponentAroundSelf.Num() == 0) return;

	// Get target direction to face to
	AAttackTargetTester* ResultFacingEnemy = GetTargetEnemy(OpponentAroundSelf);

	// if there is no direction, return
	if(ResultFacingEnemy == nullptr) return;
	

	// Store target actor and selected attack type references for later anim notify usage
	TargetActor = ResultFacingEnemy;
	const FVector TargetActorPos = TargetActor->GetActorLocation();
	const FVector PlayerPos = GetActorLocation();

	CurrentAttackType = SelectedAttackType;

	
	
	// simply make player and targeted enemy rotate to each other
	const FVector FacingEnemyDir = UKismetMathLibrary::Normal(TargetActorPos - PlayerPos);
	const FVector FacingPlayerDir = UKismetMathLibrary::Normal(PlayerPos - TargetActorPos);
	InstantRotation(FacingEnemyDir);
	TargetActor->InstantRotation(FacingPlayerDir);

	// set lerping start and end position to variable
	SetAttackMovementPositions(TargetActorPos);
	
	// change current action state enum
	CurrentActionState = EActionState::Attack;

	// TODO: Need To ReadWrite 
	// Check if Enemy is dying or now, if is, finish him
	int32 EnemyCurrentHealth = TargetActor->GetEnemyHealth();

	if(EnemyCurrentHealth <= 1)
	{
		FinishEnemy();
		return;
	}
	
	EnemyCurrentHealth--;
	TargetActor->SetEnemyHealth(EnemyCurrentHealth);

	
	// Player attack montage
	CurrentPlayingMontage = MeleeAttackMontages[MAttackRndIndex];
	PlayAnimMontage(CurrentPlayingMontage, 1, "Default");

	// Start attack movement timeline depends on the result of playering montage
	StartAttackMovementTimeline(SelectedAttackType);
}

void APlayerCharacter::FinishEnemy()
{
	const FVector TargetPos = TargetActor->GetActorLocation();
	const FVector SelfPos = GetActorLocation();
	
	const FVector TargetToSelfDir = UKismetMathLibrary::Normal(SelfPos - TargetPos);

	// Player attack montage
	CurrentPlayingMontage = FinisherAnimMontages;
	PlayAnimMontage(CurrentPlayingMontage, 1, "Default");

	TargetActor->PlayFinishedAnimation();
}

void APlayerCharacter::SetAttackMovementPositions(FVector TargetPos)
{
	MovingAttackStart = GetActorLocation();

	// Get direction from target to player
	FVector OffsetWithoutZ = MovingAttackStart - TargetPos;
	OffsetWithoutZ.Z = 0;
	const FVector DirFromTargetToPlayer = UKismetMathLibrary::Normal(OffsetWithoutZ);

	// Get lerp end position
	MovingAttackEnd = TargetPos + (DirFromTargetToPlayer * 80);
}

EPlayerAttackType APlayerCharacter::GetAttackTypeByRndNum(int32 RndNum)
{
	return static_cast<EPlayerAttackType>(RndNum);
}

void APlayerCharacter::StartAttackMovementTimeline(EPlayerAttackType AttackType)
{
	switch (AttackType)
	{
		case EPlayerAttackType::ShortFlipKick:
			ShortFlipKickTimeLine.PlayFromStart();
			break;
		case EPlayerAttackType::FlyingKick:
			FlyingKickTimeLine.PlayFromStart();
			break;
		case EPlayerAttackType::FlyingPunch:
			FlyingPunchTimeLine.PlayFromStart();
			break;
		case EPlayerAttackType::SpinKick:
			SpinKickTimeLine.PlayFromStart();
			break;
		case EPlayerAttackType::DashingDoubleKick:
			DashingDoubleKickTimeLine.PlayFromStart();
			break;
		default:
			break;
	}
}

// ====================================================== Dodge ===============================================
void APlayerCharacter::TryDodge()
{
	// if player is able to dodge, make dodge
	if(CurrentActionState == EActionState::Idle) BeginDodge();
}

void APlayerCharacter::BeginDodge()
{
	// if Dodge montage is null pointer, just return
	if(DodgeAnimMontage == nullptr) return;

	CurrentActionState = EActionState::Evade;

	CurrentPlayingMontage = DodgeAnimMontage;

	const int32 RndPerformIndex = UKismetMathLibrary::RandomIntegerInRange(0,1);

	if(RndPerformIndex == 0)
	{
		PlayAnimMontage(CurrentPlayingMontage, 1, "DodgeRight");
		return;
	}
	
	PlayAnimMontage(CurrentPlayingMontage, 1, "DodgeLeft");
}



// ================================================== Counter ======================================================
void APlayerCharacter::BeginCounterAttack(AActor* CounteringTarget)
{
	// if Dodge montage is null pointer, just return
	if(CounterAttackMontages == nullptr) return;

	CurrentActionState = EActionState::SpecialAttack;

	AAttackTargetTester* CastedTarget = Cast<AAttackTargetTester>(CounteringTarget);
	if(CastedTarget == nullptr) return;

	TargetActor = CastedTarget;
	
	CurrentPlayingMontage = CounterAttackMontages;

	const FVector FacingEnemyDir = UKismetMathLibrary::Normal(TargetActor->GetActorLocation() - GetActorLocation());
	InstantRotation(FacingEnemyDir);
	
	PlayAnimMontage(CurrentPlayingMontage, 1, NAME_None);
}

// ==================================================== Utility ===============================================
TArray<AAttackTargetTester*> APlayerCharacter::GetAllOpponentAroundSelf()
{
	TArray<AActor*> FoundActorList;
	TArray<AAttackTargetTester*> ReturnActors;
	
	const UWorld* World = GetWorld();
	if(World == nullptr) return ReturnActors;

	const FVector SelfPos = GetActorLocation();

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	
	UKismetSystemLibrary::SphereOverlapActors(World,SelfPos, DetectionRange, FilterType, FilteringClass, IgnoreActors,FoundActorList);

	if(FoundActorList.Num() != 0)
	{
		for (AActor* EachFoundActor : FoundActorList)
		{
			AAttackTargetTester* FoundCharacter = Cast<AAttackTargetTester>(EachFoundActor);
			if(FoundCharacter != nullptr) ReturnActors.Add(FoundCharacter);
		}
	}
	
	return ReturnActors;
}

void APlayerCharacter::InstantRotation(FVector RotatingVector)
{
	const FRotator InputRotation = UKismetMathLibrary::MakeRotFromX(RotatingVector);

	SetActorRotation(InputRotation);
}

AAttackTargetTester* APlayerCharacter::GetTargetEnemy(TArray<AAttackTargetTester*> OpponentsAroundSelf)
{
	const FVector SelfPos = GetActorLocation();

	// find out which way is right
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector RightX = UKismetMathLibrary::GetRightVector(YawRotation);
	const FVector ForwardY = UKismetMathLibrary::GetForwardVector(YawRotation);

	const FVector RightInputDir = RightX * InputDirection.GetInputDirectionX();
	const FVector ForwardInputDir = ForwardY * InputDirection.GetInputDirectionY();
	
	const FVector InputDest = SelfPos + ((RightInputDir * 50) + (ForwardInputDir * 50));
	
	const FVector SelfToInputDestDir = UKismetMathLibrary::GetDirectionUnitVector(SelfPos, InputDest);
	
	// set first one as closest target and iterating from opponents list
	AAttackTargetTester* ReturnTarget = OpponentsAroundSelf[0];
	
	// Set a fake dot product
	float TargetDotProduct = -1.0f;

	// TODO: Use dot product to check instead of angle
	for (int32 i = 0; i < OpponentsAroundSelf.Num(); i++)
	{
		FVector EachCharacterPos = OpponentsAroundSelf[i]->GetActorLocation();

		EachCharacterPos.Z = SelfPos.Z;

		const FVector SelfToCharacterDir = UKismetMathLibrary::GetDirectionUnitVector(SelfPos, EachCharacterPos);


		const float DotProduct = UKismetMathLibrary::Dot_VectorVector(SelfToInputDestDir, SelfToCharacterDir);
		
		// if iterating dot product is not correct
		if(DotProduct < 0.70f) continue;

		// if iterating dotproduct is bigger than current target dot product, it means iterating actor will more likely be our target
		if(DotProduct > TargetDotProduct)
		{
			ReturnTarget = OpponentsAroundSelf[i];
			TargetDotProduct = DotProduct;
		}
	}

	if(TargetDotProduct < 0.70f)
	{
		return nullptr;
	}

	
	return ReturnTarget;
}

// ========================================== Timeline Function =========================================
void APlayerCharacter::MovingAttackMovement(float Alpha)
{
	const FVector CharacterCurrentPos = GetActorLocation();
	
	const FVector MovingPos = UKismetMathLibrary::VLerp(MovingAttackStart, MovingAttackEnd, Alpha);

	const FVector LaunchingPos = FVector(MovingPos.X, MovingPos.Y, CharacterCurrentPos.Z);

	SetActorLocation(LaunchingPos);
}


// =========================================== Interface Function =========================================
void APlayerCharacter::DamagingTarget_Implementation()
{
	Super::DamagingTarget_Implementation();

	if(TargetActor == nullptr) return;

	switch (CurrentAttackType)
	{
		case EPlayerAttackType::ShortFlipKick:
			TargetActor->PlayDamageReceiveAnimation(0);
			break;
		case EPlayerAttackType::FlyingKick:
			TargetActor->PlayDamageReceiveAnimation(1);
			break;
		case EPlayerAttackType::FlyingPunch:
			TargetActor->PlayDamageReceiveAnimation(2);
			break;
		case EPlayerAttackType::SpinKick:
			TargetActor->PlayDamageReceiveAnimation(3);
			break;
		case EPlayerAttackType::DashingDoubleKick:
			TargetActor->PlayDamageReceiveAnimation(4);
			break;
		default: ;
	}
	
	
}





