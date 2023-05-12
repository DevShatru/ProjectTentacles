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

	ShackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shack Mesh"));
	SpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Location"));

	SetRootComponent(ShackMesh);
	SpawnLocation->SetupAttachment(RootComponent);
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
	const float MeleeSpawnWeight = bShouldSpawnMelee ? MeleeUnitsSpawnWeight : 0.f,
				RangedSpawnWeight = bShouldSpawnRanged ? RangedUnitsSpawnWeight : 0.f,
				BruteSpawnWeight = bShouldSpawnBrute ? BruteUnitsSpawnWeight : 0.f,
				HealerSpawnWeight = bShouldSpawnHealer ? HealerUnitsSpawnWeight : 0.f;
	const float SpawnWeightSum = MeleeSpawnWeight + RangedSpawnWeight + BruteSpawnWeight + HealerSpawnWeight;

	if (SpawnWeightSum <= 0.f) {
		StopSpawningUnits();
		return;
	}

	const float WeightedRandom = FMath::FRandRange(0.f, SpawnWeightSum);

	// Determine type of unit to spawn based on which band WeightedRandom is in
	const EEnemyType TypeToSpawn = WeightedRandom <= MeleeSpawnWeight ? EEnemyType::Melee :
								   WeightedRandom <= MeleeSpawnWeight + RangedSpawnWeight ? EEnemyType::Ranged :
								   WeightedRandom <= MeleeSpawnWeight + RangedSpawnWeight + BruteSpawnWeight ? EEnemyType::Brute : EEnemyType::Healer;

	// Get Unit from pool
	// Spawn if pool is unavailable
	AEnemyBase* Unit = UnitPool ? UnitPool->GetUnitFromPool(TypeToSpawn) :
					   GetWorld()->SpawnActor<AEnemyBase>(TypeToSpawn == EEnemyType::Melee ? MeleeUnitClass :
														  TypeToSpawn == EEnemyType::Ranged ? RangedUnitClass :
														  TypeToSpawn == EEnemyType::Brute ? BruteUnitClass : HealerUnitClass);
	SpawnedUnits.Add(Unit);
	Unit->SetActorLocation(SpawnLocation->GetComponentLocation());
	++UnitsSpawned[TypeToSpawn];
	OwningEncounterVolume->AddSpawnedUnitToEncounter(Unit);
	CheckUnitsToSpawn();
}

void ASpawnPoint::RegisterOwningEncounter(AEncounterVolume* EncounterVolume)
{
	OwningEncounterVolume = EncounterVolume;
}

bool ASpawnPoint::IsSpawningComplete() const
{
	return bSpawnComplete;
}

// Called when the game starts or when spawned
void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	Setup();
}

void ASpawnPoint::Setup()
{
	SpawnedUnits.Empty();
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
	bShouldSpawnHealer = UnitsSpawned[EEnemyType::Healer] < NumHealerUnitsSpawned;
	bSpawnComplete = !(bShouldSpawnMelee || bShouldSpawnBrute || bShouldSpawnRanged);
}

