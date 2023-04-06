// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Player/PlayerActionComponent.h"

#include "Characters/Enemies/EnemyBase.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UPlayerActionComponent::UPlayerActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}



// =================================================== Begin Functions =======================================================
void UPlayerActionComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeOwnerRef();
	// ...
	InitializeTimelineComp();
}

void UPlayerActionComponent::InitializeOwnerRef()
{
	AActor* OwnerActor = GetOwner();
	if(OwnerActor == nullptr) return;
	
	APlayerCharacter* CastedOwnerActor = Cast<APlayerCharacter>(OwnerActor);
	if(CastedOwnerActor == nullptr) return;

	PlayerOwnerRef = CastedOwnerActor;

	PlayerOwnerRef->OnExecutePlayerAction.BindDynamic(this, &UPlayerActionComponent::ExecutePlayerAction);
	PlayerOwnerRef->OnReceivingIncomingDamage.BindDynamic(this, &UPlayerActionComponent::ReceivingDamage);
	PlayerOwnerRef->OnClearingComboCount.BindDynamic(this, &UPlayerActionComponent::ResetComboCount);
}

void UPlayerActionComponent::InitializeTimelineComp()
{
	FOnTimelineFloat MovingAttackPosUpdate;
	MovingAttackPosUpdate.BindDynamic(this, &UPlayerActionComponent::MovingAttackMovement);
	ShortFlipKickTimeLine.AddInterpFloat(ShortFlipKickCurve, MovingAttackPosUpdate);
	FlyingKickTimeLine.AddInterpFloat(FlyingKickCurve, MovingAttackPosUpdate);
	FlyingPunchTimeLine.AddInterpFloat(FlyingPunchCurve, MovingAttackPosUpdate);
	SpinKickTimeLine.AddInterpFloat(SpinKickCurve, MovingAttackPosUpdate);
	DashingDoubleKickTimeLine.AddInterpFloat(DashingDoubleKickCurve, MovingAttackPosUpdate);
	CloseToPerformFinisherTimeLine.AddInterpFloat(DashingDoubleKickCurve, MovingAttackPosUpdate);
	DodgeLerpingTimeLine.AddInterpFloat(DodgeLerpingCurve, MovingAttackPosUpdate);
	
}

// ======================================================== Tick ======================================================

void UPlayerActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const float PlayerStoredInputX = PlayerOwnerRef->GetPlayerInputDir().GetInputDirectionX();
	const float PlayerStoredInputY = PlayerOwnerRef->GetPlayerInputDir().GetInputDirectionY();
	if(PlayerStoredInputX != 0.0f || PlayerStoredInputY != 0.0f)
		TryToUpdateTarget();

	// delta time will change due to player's combo time
	const float DeltaWithComboBonus = DeltaTime * (1 + (CurrentComboCount * ComboSpeedMotiplier));
	ShortFlipKickTimeLine.TickTimeline(DeltaWithComboBonus);
	FlyingKickTimeLine.TickTimeline(DeltaWithComboBonus);
	FlyingPunchTimeLine.TickTimeline(DeltaWithComboBonus);
	SpinKickTimeLine.TickTimeline(DeltaWithComboBonus);
	DashingDoubleKickTimeLine.TickTimeline(DeltaWithComboBonus);
	CloseToPerformFinisherTimeLine.TickTimeline(DeltaWithComboBonus);
	DodgeLerpingTimeLine.TickTimeline(DeltaTime);

}

