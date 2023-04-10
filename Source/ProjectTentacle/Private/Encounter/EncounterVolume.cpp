// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Encounter/EncounterVolume.h"

#include "NavigationInvokerComponent.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Characters/Enemies/EnemyBaseController.h"
#include "Encounter/SpawnPoint.h"

FTimerManager* AEncounterVolume::WorldTimerManager = nullptr;
// Sets default values
AEncounterVolume::AEncounterVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("Nav Invoker"));
	NavInvoker->SetGenerationRadii(1000.f, 1000.f);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Encounter Root"));
	Root->SetMobility(EComponentMobility::Static);
	SetRootComponent(Root);
}


void AEncounterVolume::TryTriggerEncounter(AActor* Target)
{
	// Early return if encounter has already begun
	if(bIsEncounterActive) return;
	bIsEncounterActive = true;
	EngageContainedUnits(Target);
	TryCacheTimerManager();
	WorldTimerManager->SetTimer(SpawnStartTimer, this, &AEncounterVolume::StartSpawn, SpawnStartTime, false, SpawnStartTime);
}

TArray<AEnemyBase*> AEncounterVolume::GetAlliesForPawn(APawn* Pawn)
{
	TArray<AEnemyBase*> AlliesForPawn;
	for(AEnemyBase* ContainedUnit : ContainedUnits)
	{
		if(ContainedUnit == Pawn) continue;
		AlliesForPawn.Add(ContainedUnit);
	}

	return AlliesForPawn;
}

void AEncounterVolume::RegisterOnBasicAttackQueue(AEnemyBaseController* RegisteringController)
{
	if(AttackQueueBasic.Num() == 0)
	{
		// Start timer to handle the queue
		StartBasicQueueTimer();
	}

	AttackQueueBasic.Add(RegisteringController);
}

void AEncounterVolume::RegisterCompletedBasicAttack(AEnemyBaseController* RegisteringController)
{
	LastAttacker = RegisteringController;
	StartBasicQueueTimer();
}

void AEncounterVolume::RegisterUnitDestroyed(AEnemyBaseController* Unit)
{
	// Remove units from queues and set
	if(AttackQueueBasic.Contains(Unit)) AttackQueueBasic.Remove(Unit);
	if(AttackQueueHeavy.Contains(Unit)) AttackQueueHeavy.Remove(Unit);
	if(ContainedUnits.Contains(Unit->GetOwnPawn())) ContainedUnits.Remove(Unit->GetOwnPawn());
	
	// Check if spawn has started yet
	if(bIsSpawnStarted) return;
	// If not
	++DefeatedUnits;
	// Check if should start
	const float CompletionPercentage = InitialUnits == 0.f ? InitialUnits : static_cast<float>(DefeatedUnits / InitialUnits);
	if(CompletionPercentage > SpawnStartEncounterCompletionPercent / 100.f)
	{
		TryCacheTimerManager();
		WorldTimerManager->ClearTimer(SpawnStartTimer);
		StartSpawn();
	}
}

void AEncounterVolume::AddSpawnedUnitToEncounter(AEnemyBase* Unit)
{
	ContainedUnits.Add(Unit);
	Unit->RegisterOwningEncounter(this);
	if(bIsEncounterActive)
	{
		Unit->EngageTarget(EncounterTarget);
	}
}

// Called when the game starts or when spawned
void AEncounterVolume::BeginPlay()
{
	Super::BeginPlay();
	WorldTimerManager = &GetWorldTimerManager();
	bIsEncounterActive = false;
	bIsSpawnStarted = false;
	LastAttacker = nullptr;
	EncounterTarget = nullptr;
	InitialUnits = ContainedUnits.Num();
	RegisterEncounterForUnits();
	RegisterEncounterForSpawnPoints();
}

// Select random unit to attack
void AEncounterVolume::BeginAttackBasic()
{
	int8 QueueSize = AttackQueueBasic.Num();
	if (QueueSize == 0) return;

	int8 RandomIndex;

	// Don't let same unit attack twice in a row
	do
	{
		RandomIndex = FMath::RandRange(0, QueueSize - 1);
	} while(LastAttacker == AttackQueueBasic[RandomIndex] && QueueSize > 1);
	
	AttackQueueBasic[RandomIndex]->BeginAttack();
	AttackQueueBasic.RemoveAt(RandomIndex);
}

void AEncounterVolume::StartSpawn()
{
	if(bIsSpawnStarted) return;
	bIsSpawnStarted = true;
	for(ASpawnPoint* SpawnPoint : ContainedSpawnPoints)
	{
		SpawnPoint->SetUnitPool(UnitPool);
		SpawnPoint->StartSpawningUnits();
	}
}

// Register this encounter with contained units
void AEncounterVolume::RegisterEncounterForUnits()
{
	for(AEnemyBase* ContainedUnit : ContainedUnits)
	{
		ContainedUnit->RegisterOwningEncounter(this);
	}
}

void AEncounterVolume::RegisterEncounterForSpawnPoints()
{
	for(ASpawnPoint* SpawnPoint : ContainedSpawnPoints)
	{
		SpawnPoint->RegisterOwningEncounter(this);
	}
}

// Trigger for contained units to engage the target
void AEncounterVolume::EngageContainedUnits(AActor* Target)
{
	EncounterTarget = Target;
	for(AEnemyBase* ContainedUnit : ContainedUnits)
	{
		ContainedUnit->EngageTarget(Target);
	}
}

void AEncounterVolume::TryCacheTimerManager() const
{
	if(WorldTimerManager) return;
	
	WorldTimerManager = &GetWorldTimerManager();
}

// Start cooldown and pop attacker after timer
void AEncounterVolume::StartBasicQueueTimer()
{
	TryCacheTimerManager();
	
	WorldTimerManager->SetTimer(BasicQueueTimer, this, &AEncounterVolume::BeginAttackBasic, AttackStartDelay, false, AttackStartDelay);
}
