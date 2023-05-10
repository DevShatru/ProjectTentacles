// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EncounterVolumeInterface.h"
#include "GameFramework/Actor.h"
#include "WaveParams.h"
#include "EncounterVolume.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEncounterComplete);
class UNavigationInvokerComponent;
class AEnemyBase;

UCLASS()
class PROJECTTENTACLE_API AEncounterVolume : public AActor, public IEncounterVolumeInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEncounterVolume();

	// Try to start the encounter, if it hasn't already
	void TryTriggerEncounter(AActor* Target);

	// Return list of contained units excluding passed pawn
	TArray<AEnemyBase*> GetAlliesForPawn(APawn* Pawn);

	// Register basic unit (melee or ranged) on attack queue
	void RegisterOnBasicAttackQueue(class AEnemyBaseController* RegisteringController);
	
	// Register when a basic unit has completed it's attack
	void RegisterCompletedBasicAttack(AEnemyBaseController* RegisteringController);
	
	// Fire when a unit is destroyed tp check if we should trigger spawn and update our queues
	void RegisterUnitDestroyed(AEnemyBaseController* Unit, bool bForceDespawn);

	void AddSpawnedUnitToEncounter(AEnemyBase* Unit);

	void RemoveDeadUnitFromEncounter(AEnemyBaseController* DeadUnit);

	// Delegate function to be execute to send all enemy to reposition
	UFUNCTION()
	void SendAllEnemyToReposition(bool DoesIncludeHeavy);

	virtual void AssignQueueEnemyToReposition_Implementation(bool DoesIncludeHeavy) override;

	bool IsComplete() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FEncounterComplete EncounterComplete;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Area around the encounter to generate navigation
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNavigationInvokerComponent* NavInvoker;

	// Root component
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	// Set of all contained units at any time
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat)
	TSet<AEnemyBase*> ContainedUnits;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat)
	float AttackStartDelay = 3.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Spawn)
	class AUnitPool* UnitPool;

	// Wave system setup
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Spawn)
	float SpawnStartTime = 75.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Spawn)
	float SpawnStartEncounterCompletionPercent = 75.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Spawn)
	float DespawnTimer = 5.0f;
	TArray<FWaveParams> WaveParameters;

	// Attack queue for melee and ranged units
	TArray<AEnemyBaseController*> AttackQueueBasic;

	// Attack queue for brutes
	TArray<AEnemyBaseController*> AttackQueueHeavy;

	// Pop random enemy from the queue and command them to attack
	UFUNCTION()
	void BeginAttackBasic();

	UFUNCTION()
	void StartSpawn();

	UFUNCTION()
	void DespawnUnit(AEnemyBaseController* Unit);
	
private:
	// Called on begin play and reset
	void Setup();
	// Register the encounter object with each contained units
	void RegisterEncounterForUnits();
	// Register the encounter object with each contained spawn point
	void RegisterEncounterForSpawnPoints();
	// Engage contained units when encounter starts
	void EngageContainedUnits(AActor* Target);
	// Track whether the encounter has started yet
	unsigned int bIsEncounterActive:1;
	// Track if contained spawn points have begun spawning
	unsigned int bWaveStartedSpawning:1;
	// Track whether the encounter is complete yet
	unsigned int bIsEncounterComplete:1;

	bool AllSpawnsComplete() const;
	
	// Timer handle for basic attack queue
	FTimerHandle BasicQueueTimer;
	// Timer handle to begin span after elapsed time
	FTimerHandle SpawnStartTimer;

	// Cache reference to world timer manager
	static FTimerManager* WorldTimerManager;
	void TryCacheTimerManager() const;
	// Start timer for basic queue
	void StartBasicQueueTimer();
	void TriggerNextWave();
	void ResetSpawnPoints() const;

	int8 InitialUnits, DefeatedUnits, CurrentWave;
	AEnemyBaseController* LastAttacker;
	AActor* EncounterTarget;
	FWaveParams* CurrentWaveParams;
};
