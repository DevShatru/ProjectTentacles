// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Enemies/UnitPool.h"

#include "Characters/Enemies/EnemyBase.h"
// Sets default values
AUnitPool::AUnitPool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AUnitPool::AddUnitToPool(AEnemyBase* Unit)
{
	if(Unit->GetController())
	{
		Unit->GetController()->Reset();
	}
	Unit->SetActorLocation(GetActorLocation());
	PooledUnits.Add(Unit);
}

AEnemyBase* AUnitPool::GetUnitFromPool(const EEnemyType Type, const FActorSpawnParameters* SpawnParameters)
{
	for(AEnemyBase* Unit: PooledUnits)
	{
		if(Unit->GetType() == Type)
		{
			if(Unit->GetController())
			{
				Unit->GetController()->Reset();
			}
			return Unit;
		}
	}
	TSubclassOf<AEnemyBase> ClassToSpawn;
	// If we don't find one, spawn a new one
	switch (Type)
	{
	case EEnemyType::Melee:
		ClassToSpawn = DefaultMeleeClass;
		break;
	case EEnemyType::Ranged:
		ClassToSpawn = DefaultRangedClass;
		break;
	case EEnemyType::Brute:
		ClassToSpawn = DefaultBruteClass;
		break;
	case EEnemyType::Healer:
		ClassToSpawn = DefaultHealerClass;
		break;
	default:
		return nullptr;
	}
	return World->SpawnActor<AEnemyBase>(ClassToSpawn, *SpawnParameters);
}

// Called when the game starts or when spawned
void AUnitPool::BeginPlay()
{
	Super::BeginPlay();
	PooledUnits = TArray<AEnemyBase*>();
	World = GetWorld();
}

void AUnitPool::BeginDestroy()
{
	PooledUnits.Empty();
	Super::BeginDestroy();
}
