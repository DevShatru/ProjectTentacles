// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Enemies/EnemyBrute.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"



void AEnemyBrute::BeginPlay()
{
	Super::BeginPlay();

	InitializeTimelineComp();
}

void AEnemyBrute::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UnCounterMovingTimeline.TickTimeline(DeltaSeconds);
	CounterableMovingTimeline.TickTimeline(DeltaSeconds);

}

void AEnemyBrute::TryToDamagePlayer_Implementation()
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

void AEnemyBrute::TryTriggerPlayerCounter_Implementation()
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

void AEnemyBrute::OnCounterTimeEnd_Implementation()
{
	Super::OnCounterTimeEnd_Implementation();

	OnHideAttackIndicator();

	if(!IsCountered)
	{
		ACharacter* PlayerCharacterClass = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
		ICharacterActionInterface::Execute_TryRemoveCounterTarget(PlayerCharacterClass, this);
	}

}

void AEnemyBrute::OnSetIsCountered_Implementation(bool Countered)
{
	Super::OnSetIsCountered_Implementation(Countered);

	SetIsCountered(Countered);
}

void AEnemyBrute::OnStartCounteredAnimation_Implementation()
{
	Super::OnStartCounteredAnimation_Implementation();

	//TrySwitchEnemyState(EEnemyCurrentState::Countered);

	PlayReceiveCounterAnimation();
}

void AEnemyBrute::ReceiveDamageFromPlayer_Implementation(int32 DamageAmount, AActor* DamageCauser,
	EPlayerAttackType PlayerAttackType)
{
	Super::ReceiveDamageFromPlayer_Implementation(DamageAmount, DamageCauser, PlayerAttackType);
	
	// if player is doing counter attack damage
	if(PlayerAttackType == EPlayerAttackType::CounterAttack)
	{
		TryFinishAttackTask(EEnemyCurrentState::Countered);

		if(CounteredTime == 0)
		{
			CounteredTime += 1;
			return;
		}
		
		SetIsCountered(false);
	}

}

void AEnemyBrute::InitializeTimelineComp()
{
	// timeline binding
	FOnTimelineFloat MovingAttackPosUpdate;
	MovingAttackPosUpdate.BindDynamic(this, &AEnemyBrute::UpdateAttackingPosition);
	UnCounterMovingTimeline.AddInterpFloat(UncounterableAttackMovingCurve, MovingAttackPosUpdate);
	CounterableMovingTimeline.AddInterpFloat(CounterableAttackMovingCurve, MovingAttackPosUpdate);
}


void AEnemyBrute::PlaySpecificAttackMovingTimeline(EEnemyAttackType EnemyAttack)
{
	switch (EnemyAttack)
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

void AEnemyBrute::TestFunction()
{
	FVector PlayerPos = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();

	FVector CurrentPos = GetActorLocation();

	FVector DirFromPlayer = UKismetMathLibrary::Normal((CurrentPos - PlayerPos));

	SetActorLocation(PlayerPos + (DirFromPlayer * 200));
	
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(CurrentPos, PlayerPos));
	
	ExecuteAttack();
}

void AEnemyBrute::ExecuteAttack()
{
	Super::ExecuteAttack();

	// testing
	CurrentAttackType = EEnemyAttackType::AbleToCounter;

	//UpdateAttackDestination();

	RemainAttackDistance = AttackTravelDistance_Counterable;
	float DeltaSecond = GetWorld()->GetDeltaSeconds();
	float NumOfTickPerSecond = 1 / DeltaSecond;
	TravelDistancePerTick = AttackTravelDistance_Counterable / (ToTalTravelTime_Counterable * NumOfTickPerSecond);
	
	
	// switch case on current attack type to fire different animation 
	switch (CurrentAttackType)
	{
	case EEnemyAttackType::AbleToCounter:
		if(CounterableAttackMontage != nullptr)
		{
			PlayAnimMontage(CounterableAttackMontage, 1, "Default");
			PlaySpecificAttackMovingTimeline(CurrentAttackType);
		}
		break;
	case EEnemyAttackType::UnableToCounter:
		if(NotCounterableAttackMontage != nullptr)
			PlayAnimMontage(NotCounterableAttackMontage, 1, "Default");
		break;
	default: break;
	}
	


}

FVector AEnemyBrute::CalculateDestinationForAttackMoving(FVector PlayerCurrentPos, float CurrentTimelineAlpha)
{
	// Get direction from self to player
	FVector OffsetWithoutZ = PlayerCurrentPos - SelfAttackStartPos;
	OffsetWithoutZ.Z = 0;
	const FVector DirFromSelfToPlayer = UKismetMathLibrary::Normal(OffsetWithoutZ);

	// if enemy's attack is swiping
	if(CurrentAttackType == EEnemyAttackType::AbleToCounter)
	{
		const FVector SupposedAttackMovingDestination = SelfAttackStartPos + (DirFromSelfToPlayer * 50);
		return SupposedAttackMovingDestination;
	
		// // Hit result
		// FHitResult Hit;
		// // Empty array of ignoring actor, maybe add Enemies classes to be ignored
		// TArray<AActor*> IgnoreActors;
		// IgnoreActors.Add(this);
		//
		// // Capsule trace by channel
		// const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, SelfAttackStartPos, SupposedAttackMovingDestination, UEngineTypes::ConvertToTraceType(ECC_Visibility),false, IgnoreActors,  EDrawDebugTrace::None,Hit,true);
		//
		// if(!bHit) return SupposedAttackMovingDestination;
		//
		// const FVector DirFromPlayerToSelf = DirFromSelfToPlayer * -1;
		//
		//
		//
		// return Hit.ImpactPoint + (DirFromPlayerToSelf * OffsetFromPlayer);
	}
	else
	{
		// if enemy's attack is charging
		const FVector CurrentPos = GetActorLocation();
		const float RemainingMovingDistance = ChargingDistance * (1 - CurrentTimelineAlpha);
		const FVector SupposedAttackMovingDestination = CurrentPos + (DirFromSelfToPlayer * RemainingMovingDistance);
		
		return SupposedAttackMovingDestination;
	}
	
}

