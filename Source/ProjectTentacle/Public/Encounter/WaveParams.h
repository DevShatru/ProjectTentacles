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
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnStartTime = 75.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnStartEncounterCompletionPercent = 75.0f;
};
