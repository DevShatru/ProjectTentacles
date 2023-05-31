// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Player/AttackingTentacle.h"

#include "Characters/Base/DamageInterface.h"
#include "Kismet/KismetMathLibrary.h"

void AAttackingTentacle::StopAttacking()
{
	// Set timer to attack enemy
	WorldRef->GetTimerManager().ClearTimer(AttackEnemyTimerHandle);
}

AActor* AAttackingTentacle::FindClosestEnemyInRange(TArray<AActor*> EnemiesInRange)
{
	const FVector TentaclePos = GetActorLocation();
	
	AActor* ClosestEnemy = nullptr;
	float ClosestEnemyRange = 250.0f;
	
	for (AActor* IterateEnemy : EnemiesInRange)
	{
		FVector CurrentEnemyPos = IterateEnemy->GetActorLocation();
		CurrentEnemyPos.Z = TentaclePos.Z;
		const float CurrentEnemyDistance = UKismetMathLibrary::Vector_Distance(TentaclePos, CurrentEnemyPos);

		// Overwrite closest enemy if range is smaller
		if(CurrentEnemyDistance < ClosestEnemyRange)
		{
			ClosestEnemy = IterateEnemy;
			ClosestEnemyRange = CurrentEnemyDistance;
		}
	}

	return ClosestEnemy;
}

void AAttackingTentacle::DamageInRange(FVector ClosestEnemyPos)
{
	TArray<AActor*> EnemiesInRange = GetEnemyInRadius(AttackingRadius, ClosestEnemyPos);

	if(EnemiesInRange.Num() < 1) return;

	for(AActor* IterateEnemy : EnemiesInRange)
	{
		if(IterateEnemy->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()))
		{
			// Try Stop Movement and pull enemy over here
			IDamageInterface::Execute_ReceiveDamageFromPlayer(IterateEnemy, DamageAmount, this, EPlayerAttackType::LongMeleeAttack);
		}
	}
	
}

void AAttackingTentacle::AttackEnemy()
{
	const FVector CurrentTentaclePos = GetActorLocation();

	const TArray<AActor*> FoundEnemies = GetEnemyInRadius(SearchingRadius, CurrentTentaclePos);

	if(FoundEnemies.Num() < 1) return;

	const AActor* ClosestEnemy = FindClosestEnemyInRange(FoundEnemies);

	if(!ClosestEnemy) return;

	FVector ClosestEnemyPos = ClosestEnemy->GetActorLocation();
	ClosestEnemyPos.Z = CurrentTentaclePos.Z;
	
	DrawDebugAttackRange(ClosestEnemyPos, AttackingRadius);
	
	DamageInRange(ClosestEnemyPos);
}

void AAttackingTentacle::LifeCycleBegin()
{
	Super::LifeCycleBegin();

	// Set timer to attack enemy
	WorldRef->GetTimerManager().SetTimer(AttackEnemyTimerHandle, this, &AAttackingTentacle::AttackEnemy, AttackingGap, true, -1);
}

void AAttackingTentacle::LifeSpanExpired()
{
	Super::LifeSpanExpired();

	StopAttacking();

}
