// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Enemies/EnemyRanged.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


void AEnemyRanged::ExecuteAttack()
{
	Super::ExecuteAttack();

	PlayAnimMontage(KneelDownToAimAnim, 1, "Default");
}

void AEnemyRanged::BeginFire()
{
	// Hide the Attack indicator
	OnHideAttackIndicator();
	
	// Start fire animation
	StopAnimMontage();
	PlayAnimMontage(KneelFireAnim,1, "Default");
}

AActor* AEnemyRanged::GetDamageActorByLineTrace()
{
	// Hit result
	FHitResult Hit;
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	
	// Capsule trace by channel
	const FVector CurrentPos = GetActorLocation();
	const FVector FacingDir = GetActorForwardVector();
	const FVector AimingDestination = CurrentPos + (FacingDir * AimingRange);
	
	const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, CurrentPos, AimingDestination, UEngineTypes::ConvertToTraceType(ECC_Camera),false, IgnoreActors,  EDrawDebugTrace::None,Hit,true);

	if(bHit) return Hit.GetActor();

	return nullptr;
}

void AEnemyRanged::StopAimingTimer()
{
	const UWorld* World = GetWorld();
	if(!World) return;

	World->GetTimerManager().ClearTimer(AimingTimerHandle);
}

void AEnemyRanged::ShowOrHidePlayerHUD(bool Show)
{
	ACharacter* PlayerCha = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if(!PlayerCha) return;

	if(PlayerCha->GetClass()->ImplementsInterface(UCharacterActionInterface::StaticClass()))
		if(Show)
			ICharacterActionInterface::Execute_OnShowPlayerIndicatorHUD(PlayerCha);
		else
			ICharacterActionInterface::Execute_OnHidePlayerIndicatorHUD(PlayerCha);
}

void AEnemyRanged::OnRifleBeginAiming_Implementation()
{
	IEnemyRangeInterface::OnRifleBeginAiming_Implementation();

	// Set timer to shoot
	const UWorld* World = GetWorld();
	if(!World) return;

	World->GetTimerManager().SetTimer(AimingTimerHandle, this, &AEnemyRanged::BeginFire, AimTimeToShoot, false, -1);

	// Start aiming loop animation
	StopAnimMontage();
	PlayAnimMontage(KneelAimingLoopAnim,1, "Default");

	// Show attack indicator
	if(AttackIndicatorRef)
		AttackIndicatorRef->ShowIndicator();

	// Show Player attack indicator HUD
	ShowOrHidePlayerHUD(true);
	
}

void AEnemyRanged::OnRifleFinishFiring_Implementation()
{
	IEnemyRangeInterface::OnRifleFinishFiring_Implementation();

	// Start aiming loop animation
	StopAnimMontage();
	PlayAnimMontage(StandUpAnim,1, "Default");
}

void AEnemyRanged::TryToDamagePlayer_Implementation()
{
	Super::TryToDamagePlayer_Implementation();

	ShowOrHidePlayerHUD(false);

	AActor* SupposeDamageActor = GetDamageActorByLineTrace();

	if(!SupposeDamageActor) return;

	
	if(SupposeDamageActor->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()))
		IDamageInterface::Execute_ReceiveDamageFromEnemy(SupposeDamageActor, BaseDamageAmount, this, EEnemyAttackType::UnableToCounter);
}

void AEnemyRanged::ReceiveDamageFromPlayer_Implementation(int32 DamageAmount, AActor* DamageCauser,
	EPlayerAttackType PlayerAttackType)
{
	Super::ReceiveDamageFromPlayer_Implementation(DamageAmount, DamageCauser, PlayerAttackType);

	bool StateChanged = false;
	
	// if enemy is attack, stop montage, cancel fire timer, unshow attack indicator, and execute onfinish attack delegate
	if(CurrentEnemyState == EEnemyCurrentState::Attacking)
	{
		// Stop current montage
		StopAnimMontage();

		// Stop timer
		StopAimingTimer();
		
		// Stop Both attack indicators
		OnHideAttackIndicator();
		ShowOrHidePlayerHUD(false);
		
		// Finish task
		TryFinishAttackTask(EEnemyCurrentState::Damaged);

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