void AEnemyBrute::UpdateAttackingPosition(float Alpha)
{
	// TODO: REWRITE WHOLE THING

	const ACharacter* PlayerCha = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
	if(!PlayerCha) return;

	const FVector PlayerPos = PlayerCha->GetActorLocation();
	const FVector CurrentLocation = GetActorLocation();

	const float Displacement = UKismetMathLibrary::Vector_Distance(PlayerPos, CurrentLocation);
	
	if(RemainAttackDistance > TravelDistancePerTick && Displacement > 90.0f)
	{
		RemainAttackDistance -= TravelDistancePerTick;
		
		// Get direction from self to player
		FVector OffsetWithoutZ = PlayerPos - CurrentLocation;
		OffsetWithoutZ.Z = 0;
		const FVector DirFromSelfToPlayer = UKismetMathLibrary::Normal(OffsetWithoutZ);
		const FVector SupposedMovingPos = CurrentLocation + (DirFromSelfToPlayer * TravelDistancePerTick);


		
		
		
		SetActorLocation(SupposedMovingPos);

		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(CurrentLocation, PlayerPos));
	}
	

	


	
	
	// // Update lerping destination if enemy attack tracking is enable and current Alpha is not exceeding limit
	//
	// 		const ACharacter* PlayerCha = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
	// 		if(!PlayerCha) return;
	//
	// 		const FVector PlayerPos = PlayerCha->GetActorLocation();
	//
	// 		AttackMovingDestination = CalculateDestinationForAttackMoving(PlayerPos, Alpha);
	//
	// DebugCircle(AttackMovingDestination);
	//
	// //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("PlayerPos: %s  DestinationPos: %s"), *PlayerPos.ToString(), *AttackMovingDestination.ToString()));
	//
	// const FVector CharacterCurrentPos = GetActorLocation();
	//
	// const FVector LerpPos = UKismetMathLibrary::VLerp(SelfAttackStartPos,AttackMovingDestination, Alpha);
	//
	// const FVector MovingPos = FVector(LerpPos.X, LerpPos.Y, CharacterCurrentPos.Z);
	//
	// SetActorLocation(MovingPos);
}

void AEnemyBrute::OnContinueSecondAttackMontage_Implementation()
{
	Super::OnContinueSecondAttackMontage_Implementation();
	
	FVector PlayerPos = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
	FVector CurrentPos = GetActorLocation();
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(CurrentPos, PlayerPos));

	PlaySpecificAttackMovingTimeline(CurrentAttackType);

	

	if(CurrentAttackType == EEnemyAttackType::AbleToCounter)
	{
		RemainAttackDistance = AttackTravelDistance_Counterable;
		float DeltaSecond = GetWorld()->GetDeltaSeconds();
		float NumOfTickPerSecond = 1 / DeltaSecond;
		TravelDistancePerTick = AttackTravelDistance_Counterable / (ToTalTravelTime_Counterable * NumOfTickPerSecond);
		
		StopAnimMontage();
		PlayAnimMontage(CouterableAttackSecond, 1, "Default");
		PlaySpecificAttackMovingTimeline(CurrentAttackType);
		return;
	}

	StopAnimMontage();
	PlayAnimMontage(UnableCounterAttackSecond, 1, "Default");
	
	

}

void AEnemyBrute::RecoverFromStunState()
{
	TrySwitchEnemyState(EEnemyCurrentState::WaitToAttack);	

	IsStunned = false;

	TryResumeMoving();
}

TArray<AActor*> AEnemyBrute::GetActorsInFrontOfEnemy(bool IsDamaging)
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

void AEnemyBrute::TryStopAttackMovement()
{
}

void AEnemyBrute::UpdateAttackDestination()
{
	// Save lerp start and end position for later timeline function usage
	SelfAttackStartPos = GetActorLocation();

	// Get Player Position
	const ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	FVector PlayerCurrentPos = PlayerCharacter->GetActorLocation();
		
	const FVector DestinationPos = CalculateDestinationForAttackMoving(PlayerCurrentPos, 0);
	AttackMovingDestination = DestinationPos;
}

void AEnemyBrute::PlayReceiveCounterAnimation()
{
	StopAnimMontage();
	PlayAnimMontage(StunAnimation, 1, "Default");
	
	
	GetWorld()->GetTimerManager().SetTimer(StunningTimerHandle, this, &AEnemyBrute::RecoverFromStunState, TotalStunDuration, false, -1);
}