// ====================================================== Attack ==============================================
void UPlayerActionComponent::BeginMeleeAttack()
{
	// Reference validation check
	if(PlayerOwnerRef == nullptr) return;

	// if player is not having target, return;
	AEnemyBase* RegisteredTarget = PlayerOwnerRef->GetTargetActor();
	if(PlayerOwnerRef->GetTargetActor() == nullptr) return;
	
	// Get max number of attack animation montage array
	const int32 MAttackMontagesNum = MeleeAttackMontages.Num();

	// Get random number from 0 to max number of attack animation montage array
	const int32 MAttackRndIndex = UKismetMathLibrary::RandomIntegerInRange(0, MAttackMontagesNum - 1);
	
	// Validation Check
	if(MeleeAttackMontages[MAttackRndIndex] == nullptr) return;

	// 
	const EPlayerAttackType SelectedAttackType = GetAttackTypeByRndNum(MAttackRndIndex);


	
	const FVector TargetActorPos = RegisteredTarget->GetActorLocation();
	const FVector PlayerPos = PlayerOwnerRef->GetActorLocation();

	PlayerOwnerRef->SetCurrentAttackType(SelectedAttackType);

	
	
	// simply make player and targeted enemy rotate to each other
	const FVector FacingEnemyDir = UKismetMathLibrary::Normal(TargetActorPos - PlayerPos);
	const FVector FacingPlayerDir = UKismetMathLibrary::Normal(PlayerPos - TargetActorPos);
	InstantRotation(FacingEnemyDir);
	RegisteredTarget->InstantRotation(FacingPlayerDir);

	// set lerping start and end position to variable
	SetAttackMovementPositions(TargetActorPos);

	// if current action state is waiting for combo, it means player land a successful combo, combo count increment
	if(PlayerOwnerRef->GetCurrentActionState() == EActionState::WaitForCombo)
		ComboCountIncrement();
	
	// change current action state enum
	PlayerOwnerRef->SetCurrentActionState(EActionState::Attack);

	// TODO: Need To ReadWrite 
	// Check if Enemy is dying or now, if is, finish him
	int32 EnemyCurrentHealth = RegisteredTarget->GetEnemyHealth();

	if(EnemyCurrentHealth <= 1)
	{
		FinishEnemy();
		return;
	}
	
	// Set damaging actor
	PlayerOwnerRef->SetDamagingActor(RegisteredTarget);
	
	// Player attack montage
	CurrentPlayingMontage = MeleeAttackMontages[MAttackRndIndex];

	const float CurrentComboSpeed = CalculateCurrentComboSpeed();


	
	PlayerOwnerRef->PlayAnimMontage(CurrentPlayingMontage, CurrentComboSpeed, "Default");

	// Start attack movement timeline depends on the result of playering montage
	StartAttackMovementTimeline(SelectedAttackType);
}

void UPlayerActionComponent::ComboCountIncrement()
{
	CurrentComboCount = UKismetMathLibrary::Clamp((CurrentComboCount+1), 0, MaxComboCount);
}

void UPlayerActionComponent::FinishEnemy()
{
	AEnemyBase* CurrentTarget = PlayerOwnerRef->GetTargetActor();
	if(CurrentTarget == nullptr) return;
	
	// Player attack montage
	CurrentPlayingMontage = FinisherAnimMontages;

	const float CurrentComboSpeed = CalculateCurrentComboSpeed();
	PlayerOwnerRef->PlayAnimMontage(CurrentPlayingMontage, CurrentComboSpeed, "Default");

	CurrentTarget->PlayFinishedAnimation();
	CloseToPerformFinisherTimeLine.PlayFromStart();
}

void UPlayerActionComponent::SetAttackMovementPositions(FVector TargetPos)
{
	MovingStartPos = PlayerOwnerRef->GetActorLocation();

	// Get direction from target to player
	FVector OffsetWithoutZ = MovingStartPos - TargetPos;
	OffsetWithoutZ.Z = 0;
	const FVector DirFromTargetToPlayer = UKismetMathLibrary::Normal(OffsetWithoutZ);

	// Get lerp end position
	MovingEndPos = TargetPos + (DirFromTargetToPlayer * 80);
}

EPlayerAttackType UPlayerActionComponent::GetAttackTypeByRndNum(int32 RndNum)
{
	return static_cast<EPlayerAttackType>(RndNum);
}

void UPlayerActionComponent::StartAttackMovementTimeline(EPlayerAttackType AttackType)
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

float UPlayerActionComponent::CalculateCurrentComboSpeed()
{
	
	float ComboSpeedBonus = static_cast<float>(CurrentComboCount) * ComboSpeedMotiplier;
	
	return 1 + ComboSpeedBonus;
}

