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

AEnemyBase* AUnitPool::GetUnitFromPool(EEnemyType Type)
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

	// If we don't find one, spawn a new one
	switch (Type)
	{
	case EEnemyType::Melee:
			return World->SpawnActor<AEnemyBase>(DefaultMeleeClass);
		case EEnemyType::Ranged:
			return World->SpawnActor<AEnemyBase>(DefaultRangedClass);
		case EEnemyType::Brute:
			return World->SpawnActor<AEnemyBase>(DefaultBruteClass);
		case EEnemyType::Healer:
			return World->SpawnActor<AEnemyBase>(DefaultHealerClass);
		default:
			return nullptr;
	}
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
