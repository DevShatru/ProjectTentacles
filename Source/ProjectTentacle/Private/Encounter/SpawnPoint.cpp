// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Encounter/SpawnPoint.h"

#include "Characters/Enemies/EnemyBase.h"
#include "Characters/Enemies/UnitPool.h"
#include "Encounter/EncounterVolume.h"

// Sets default values
ASpawnPoint::ASpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void ASpawnPoint::StartSpawningUnits()
{
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASpawnPoint::SpawnUnit, TimeBetweenSpawns, true, TimeBetweenSpawns);
}

void ASpawnPoint::StopSpawningUnits()
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	ResetSpawnPoint();
}

void ASpawnPoint::SetUnitPool(AUnitPool* NewUnitPool)
{
	UnitPool = NewUnitPool;
}

void ASpawnPoint::SpawnUnit()
{
	// Do nothing if not part of an encounter
	if(!OwningEncounterVolume) return;
	
	// Determine what unit to spawn based on weight
	CheckUnitsToSpawn();
	const float MeleeSpawnWeight = bShouldSpawnMelee ? MeleeUnitsSpawnWeight : 0.f,
				RangedSpawnWeight = bShouldSpawnRanged ? RangedUnitsSpawnWeight : 0.f,
				BruteSpawnWeight = bShouldSpawnBrute ? BruteUnitsSpawnWeight : 0.f;
	const float SpawnWeightSum = MeleeSpawnWeight + RangedSpawnWeight + BruteSpawnWeight;

	if (SpawnWeightSum <= 0.f) {
		StopSpawningUnits();
		return;
	}

	const float WeightedRandom = FMath::FRandRange(0.f, SpawnWeightSum);

	// Determine type of unit to spawn based on which band WeightedRandom is in
	const EEnemyType TypeToSpawn = WeightedRandom <= MeleeSpawnWeight ? EEnemyType::Melee :
								   (WeightedRandom <= MeleeSpawnWeight + RangedSpawnWeight ? EEnemyType::Ranged : EEnemyType::Brute);

	// Get Unit from pool
	// Spawn if pool is unavailable
	AEnemyBase* Unit = UnitPool ? UnitPool->GetUnitFromPool(TypeToSpawn) :
					   GetWorld()->SpawnActor<AEnemyBase>(TypeToSpawn == EEnemyType::Melee ? MeleeUnitClass :
					   (TypeToSpawn == EEnemyType::Ranged ? RangedUnitClass : BruteUnitClass));
	Unit->SetActorLocation(GetActorLocation());
	++UnitsSpawned[TypeToSpawn];
	OwningEncounterVolume->AddSpawnedUnitToEncounter(Unit);
}

void ASpawnPoint::RegisterOwningEncounter(AEncounterVolume* EncounterVolume)
{
	OwningEncounterVolume = EncounterVolume;
}

// Called when the game starts or when spawned
void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	ResetSpawnPoint();
	CheckUnitsToSpawn();
}

void ASpawnPoint::ResetSpawnPoint()
{
	for(const EEnemyType Type : TEnumRange<EEnemyType>())
	{
		UnitsSpawned.Add(Type, 0);
	}
}

void ASpawnPoint::CheckUnitsToSpawn()
{
	bShouldSpawnMelee = UnitsSpawned[EEnemyType::Melee] < NumMeleeUnitsSpawned;
	bShouldSpawnRanged = UnitsSpawned[EEnemyType::Ranged] < NumRangedUnitsSpawned;
	bShouldSpawnBrute = UnitsSpawned[EEnemyType::Brute] < NumBruteUnitsSpawned;
}

