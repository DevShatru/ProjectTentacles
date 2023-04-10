// Copyright Epic Games, Inc. All Rights Reserved.


#include "ProjectTentacleGameModeBase.h"

#include "Kismet/GameplayStatics.h"

void AProjectTentacleGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	TryInitializeEncounterVolumeRef();

	StartRepositionEnemyLoop();
}

void AProjectTentacleGameModeBase::TryInitializeEncounterVolumeRef()
{

	const UWorld* World = GetWorld();
	if(!World) return;
	
	AActor* ResultActor = UGameplayStatics::GetActorOfClass(World, AEncounterVolume::StaticClass());

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

