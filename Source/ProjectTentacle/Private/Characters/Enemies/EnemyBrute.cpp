// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Enemies/EnemyBrute.h"

#include "Characters/Player/PlayerDamageInterface.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


AEnemyBrute::AEnemyBrute()
{

}

void AEnemyBrute::BeginPlay()
{
	Super::BeginPlay();

	InitializeTimelineComp();

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	if(CapsuleComp)
	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBrute::OnDealChargeDamage);
}
	
void AEnemyBrute::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ChargeMovingTimeline.TickTimeline(DeltaSeconds);
	CounterableMovingTimeline.TickTimeline(DeltaSeconds);
	JumpSlamMovingTimeline.TickTimeline(DeltaSeconds);
}

void AEnemyBrute::TryToDamagePlayer_Implementation()
{
	Super::TryToDamagePlayer_Implementation();

	if(IsSecondAttack || BruteAttack == EBruteAttackType::JumpSlam) TryFinishAttackTask(EEnemyCurrentState::WaitToAttack);
	
	TArray<AActor*> FoundActorList = GetActorsInFrontOfEnemy(true);

	if(FoundActorList.Num() != 0)
	{
		for (AActor* EachFoundActor : FoundActorList)
		{
			IPlayerDamageInterface::Execute_ReceiveDamageFromEnemy(EachFoundActor, BaseDamageAmount, this, CurrentAttackType);
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

void AEnemyBrute::ReceiveDamageFromPlayer_Implementation(float DamageAmount, AActor* DamageCauser,
	EPlayerAttackType PlayerAttackType)
{
	Super::ReceiveDamageFromPlayer_Implementation(DamageAmount, DamageCauser, PlayerAttackType);
	
	// if player is doing counter attack damage
	if(PlayerAttackType == EPlayerAttackType::CounterAttack)
	{

		if(CounteredTime == 0)
		{
			CounteredTime += 1;
			PlayAnimMontage(DamageReceiveAnimation, 1, "Default");
			return;
		}

		OnStunned();
		
		// TryFinishAttackTask(EEnemyCurrentState::Stunned);
		//
		// TrySwitchEnemyState(EEnemyCurrentState::Stunned);
		//
		// StopAnimMontage();
		// PlayAnimMontage(StunAnimation, 1, "Default");
		//
		// GetWorld()->GetTimerManager().SetTimer(StunningTimerHandle, this, &AEnemyBrute::RecoverFromStunState, TotalStunDuration, false, -1);
		//
		SetIsCountered(false);
		return;
	}

	PlayAnimMontage(DamageReceiveAnimation, 1, "Default");
}

void AEnemyBrute::OnDealAoeDamage_Implementation()
{
	Super::OnDealAoeDamage_Implementation();

	TryFinishAttackTask(EEnemyCurrentState::WaitToAttack);
	
	TArray<AActor*> FoundActorList = GetActorsAroundEnemy();

	if(FoundActorList.Num() != 0)
	{
		for (AActor* EachFoundActor : FoundActorList)
		{
			IPlayerDamageInterface::Execute_ReceiveDamageFromEnemy(EachFoundActor, BaseDamageAmount, this, CurrentAttackType);
		}
	}
}

void AEnemyBrute::InitializeTimelineComp()
{
	// timeline binding
	FOnTimelineFloat MovingAttackPosUpdate;
	MovingAttackPosUpdate.BindDynamic(this, &AEnemyBrute::UpdateAttackingPosition);
	CounterableMovingTimeline.AddInterpFloat(CounterableAttackMovingCurve, MovingAttackPosUpdate);
	JumpSlamMovingTimeline.AddInterpFloat(JumpSlamHeightCurve, MovingAttackPosUpdate);
	ChargeMovingTimeline.AddInterpFloat(UncounterableAttackMovingCurve, MovingAttackPosUpdate);
}


void AEnemyBrute::PlaySpecificAttackMovingTimeline(EEnemyAttackType EnemyAttack)
{
	// switch case to play specific timeline
	switch (EnemyAttack)
	{
	case EEnemyAttackType::AbleToCounter:
		CounterableMovingTimeline.PlayFromStart();
		break;
	case EEnemyAttackType::UnableToCounter:
		if(BruteAttack == EBruteAttackType::Charging) ChargeMovingTimeline.PlayFromStart();
		else JumpSlamMovingTimeline.PlayFromStart();
		break;
	default: break;
	}
}

EBruteAttackType AEnemyBrute::FindExecutingAttackType()
{

	// TODO: Debug Testing
	// SetExecutingAttackTypes(DebugAttackType);
	// return DebugAttackType;
	
	// Get player position and brute's position
	const ACharacter* PlayerCha = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
	// if somehow player is invalid, return charging
	if(!PlayerCha)
	{
		SetExecutingAttackTypes(EBruteAttackType::Charging);
		return EBruteAttackType::Charging;
	}
	
	const FVector PlayerPos = PlayerCha->GetActorLocation();
	FVector CurrentLocationWithSameZ = GetActorLocation();
	CurrentLocationWithSameZ.Z = PlayerPos.Z;
	
	// if distance is close enough, get random float to see execute swipe or charge 
	const float Distance = UKismetMathLibrary::Vector_Distance(CurrentLocationWithSameZ, PlayerPos);
	if(Distance < DistanceToDecideFarOrClose)
	{
		const float RndChance = UKismetMathLibrary::RandomFloatInRange(0,100);
		if(RndChance < ChanceToExecuteCharging)
		{
			SetExecutingAttackTypes(EBruteAttackType::Charging);
			return EBruteAttackType::Charging;
		}
		
		SetExecutingAttackTypes(EBruteAttackType::Swipe);
		return EBruteAttackType::Swipe;
	}

	// if distance is far enough, execute jump slam
	SetExecutingAttackTypes(EBruteAttackType::JumpSlam);
	return EBruteAttackType::JumpSlam;
}

void AEnemyBrute::SetExecutingAttackTypes(EBruteAttackType ExecutingBruteAction)
{
	BruteAttack = ExecutingBruteAction;

	switch (BruteAttack)
	{
	case EBruteAttackType::Swipe:
		CurrentAttackType = EEnemyAttackType::AbleToCounter;
		break;
	case EBruteAttackType::Charging:
		CurrentAttackType = EEnemyAttackType::UnableToCounter;
		break;
	case EBruteAttackType::JumpSlam:
		CurrentAttackType = EEnemyAttackType::UnableToCounter;
		break;
	default: break;
	}
}

void AEnemyBrute::TestFunction()
{
	PlayAnimMontage(DamageReceiveAnimation, 1, "Default");
}

void AEnemyBrute::TryGetPlayerRef()
{
	if(!PlayerRef)
	{
		ACharacter* PlayerCha = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		APlayerCharacter* CastedPlayer = Cast<APlayerCharacter>(PlayerCha);
		if(CastedPlayer)
			PlayerRef = CastedPlayer;
	}
}

void AEnemyBrute::ExecuteAttack()
{
	Super::ExecuteAttack();

	// TODO: Debug Testing
	// {
	// 	FVector PlayerPos = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
	//
	// 	FVector CurrentPos = GetActorLocation();
	//
	// 	FVector DirFromPlayer = UKismetMathLibrary::Normal((CurrentPos - PlayerPos));
	//
	// 	float TestDistance = 0;
	//
	// 	EBruteAttackType DecidedAttackType = FindExecutingAttackType();
	// 	SetExecutingAttackTypes(DecidedAttackType);
	//
	// 	if(DecidedAttackType == EBruteAttackType::Swipe)
	// 		TestDistance = 200;
	// 	else if(DecidedAttackType == EBruteAttackType::Charging)
	// 		TestDistance = 1200;
	// 	else
	// 		TestDistance = 700;
	// 	
	//
	// 	SetActorLocation(PlayerPos + (DirFromPlayer * TestDistance));
	// 	
	// }

	// Allocate player reference if player reference variable is not valid
	TryGetPlayerRef();
	
	
	
	if(AttackIndicatorRef)
		AttackIndicatorRef->OnReceivingNewAttackType(CurrentAttackType);
	
	UpdateAttackingVariables();
	
	IsSecondAttack = false;
	
	// switch case on current attack type to fire different animation 
	switch (BruteAttack)
	{
		case EBruteAttackType::Swipe:
			PlayAnimMontage(CounterableAttackMontage, 1, "Default");
			PlaySpecificAttackMovingTimeline(CurrentAttackType);
			break;
		case EBruteAttackType::Charging:
			if(NotCounterableAttackMontage != nullptr)
				PlayAnimMontage(NotCounterableAttackMontage, 1, "Default");
			DoesPlayerDodge = false;
		
			break;
		case EBruteAttackType::JumpSlam:
			if(JumpSlamAttack != nullptr)
				PlayAnimMontage(JumpSlamAttack, 1, "Default");
			PlaySpecificAttackMovingTimeline(CurrentAttackType);
			DoesPlayerDodge = false;

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
	const ACharacter* PlayerCha = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
	if(!PlayerCha) return;

	const FVector PlayerPos = PlayerCha->GetActorLocation();
	const FVector CurrentLocation = GetActorLocation();


	// Get direction from self to player
	FVector OffsetWithoutZ = PlayerPos - CurrentLocation;
	OffsetWithoutZ.Z = 0;
	const FVector DirFromSelfToPlayer = UKismetMathLibrary::Normal(OffsetWithoutZ);
	
	// if brute's attack is swiping
	if(BruteAttack == EBruteAttackType::Swipe)
	{
		const float Displacement = UKismetMathLibrary::Vector_Distance(PlayerPos, CurrentLocation);
		
		if(RemainAttackDistance > TravelDistancePerTick && Displacement > 120.0f)
		{
			RemainAttackDistance -= TravelDistancePerTick;
			
			const FVector SupposedMovingPos = CurrentLocation + (DirFromSelfToPlayer * TravelDistancePerTick);
			
			SetActorLocation(SupposedMovingPos);

			SetActorRotation(UKismetMathLibrary::FindLookAtRotation(CurrentLocation, PlayerPos));
		}
		
		return;
	}


	if(!DoesPlayerDodge && CheckIfPlayerDodge()) DoesPlayerDodge = true;
	
	// if brute's attack is charging
	if(BruteAttack == EBruteAttackType::Charging)
	{
		const bool ShouldCharge = ShouldKeepCharging(DirFromSelfToPlayer);
		if(!ShouldCharge)
		{
			StopAnimMontage();
			ChargeMovingTimeline.Stop();
			SetCapsuleCompCollision(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
			OnSetFocus();
			TryResumeMoving();
			TryFinishAttackTask(EEnemyCurrentState::WaitToAttack);
			return;
		}
		
		
		RemainAttackDistance -= TravelDistancePerTick;


		
		FVector ChargingDirection = GetActorForwardVector();
		if(!DoesPlayerDodge) ChargingDirection = DirFromSelfToPlayer;
		//if(!DoesPlayerDodge) ChargingDirection = GetChargeDirection(DirFromSelfToPlayer, CurrentLocation);

		
		const FVector SupposedMovingPos = CurrentLocation + (ChargingDirection * TravelDistancePerTick);
		
		SetActorLocation(SupposedMovingPos);
		
		// record moving direction
		FVector AttackMovingOffsetWithoutZ = SupposedMovingPos - CurrentLocation;
		AttackMovingOffsetWithoutZ.Z = 0;
		AttackMovingDir = UKismetMathLibrary::Normal(AttackMovingOffsetWithoutZ);

		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(CurrentLocation, SupposedMovingPos));

		return;
	}


	// if Brute is performing jumpslam
	
	RemainAttackDistance -= TravelDistancePerTick;

	const float GroundPlayBackPos = JumpSlamMovingTimeline.GetPlaybackPosition();
	const float GroundAlpha = JumpSlamDistanceCurve->GetFloatValue(GroundPlayBackPos);
	
	// Check if player does dodge, if not, update JumpSlamPosition
	if(!DoesPlayerDodge && GroundPlayBackPos < 1.9) EndJumpingLocation = GetJumpSlamPosition((DirFromSelfToPlayer * -1), PlayerPos);

	
	FVector SupposedMovingPos = UKismetMathLibrary::VLerp(StartJumpingLocation, EndJumpingLocation, GroundAlpha);
	
	float SupposedHeight = UKismetMathLibrary::Lerp(StartJumpingLocation.Z, StartJumpingLocation.Z + JumpSlamHeight, Alpha);

	SupposedMovingPos.Z = SupposedHeight;

	// TODO: Fix Rotation
	//SetActorRotation(UKismetMathLibrary::FindLookAtRotation(CurrentLocation, SupposedMovingPos));
	
	SetActorLocation(SupposedMovingPos);
	
}

bool AEnemyBrute::CheckIfPlayerDodge()
{
	if(!PlayerRef) return false;

	
	return PlayerRef->GetCurrentActionState() == EActionState::Dodge;
}


FVector AEnemyBrute::GetChargeDirection(FVector DirToPlayer, FVector ActorCurrentPos)
{
	const FVector ForwardDir = GetActorForwardVector();
	const FVector RightDir = GetActorRightVector();
	const FVector LeftDir = RightDir * -1 ;


	const float DotResult = UKismetMathLibrary::Dot_VectorVector(RightDir,DirToPlayer);
	const bool IsRight = DotResult > 0;

	const float DotAngleOffset = 90 * (1 - (UKismetMathLibrary::Abs(DotResult)));


	float DeltaSecond = GetWorld()->GetDeltaSeconds();
	float NumOfTickPerSecond = 1 / DeltaSecond;
	float TrackingAnglePerTick = ChargeTrackingAngle / NumOfTickPerSecond;
	
	if(DotAngleOffset < TrackingAnglePerTick) return DirToPlayer;


	
	const float ReturnAlpha = TrackingAnglePerTick / 90;

	if(IsRight)
	{
		return UKismetMathLibrary::VLerp(ForwardDir, RightDir, ReturnAlpha);
	}

	return UKismetMathLibrary::VLerp(ForwardDir, LeftDir, ReturnAlpha);
}

FVector AEnemyBrute::GetJumpSlamPosition(FVector DirFromPlayerToSelf, FVector PlayerPos)
{
	return PlayerPos + (DirFromPlayerToSelf * 100);
}


void AEnemyBrute::OnContinueSecondAttackMontage_Implementation()
{
	Super::OnContinueSecondAttackMontage_Implementation();
	
	FVector PlayerPos = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
	FVector CurrentPos = GetActorLocation();
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(CurrentPos, PlayerPos));

	PlaySpecificAttackMovingTimeline(CurrentAttackType);
	
	UpdateAttackingVariables();

	if(CurrentAttackType == EEnemyAttackType::AbleToCounter)
	{
		IsSecondAttack = true;
		
		
		StopAnimMontage();
		PlayAnimMontage(CouterableAttackSecond, 1, "Default");
		PlaySpecificAttackMovingTimeline(CurrentAttackType);
		return;
	}

	OnStopFocusing();
	StopAnimMontage();
	PlayAnimMontage(UnableCounterAttackSecond, 1, "Default");
	SetCapsuleCompCollision(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	

}

// void AEnemyBrute::RecoverFromStunState()
// {
// 	TrySwitchEnemyState(EEnemyCurrentState::WaitToAttack);	
// 	
// 	TryResumeMoving();
// }

TArray<AActor*> AEnemyBrute::GetActorsInFrontOfEnemy(bool IsDamaging)
{
	const UWorld* World = GetWorld();
	
	FName AttackSocketName;
	if(!IsSecondAttack)
		AttackSocketName = "LeftHand";
	else
		AttackSocketName = "RightHand";
	
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

TArray<AActor*> AEnemyBrute::GetActorsAroundEnemy()
{
	const UWorld* World = GetWorld();
	TArray<AActor*> FoundActorList;

	FVector StartLocation = GetActorLocation();
	const FVector DownwardEnd = StartLocation + ((GetActorUpVector() * -1) * 100.0f);

	// Hit result
	FHitResult Hit;
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	
	const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, StartLocation, DownwardEnd, UEngineTypes::ConvertToTraceType(ECC_Visibility),false, IgnoreActors,  EDrawDebugTrace::None,Hit,true);
	if(bHit) StartLocation.Z = Hit.Location.Z;
	
	TArray<AEnemyBase*> Allies = OwnController->GetAllies();
	if(Allies.Num() > 0)
		for (AEnemyBase* EachAlly : Allies)
		{
			IgnoreActors.Add(EachAlly);
		}
	
	UKismetSystemLibrary::SphereOverlapActors(World, StartLocation, AoeDamageRadius, FilterType, FilteringClass, IgnoreActors, FoundActorList);
	
	return FoundActorList;
}

void AEnemyBrute::OnDeath()
{
	Super::OnDeath();
}

void AEnemyBrute::TryStopAttackMovement()
{
}

void AEnemyBrute::UpdateAttackingVariables()
{
	float AttackTravelDistance = 0;
	float TotalTravelTime = 0;
	if(BruteAttack == EBruteAttackType::Swipe)
	{
		AttackTravelDistance = AttackTravelDistance_Swipe;
		TotalTravelTime = ToTalTravelTime_Swipe;
	}
	else if(BruteAttack == EBruteAttackType::Charging)
	{
		AttackTravelDistance = AttackTravelDistance_Charge;
		TotalTravelTime = TotalTravelTime_Charging;
	}
	else
	{
		AttackTravelDistance = AttackTravelDistance_JumpSlam;
		TotalTravelTime = TotalTravelTime_JumpSlam;
		StartJumpingLocation = GetActorLocation();
	}
	
	RemainAttackDistance = AttackTravelDistance;
	MaxTravelDistance = AttackTravelDistance;
	float DeltaSecond = GetWorld()->GetDeltaSeconds();
	float NumOfTickPerSecond = 1 / DeltaSecond;
	TravelDistancePerTick = AttackTravelDistance / (TotalTravelTime * NumOfTickPerSecond);
}

bool AEnemyBrute::ShouldKeepCharging(FVector DirToPlayer)
{
	// if brute charge through player, or finish traveling, end task and timeline
	if(RemainAttackDistance < TravelDistancePerTick * 3)
	{
		return false;
	}

	// if(AttackMovingDir != FVector(0,0,0))
	// {
	// 	const float DotResult = UKismetMathLibrary::Dot_VectorVector(AttackMovingDir, DirToPlayer);
	// 	if(DotResult < 0.2)
	// 	{
	// 		return false;
	// 	}
	// }

	return true;
}

void AEnemyBrute::PlayReceiveCounterAnimation()
{
}

void AEnemyBrute::ChargeKnock(AActor* KnockingActor)
{
	// TODO:Get knocking force with direction

	const FVector CharForwardDir = GetActorForwardVector();
	const FVector CharUpDir = GetActorUpVector();
	
	const float UpForce = UKismetMathLibrary::DegSin(ChargeKnockAngle) * ChargeKnockForce;
	const float ForwardForce = UKismetMathLibrary::DegCos(ChargeKnockAngle) * ChargeKnockForce;

	const FVector ForwardForceVector = CharForwardDir * ForwardForce;
	const FVector UpForceVector = CharUpDir * UpForce;

	const FVector KnockForce = ForwardForceVector + UpForceVector;
	
	ICharacterActionInterface::Execute_OnApplyChargeKnockForce(KnockingActor, KnockForce);
}

void AEnemyBrute::SetCapsuleCompCollision(ECollisionChannel ResponseChannel, ECollisionResponse RequestResponse)
{
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	if(CapsuleComp)
		CapsuleComp->SetCollisionResponseToChannel(ResponseChannel, RequestResponse);
}

void AEnemyBrute::OnDealChargeDamage(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
											UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// check if owner class has player damage interface
	if(OtherActor->GetClass()->ImplementsInterface(UPlayerDamageInterface::StaticClass()))
	{
		
		StopAnimMontage();
		SetCapsuleCompCollision(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
		TryResumeMoving();
		ChargeMovingTimeline.Stop();
		ChargeKnock(OtherActor);
		// if it has character action interface, it means its base character, execute its SwitchToIdleState function
		IPlayerDamageInterface::Execute_ReceiveDamageFromEnemy(OtherActor, BaseDamageAmount, this, CurrentAttackType);
		TryFinishAttackTask(EEnemyCurrentState::WaitToAttack);
	}
}
