// Copyright Epic Games, Inc. All Rights Reserved.


#include "ProjectTentacleGameModeBase.h"

#include "EngineUtils.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Encounter/EncounterVolume.h"

void AProjectTentacleGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	World = GetWorld();
	if(!World) return;

	CacheEncounterReferences();

	for (TActorIterator<APlayerCharacter> It(World, APlayerCharacter::StaticClass()); It; ++It)
	{
		const APlayerCharacter* PC = *It;
		if(PC)
		{
			ActiveCheckpointLocation = PC->GetActorLocation();
			break;
		}
	}
	// TryInitializeEncounterVolumeRef();

	// StartRepositionEnemyLoop();
}

void AProjectTentacleGameModeBase::CacheEncounterReferences()
{
	if(!World) return;
	
	for (TActorIterator<AEncounterVolume> It(World, AEncounterVolume::StaticClass()); It; ++It)
	{
		AEncounterVolume* Encounter = *It;
		if(Encounter) AllEncounters.Add(Encounter);
	}
}

/*
void AProjectTentacleGameModeBase::TryInitializeEncounterVolumeRef()
{
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
*/

/*
void AProjectTentacleGameModeBase::StartRepositionEnemyLoop()
{
	const UWorld* World = GetWorld();
	if(!World) return;


	World->GetTimerManager().SetTimer(EnemyRepositionTimerHandle,this, &AProjectTentacleGameModeBase::StartRepositionEnemies, GapTimeToReposition, true, -1);
}
*/

/*
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
*/

