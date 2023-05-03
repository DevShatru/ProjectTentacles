// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Enemies/EnemyMelee.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// ======================================================= Attack =======================================================
void AEnemyMelee::ExecuteAttack()
{
	Super::ExecuteAttack();

	

	// Debug Attack Moving timeline activate
	if(EnableAttackMovement)
	{
		// Save lerp start and end position for later timeline function usage
		SelfAttackStartPos = GetActorLocation();

		// Get Player Position
		const ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		FVector PlayerCurrentPos = PlayerCharacter->GetActorLocation();
		
		const FVector DestinationPos = CalculateDestinationForAttackMoving(PlayerCurrentPos);
		AttackMovingDestination = DestinationPos;
		
		PlaySpecificAttackMovingTimeline(CurrentAttackType);
	}
	
	// switch case on current attack type to fire different animation 
	switch (CurrentAttackType)
	{
		case EEnemyAttackType::AbleToCounter:
			if(CounterableAttackMontage != nullptr)
				PlayAnimMontage(CounterableAttackMontage, 1, "Default");
			break;
		case EEnemyAttackType::UnableToCounter:
			if(NotCounterableAttackMontage != nullptr)
				PlayAnimMontage(NotCounterableAttackMontage, 1, "Default");
			break;
		default: break;
	}


}

void AEnemyMelee::OnDeath()
{
	if(bShouldHealOnDeath && OwnController)	OwnController->HealEncounterTarget(HealAmount);
	Super::OnDeath();
}

FVector AEnemyMelee::CalculateDestinationForAttackMoving(FVector PlayerPos)
{
	// Get direction from self to player
	FVector OffsetWithoutZ = PlayerPos - SelfAttackStartPos;
	OffsetWithoutZ.Z = 0;
	const FVector DirFromSelfToPlayer = UKismetMathLibrary::Normal(OffsetWithoutZ);

	const FVector SupposedAttackMovingDestination = SelfAttackStartPos + (DirFromSelfToPlayer * AttackMovingDistance);

	
	// Hit result
	FHitResult Hit;
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	
	// Capsule trace by channel
	const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, SelfAttackStartPos, SupposedAttackMovingDestination, UEngineTypes::ConvertToTraceType(ECC_Visibility),false, IgnoreActors,  EDrawDebugTrace::None,Hit,true);

	if(!bHit) return SupposedAttackMovingDestination;

	const FVector DirFromPlayerToSelf = DirFromSelfToPlayer * -1;
	
	return Hit.ImpactPoint + (DirFromPlayerToSelf * OffsetFromPlayer);
}

void AEnemyMelee::PlaySpecificAttackMovingTimeline(EEnemyAttackType AttackType)
{
	switch (AttackType)
	{
	case EEnemyAttackType::AbleToCounter:
		CounterableMovingTimeline.PlayFromStart();
		break;
	case EEnemyAttackType::UnableToCounter:
		UnCounterMovingTimeline.PlayFromStart();
		break;
	default: break;
	}
}


// ===================================================== Stunning ===========================================
void AEnemyMelee::StartCounteredAnimation()
{
	if(!CounterVictimMontage) return;
	
	StopAnimMontage();
	OnHideAttackIndicator();
	PlayAnimMontage(CounterVictimMontage,1,"Start");
}


void AEnemyMelee::BeginLyingCountDown()
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	PlayLyingMontage();
	
	World->GetTimerManager().SetTimer(GettingUpTimerHandle,this, &AEnemyMelee::RecoverFromLying, TimeToGetUp, false, -1);
}

void AEnemyMelee::PlayLyingMontage()
{
	if(!CounterVictimMontage) return;
	
	StopAnimMontage();
	PlayAnimMontage(CounterVictimMontage,1,"LyingLoop");
}


void AEnemyMelee::RecoverFromLying()
{
	if(!GetUpMontage) return;

	TrySwitchEnemyState(EEnemyCurrentState::WaitToAttack);	

	StopAnimMontage();
	PlayAnimMontage(GetUpMontage,2,"Default");
}

// =================================================== Utility =======================================================

TArray<AActor*> AEnemyMelee::GetActorsInFrontOfEnemy(bool IsDamaging)
{
	const UWorld* World = GetWorld();
	
	FName AttackSocketName;
	if(CurrentAttackType == EEnemyAttackType::AbleToCounter)
		AttackSocketName = "RightHand";
	else
		AttackSocketName = "RightFoot";
	
	const FVector HeadSocketLocation = GetMesh()->GetSocketLocation(AttackSocketName);

	TArray<AActor*> FoundActorList;


	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	
	TArray<AEnemyBase*> Allies = OwnController->GetAllies();
	
	if(Allies.Num() > 0)
		for (AEnemyBase* EachAlly : Allies)
		{
			IgnoreActors.Add(EachAlly);
		}

	float Radius;
	float Height;
	
	if(IsDamaging)
	{
		Radius = DamageTriggerRadius;
		Height = DamageTriggerHeight;
	}
	else
	{
		Radius = CounterTriggerRadius;
		Height = CounterTriggerHeight;
	}
	
	
	UKismetSystemLibrary::CapsuleOverlapActors(World, HeadSocketLocation, Radius, Height, FilterType, FilteringClass, IgnoreActors, FoundActorList);

	return FoundActorList;
}



// =================================================== Begin, Tick =======================================================
void AEnemyMelee::BeginPlay()
{
	Super::BeginPlay();

	InitializeTimelineComp();
}

void AEnemyMelee::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// TODO: Melee Class
	UnCounterMovingTimeline.TickTimeline(DeltaSeconds);
	CounterableMovingTimeline.TickTimeline(DeltaSeconds);

}