// ====================================================== Evade ===============================================

void UPlayerActionComponent::BeginEvade()
{
	// if evade montage is null pointer, just return
	if(EvadeAnimMontage == nullptr) return;

	PlayerOwnerRef->SetCurrentActionState(EActionState::Evade);
	
	CurrentPlayingMontage = EvadeAnimMontage;

	const int32 RndPerformIndex = UKismetMathLibrary::RandomIntegerInRange(0,1);

	if(RndPerformIndex == 0)
	{
		PlayerOwnerRef->PlayAnimMontage(CurrentPlayingMontage, 1, "DodgeRight");
		return;
	}
	
	PlayerOwnerRef->PlayAnimMontage(CurrentPlayingMontage, 1, "DodgeLeft");
}



// ================================================== Counter ======================================================
void UPlayerActionComponent::BeginCounterAttack(AActor* CounteringTarget)
{
	// ref validation check
	if(PlayerOwnerRef == nullptr) return;
	
	// if Dodge montage is null pointer, just return
	if(CounterAttackMontages == nullptr) return;

	PlayerOwnerRef->SetCurrentActionState(EActionState::SpecialAttack);

	AEnemyBase* CastedTarget = Cast<AEnemyBase>(CounteringTarget);
	if(CastedTarget == nullptr) return;

	
	PlayerOwnerRef->SetTargetActor(CastedTarget);
	
	CurrentPlayingMontage = CounterAttackMontages;

	const FVector FacingEnemyDir = UKismetMathLibrary::Normal(CastedTarget->GetActorLocation() - PlayerOwnerRef->GetActorLocation());
	InstantRotation(FacingEnemyDir);
	
	PlayerOwnerRef->PlayAnimMontage(CurrentPlayingMontage, 1, NAME_None);
}

// ==================================================== Dodge ===============================================

void UPlayerActionComponent::BeginDodge()
{
	// ref validation check
	if(PlayerOwnerRef == nullptr) return;
	
	// Get Player facing direction
	const FVector PlayerFaceDir = PlayerOwnerRef->GetActorForwardVector();

	// Get dodging direction
	const FVector PlayerDodgingDir = DecideDodgingDirection(PlayerFaceDir);

	// Get dodging montage depends on dodging direction
	UAnimMontage* PlayerDodgingMontage = FrontRollingMontage;
	InstantRotation(PlayerDodgingDir);
	//UAnimMontage* PlayerDodgingMontage = DecideDodgingMontage(PlayerDodgingDir);

	// Set action state and playing montage to dodging
	PlayerOwnerRef->SetCurrentActionState(EActionState::Dodge);
	CurrentPlayingMontage = PlayerDodgingMontage;

	// Set moving locations
	const FVector DodgeStart = PlayerOwnerRef->GetActorLocation();
	const FVector DodgingDest = DodgeStart + (PlayerDodgingDir * DodgeDistance);
	MovingStartPos = DodgeStart;
	MovingEndPos = DodgingDest;

	

	// Play both lerping timeline and montage
	PlayerOwnerRef->PlayAnimMontage(CurrentPlayingMontage, 1 , NAME_None);
	DodgeLerpingTimeLine.PlayFromStart();
}

FVector UPlayerActionComponent::DecideDodgingDirection(FVector PlayerFaceDir)
{
	const FInputDirection OwnerInputDirection = PlayerOwnerRef->GetPlayerInputDir();
	
	// if input direction is both 0, it means player didn't 
	if(OwnerInputDirection.GetInputDirectionX() == 0 && OwnerInputDirection.GetInputDirectionY() == 0)
	{
		const FVector PlayerBackDir = -1 * PlayerFaceDir;
		return PlayerBackDir;
	}
	
	// Get input direction
	const FVector SelfPos = PlayerOwnerRef->GetActorLocation();
	const FRotator Rotation = PlayerOwnerRef->Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector RightX = UKismetMathLibrary::GetRightVector(YawRotation);
	const FVector ForwardY = UKismetMathLibrary::GetForwardVector(YawRotation);
	const FVector RightInputDir = RightX * OwnerInputDirection.GetInputDirectionX();
	const FVector ForwardInputDir = ForwardY * OwnerInputDirection.GetInputDirectionY();
	const FVector InputDest = SelfPos + ((RightInputDir * 50) + (ForwardInputDir * 50));
	const FVector SelfToInputDestDir = UKismetMathLibrary::GetDirectionUnitVector(SelfPos, InputDest);

	return SelfToInputDestDir;
}

