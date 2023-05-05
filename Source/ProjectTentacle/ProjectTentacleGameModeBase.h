// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Encounter/EncounterVolume.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectTentacleGameModeBase.generated.h"

/**
 * 
 */


UCLASS()
class PROJECTTENTACLE_API AProjectTentacleGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:
	// FTimerHandle EnemyRepositionTimerHandle;
	
	// Try to grab encounter volume in current map and assign it as our encounter volume reference
	// void TryInitializeEncounterVolumeRef();

	// void SetupRepositionSignature();
	
	// void StartRepositionEnemyLoop();

	void RegisterForCheckpoints();
	FVector ActiveCheckpointLocation;
	UWorld* World;
	
	
protected:

	// encounter volume reference
	UPROPERTY()
	AEncounterVolume* CurrentEncounterVolumeRef;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= EnemySetting_Repositioning)
	float GapTimeToReposition = 5.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category= EnemySetting_Repositioning)
	bool IncludeHeavy = false;


	// UFUNCTION()
	// void StartRepositionEnemies();
	
	
	
public:

	virtual void BeginPlay() override;
	FVector ResetAndGetCheckpointLocation();
	void ResetEncounters();
	void SetActiveCheckpointLocation(const class ACheckpoint* NewActiveCheckpoint);
};