void AEnemyMelee::InitializeTimelineComp()
{
	// timeline binding
	FOnTimelineFloat MovingAttackPosUpdate;
	MovingAttackPosUpdate.BindDynamic(this, &AEnemyMelee::UpdateAttackingPosition);
	UnCounterMovingTimeline.AddInterpFloat(UncounterableAttackMovingCurve, MovingAttackPosUpdate);
	CounterableMovingTimeline.AddInterpFloat(CounterableAttackMovingCurve, MovingAttackPosUpdate);
}

void AEnemyMelee::TryStopAttackMovement()
{
	if(EnableAttackMovement)
	{
		UnCounterMovingTimeline.Stop();
		CounterableMovingTimeline.Stop();
	}
}

// ================================================== Timeline Functions ============================================

void AEnemyMelee::UpdateAttackingPosition(float Alpha)
{
	// Update lerping destination if enemy attack tracking is enable and current Alpha is not exceeding limit
	if(EnableEnemyAttackTracking)
		if(Alpha < AttackTrackingLimitInAlpha)
		{
			const ACharacter* PlayerCha = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
			if(!PlayerCha) return;

			const FVector PlayerPos = PlayerCha->GetActorLocation();
	
			AttackMovingDestination = CalculateDestinationForAttackMoving(PlayerPos);
		}

	const FVector CharacterCurrentPos = GetActorLocation();
	
	const FVector LerpPos = UKismetMathLibrary::VLerp(SelfAttackStartPos,AttackMovingDestination, Alpha);

	const FVector MovingPos = FVector(LerpPos.X, LerpPos.Y, CharacterCurrentPos.Z);

	SetActorLocation(MovingPos);
}



// ================================================== Interface Functions ============================================
void AEnemyMelee::TryToDamagePlayer_Implementation()
{
	Super::TryToDamagePlayer_Implementation();

	TArray<AActor*> FoundActorList = GetActorsInFrontOfEnemy(true);

	if(FoundActorList.Num() != 0)
	{
		for (AActor* EachFoundActor : FoundActorList)
		{
			IDamageInterface::Execute_ReceiveDamageFromEnemy(EachFoundActor, BaseDamageAmount, this, CurrentAttackType);
		}
	}
}

void AEnemyMelee::TryTriggerPlayerCounter_Implementation()
{
	Super::TryTriggerPlayerCounter_Implementation();

	TArray<AActor*> FoundActorList = GetActorsInFrontOfEnemy(false);

	if(FoundActorList.Num() != 0)
	{
		for (AActor* EachFoundActor : FoundActorList)
		{
			ICharacterActionInterface::Execute_TryStoreCounterTarget(EachFoundActor, this);
		}
	}
}

void AEnemyMelee::OnCounterTimeEnd_Implementation()
{
	Super::OnCounterTimeEnd_Implementation();

	OnHideAttackIndicator();

	if(!IsCountered)
	{
		ACharacter* PlayerCharacterClass = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
		ICharacterActionInterface::Execute_TryRemoveCounterTarget(PlayerCharacterClass, this);
	}
}

void AEnemyMelee::OnSetIsCountered_Implementation(bool Countered)
{
	Super::OnSetIsCountered_Implementation(Countered);
	
	SetIsCountered(Countered);
}

void AEnemyMelee::OnStartCounteredAnimation_Implementation()
{
	Super::OnStartCounteredAnimation_Implementation();

	StartCounteredAnimation();
}

void AEnemyMelee::ReceiveDamageFromPlayer_Implementation(int32 DamageAmount, AActor* DamageCauser,
                                                         EPlayerAttackType PlayerAttackType)
{
	Super::ReceiveDamageFromPlayer_Implementation(DamageAmount, DamageCauser, PlayerAttackType);

	bool StateChanged = false;
	
	// if enemy is attack, stop montage, flip bool to false, unshow attack indicator, and execute onfinish attack delegate
	if(CurrentEnemyState == EEnemyCurrentState::Attacking && PlayerAttackType != EPlayerAttackType::CounterAttack)
	{
		StopAnimMontage();
		
		OnHideAttackIndicator();

		// Stop attack movement
		TryStopAttackMovement();
		
		TryFinishAttackTask(EEnemyCurrentState::Damaged);

		// change StateChanged bool to true to prevent changing again
		StateChanged = true;

	}
	
	// if player is doing counter attack damage
	if(PlayerAttackType == EPlayerAttackType::CounterAttack)
	{
		TryFinishAttackTask(EEnemyCurrentState::Countered);

		SetIsCountered(false);
		
		// change StateChanged bool to true to prevent changing again
		StateChanged = true;
	}
	
	HealthReduction(DamageAmount);

	
	// if bool StateChanged is false, it means enemy is not taking damage when it get countered or get damaged while doing attack
	if(!StateChanged) TrySwitchEnemyState(EEnemyCurrentState::Damaged);
	
	if((Health - DamageAmount) > 0)
	{
		PlayReceiveDamageAnimation(PlayerAttackType);
		return;
	}
	
	OnDeath();

}


void AEnemyMelee::StartLyingOnTheGround_Implementation()
{
	ICharacterActionInterface::StartLyingOnTheGround_Implementation();


	// TODO: Maybe don't need
	//CurrentEnemyState = EEnemyCurrentState::Countered;
	
	BeginLyingCountDown();
}

void AEnemyMelee::RepeatLyingOnTheGround_Implementation()
{
	ICharacterActionInterface::RepeatLyingOnTheGround_Implementation();

	PlayLyingMontage();
}