UAnimMontage* UPlayerActionComponent::DecideDodgingMontage(FVector PlayerDodgingDirection)
{
	// Get Player facing direction
	const FVector PlayerFaceDir = PlayerOwnerRef->GetActorForwardVector();
	
	const float DotProduct = UKismetMathLibrary::Dot_VectorVector(PlayerFaceDir, PlayerDodgingDirection);

	// Dot product is negative = player is dodging backward
	if(DotProduct < 0)
	{
		InstantRotation((-1 * PlayerDodgingDirection));
		return BackFlipMontage;
	}

	InstantRotation(PlayerDodgingDirection);
	return FrontRollingMontage;
}

// ==================================================== Utility ===============================================

void UPlayerActionComponent::TryToUpdateTarget()
{
	// Get All enemy around player
	TArray<AEnemyBase*> OpponentAroundSelf = GetAllOpponentAroundSelf();
	
	// if there is no opponent around, simply return
	if(OpponentAroundSelf.Num() == 0) return;
	
	// Get target direction to face to
	AEnemyBase* ResultFacingEnemy = GetTargetEnemy(OpponentAroundSelf);
	
	// if there is no direction, return
	if(ResultFacingEnemy == nullptr) return;
	
	// if result is not saved target enemy, update it for later usage
	if(ResultFacingEnemy && PlayerOwnerRef->GetTargetActor() != ResultFacingEnemy)
		PlayerOwnerRef->SetTargetActor(ResultFacingEnemy);
}

TArray<AEnemyBase*> UPlayerActionComponent::GetAllOpponentAroundSelf()
{
	TArray<AActor*> FoundActorList;
	TArray<AEnemyBase*> ReturnActors;
	
	const UWorld* World = GetWorld();
	if(World == nullptr) return ReturnActors;

	const FVector SelfPos = PlayerOwnerRef->GetActorLocation();

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerOwnerRef);
	
	UKismetSystemLibrary::SphereOverlapActors(World,SelfPos, DetectionRange, FilterType, FilteringClass, IgnoreActors,FoundActorList);

	if(FoundActorList.Num() != 0)
	{
		for (AActor* EachFoundActor : FoundActorList)
		{
			AEnemyBase* FoundCharacter = Cast<AEnemyBase>(EachFoundActor);
			if(FoundCharacter != nullptr) ReturnActors.Add(FoundCharacter);
		}
	}
	
	return ReturnActors;
}

void UPlayerActionComponent::InstantRotation(FVector RotatingVector)
{
	const FRotator InputRotation = UKismetMathLibrary::MakeRotFromX(RotatingVector);

	PlayerOwnerRef->SetActorRotation(InputRotation);
}

