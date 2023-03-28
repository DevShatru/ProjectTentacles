// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EncounterVolume.generated.h"

class UNavigationInvokerComponent;
class AEnemyBase;

UCLASS()
class PROJECTTENTACLE_API AEncounterVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEncounterVolume();

	// Try to start the encounter, if it hasn't already
	void TryTriggerEncounter(AActor* Target);

	TArray<AEnemyBase*> GetAlliesForPawn(APawn* Pawn);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Area around the encounter to generate navigation
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNavigationInvokerComponent* NavInvoker;

	// Root component
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	// Set of all contained units at any time
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSet<AEnemyBase*> ContainedUnits;

	// Attack queue for melee and ranged units
	TQueue<AEnemyBase*> AttackQueueBasic;

	// Attack queue for brutes
	TQueue<AEnemyBase*> AttackQueueHeavy;

private:
	// Register the encounter object with each contained units
	void RegisterEncounterForUnits();
	// Engage contained units when encounter starts
	void EngageContainedUnits(AActor* Target);
	// Track whether the encounter has started yet
	unsigned int bIsEncounterActive:1;
};
