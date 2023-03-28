// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Enemies/EncounterVolume.h"

#include "NavigationInvokerComponent.h"
#include "Characters/Enemies/EnemyBase.h"

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

// Called when the game starts or when spawned
void AEncounterVolume::BeginPlay()
{
	Super::BeginPlay();
	bIsEncounterActive = false;
	RegisterEncounterForUnits();
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
