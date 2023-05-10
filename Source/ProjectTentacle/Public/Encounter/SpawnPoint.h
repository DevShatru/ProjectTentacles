// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Enemies/EnemyType.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

UCLASS()
class PROJECTTENTACLE_API ASpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnPoint();
	void StartSpawningUnits();
	void StopSpawningUnits();
	void SetUnitPool(class AUnitPool* NewUnitPool);
	void SpawnUnit();
	void RegisterOwningEncounter(class AEncounterVolume* EncounterVolume);
	bool IsSpawningComplete() const;

	virtual void Reset() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Spawn Parameters")
	float TimeBetweenSpawns = 5.0f;

	// Defaults class and number of units spawned for each type
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=MeleeSpawn)
	int32 NumMeleeUnitsSpawned = 5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=MeleeSpawn)
	TSubclassOf<class AEnemyBase> MeleeUnitClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=MeleeSpawn)
	float MeleeUnitsSpawnWeight = 5.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=HealerSpawn)
	int32 NumHealerUnitsSpawned = 5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=HealerSpawn)
	TSubclassOf<AEnemyBase> HealerUnitClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=HealerSpawn)
	float HealerUnitsSpawnWeight = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=RangedSpawn)
	int32 NumRangedUnitsSpawned = 5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=RangedSpawn)
	TSubclassOf<AEnemyBase> RangedUnitClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=RangedSpawn)
	float RangedUnitsSpawnWeight = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BruteSpawn)
	int32 NumBruteUnitsSpawned = 5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=BruteSpawn)
	TSubclassOf<AEnemyBase> BruteUnitClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=BruteSpawn)
	float BruteUnitsSpawnWeight = 5.0f;

	UPROPERTY()
	AUnitPool* UnitPool;
	
private:
	void Setup();
	void ResetSpawnPoint();
	AEncounterVolume* OwningEncounterVolume;
	FTimerHandle SpawnTimerHandle;
	TMap<EEnemyType, int8> UnitsSpawned;
	unsigned int bShouldSpawnMelee:1, bShouldSpawnHealer:1, bShouldSpawnRanged:1, bShouldSpawnBrute:1, bSpawnComplete:1;
	void CheckUnitsToSpawn();
	TSet<AEnemyBase*> SpawnedUnits;
};