AEnemyBase* UPlayerActionComponent::GetTargetEnemy(TArray<AEnemyBase*> OpponentsAroundSelf)
{
	const FInputDirection OwnerInputDirection = PlayerOwnerRef->GetPlayerInputDir();

	
	const FVector SelfPos = PlayerOwnerRef->GetActorLocation();

	FVector SelfToInputDestDir;

	const float InputXValue = OwnerInputDirection.GetInputDirectionX(); 
	const float InputYValue = OwnerInputDirection.GetInputDirectionY(); 
	
	// find out which way is right
	if(InputXValue != 0 || InputYValue != 0)
	{
		const FRotator Rotation = PlayerOwnerRef->Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector RightX = UKismetMathLibrary::GetRightVector(YawRotation);
		const FVector ForwardY = UKismetMathLibrary::GetForwardVector(YawRotation);

		const FVector RightInputDir = RightX * InputXValue;
		const FVector ForwardInputDir = ForwardY * InputYValue;
		
		const FVector InputDest = SelfPos + ((RightInputDir * 50) + (ForwardInputDir * 50));

		SelfToInputDestDir = UKismetMathLibrary::GetDirectionUnitVector(SelfPos, InputDest);
	}
	else
	{
		// if there is no input direction, it means player didn't press movement key, it means attack will happen in front of player
		SelfToInputDestDir = PlayerOwnerRef->GetActorForwardVector();
	}
	
	// set first one as closest target and iterating from opponents list
	AEnemyBase* ReturnTarget = OpponentsAroundSelf[0];
	
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
void UPlayerActionComponent::MovingAttackMovement(float Alpha)
{
	const FVector CharacterCurrentPos = PlayerOwnerRef->GetActorLocation();
	
	const FVector MovingPos = UKismetMathLibrary::VLerp(MovingStartPos, MovingEndPos, Alpha);

	const FVector LaunchingPos = FVector(MovingPos.X, MovingPos.Y, CharacterCurrentPos.Z);

	PlayerOwnerRef->SetActorLocation(LaunchingPos);
}


// ========================================================= Delegate Function =================================================

void UPlayerActionComponent::ExecutePlayerAction(EActionState ExecutingAction)
{
	switch (ExecutingAction)
	{
		case EActionState::Idle:
			
			break;
		case EActionState::Evade:
			BeginEvade();
			break;
		case EActionState::Attack:
			BeginMeleeAttack();
			break;
		case EActionState::Recovering:
			
			break;
		case EActionState::ReceivedDamage:
			
			break;
		case EActionState::SpecialAttack:
			
			break;
		case EActionState::WaitForCombo:
			
			break;
		case EActionState::Dodge:
			BeginDodge();
			break;
		default: break;
	}
}


void UPlayerActionComponent::ReceivingDamage(int32 DamageAmount, AActor* DamageCauser,
	EEnemyAttackType ReceivingAttackType)
{
	// early return if player is dodging, since player is not getting damaged when dodging
	EActionState PlayerCurrentAction = PlayerOwnerRef->GetCurrentActionState();

	if(PlayerCurrentAction == EActionState::Dodge) return;
	
	
	// TODO: Check EnemyAttackType before performing counter
	
	if(IsPlayerCountering(PlayerCurrentAction, ReceivingAttackType))
	{
		PlayerOwnerRef->StopAnimMontage();
		BeginCounterAttack(DamageCauser);
		return;
	}
	

	if(IsPlayerCanBeDamaged(PlayerCurrentAction, ReceivingAttackType))
	{
		if(PlayerCurrentAction == EActionState::Attack)
		{
			PlayerOwnerRef->StopAnimMontage();	
		}

		// TODO: Damage player
		

		// Player Play receiving damage montage
		if(ReceiveDamageMontage == nullptr) return;

		PlayerOwnerRef->SetCurrentActionState(EActionState::Recovering);
		
	
		CurrentPlayingMontage = ReceiveDamageMontage;
		
		PlayerOwnerRef->PlayAnimMontage(CurrentPlayingMontage, 1, NAME_None);
	}
}

bool UPlayerActionComponent::IsPlayerCountering(EActionState PlayerCurrentAction, EEnemyAttackType ReceivingAttackType)
{
	// return true if player is in counter state and enemy's incoming attack is counterable
	return PlayerCurrentAction == EActionState::Evade && ReceivingAttackType == EEnemyAttackType::AbleToCounter;
}

bool UPlayerActionComponent::IsPlayerCanBeDamaged(EActionState PlayerCurrentAction,
	EEnemyAttackType ReceivingAttackType)
{
	// return true if player is standing or attacking or player is countering but incoming attack is not counterable
	return PlayerCurrentAction == EActionState::Idle || PlayerCurrentAction == EActionState::Attack || (PlayerCurrentAction == EActionState::Evade && ReceivingAttackType == EEnemyAttackType::UnableToCounter);
}

