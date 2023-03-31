// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Enemies/EncounterVolume.h"

#include "NavigationInvokerComponent.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Characters/Enemies/EnemyBaseController.h"

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

// Called when the game starts or when spawned
void AEncounterVolume::BeginPlay()
{
	Super::BeginPlay();
	WorldTimerManager = &GetWorldTimerManager();
	bIsEncounterActive = false;
	LastAttacker = nullptr;
	
	RegisterEncounterForUnits();
}

// Select random unit to attack
void AEncounterVolume::BeginAttackBasic()
{
	int8 RandomIndex;

	// Don't let same unit attack twice in a row
	do
	{
		RandomIndex = FMath::RandRange(0, AttackQueueBasic.Num() - 1);
	} while(LastAttacker == AttackQueueBasic[RandomIndex]);
	
	AttackQueueBasic[RandomIndex]->BeginAttack();
	AttackQueueBasic.RemoveAt(RandomIndex);
}

// Register this encounter with contained units
void AEncounterVolume::RegisterEncounterForUnits()
{
	for(AEnemyBase* ContainedUnit : ContainedUnits)
	{
		ContainedUnit->RegisterOwningEncounter(this);
	}
}

// Trigger for contained units to engage the target
void AEncounterVolume::EngageContainedUnits(AActor* Target)
{
	for(AEnemyBase* ContainedUnit : ContainedUnits)
	{
		ContainedUnit->EngageTarget(Target);
	}
}

// Start cooldown and pop attacker after timer
void AEncounterVolume::StartBasicQueueTimer()
{
	if(!WorldTimerManager)
	{
		WorldTimerManager = &GetWorldTimerManager();
	}
	WorldTimerManager->SetTimer(BasicQueueTimer, this, &AEncounterVolume::BeginAttackBasic, AttackStartDelay, false, AttackStartDelay);
}
