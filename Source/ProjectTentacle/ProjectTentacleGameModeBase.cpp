// Copyright Epic Games, Inc. All Rights Reserved.


#include "ProjectTentacleGameModeBase.h"

#include "EngineUtils.h"
#include "Encounter/Checkpoint.h"

void AProjectTentacleGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	CacheCheckpointRefs();

	// TryInitializeEncounterVolumeRef();

	// StartRepositionEnemyLoop();
}

void AProjectTentacleGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AllCheckpoints.Empty();
	Super::EndPlay(EndPlayReason);
}

void AProjectTentacleGameModeBase::TryInitializeEncounterVolumeRef()
{

	UWorld* World = GetWorld();
	if(!World) return;
	
	AActor* ResultActor = nullptr;

	// Replaced UGameplayStatics import with TActorIterator import (Smaller file, since we're only using one method from it)
	for (TActorIterator<AEncounterVolume> It(World, AEncounterVolume::StaticClass()); It; ++It)
	{
		ResultActor = *It;
	}

	if(!ResultActor) return;

	AEncounterVolume* CastedEnVolume = Cast<AEncounterVolume>(ResultActor);

	if(!CastedEnVolume) return;
	
	CurrentEncounterVolumeRef = CastedEnVolume;
}

void AProjectTentacleGameModeBase::StartRepositionEnemyLoop()
{
	const UWorld* World = GetWorld();
	if(!World) return;


	World->GetTimerManager().SetTimer(EnemyRepositionTimerHandle,this, &AProjectTentacleGameModeBase::StartRepositionEnemies, GapTimeToReposition, true, -1);
}

// Get and save a reference to all ACheckpoint actors in the level
void AProjectTentacleGameModeBase::CacheCheckpointRefs()
{
	AllCheckpoints.Empty();
	UWorld* World = GetWorld();
	if(!World) return;
	
	for (TActorIterator<ACheckpoint> It(World, ACheckpoint::StaticClass()); It; ++It)
	{
		const ACheckpoint* Checkpoint = *It;
		if(Checkpoint) AllCheckpoints.Add(Checkpoint);
	}
}

void AProjectTentacleGameModeBase::StartRepositionEnemies()
{
	if(!CurrentEncounterVolumeRef) return;
	
	// check if owner class has character action interface
	if(CurrentEncounterVolumeRef->GetClass()->ImplementsInterface(UEncounterVolumeInterface::StaticClass()))
	{
		// if it has character action interface, it means its base character, execute its SwitchToIdleState function
		IEncounterVolumeInterface::Execute_AssignQueueEnemyToReposition(CurrentEncounterVolumeRef, IncludeHeavy);
	}
}

