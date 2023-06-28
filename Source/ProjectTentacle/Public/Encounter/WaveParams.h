#pragma once

#include "CoreMinimal.h"
#include "WaveParams.generated.h"

USTRUCT(BlueprintType)
struct PROJECTTENTACLE_API FWaveParams
{
	GENERATED_BODY()
	
	// Set of all contained spawn points
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSet<class ASpawnPoint*> ContainedSpawnPoints;
	// Time before the next wave starts spawning
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnStartTime = 75.0f;
	// Percentage of enemies defeated to start spawning the next wave early (Only checks if SpawnStartTime has not already passed)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnStartEncounterCompletionPercent = 75.0f;
};
