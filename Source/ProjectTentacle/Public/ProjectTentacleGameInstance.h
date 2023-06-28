// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Encounter/CheckpointSave.h"
#include "Engine/GameInstance.h"
#include "ProjectTentacleGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UProjectTentacleGameInstance : public UGameInstance
{
	GENERATED_BODY()

public: 
	// Write data into SaveObject and save to file
	UFUNCTION(BlueprintCallable)
	void SaveGame();
	UFUNCTION(BlueprintCallable)
	void ReloadLastSave();
	void RegisterEncounterVolume(class AEncounterVolume* Volume);
	bool ShouldSaveAtPCSpawn() const;

protected:
	UFUNCTION(BlueprintCallable)
	void KillActiveUnits();
	
	virtual void Init() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UCheckpointSave> SaveObjectClass = UCheckpointSave::StaticClass();

	UPROPERTY()
	UCheckpointSave* SaveObject;

	// Sets up environment after save object gets loaded into memory
	UFUNCTION()
	void OnSaveLoad(const FString& SlotName, const int32 SlotID, USaveGame* Save);
	
	// Set up world from save data
	UFUNCTION()
	void OnLevelLoad();
	
	// Checks if the map has successfully reloaded, loops on a timer if not
	UFUNCTION()
	void WaitForLevelLoad();
	
private:
	class APlayerCharacter* PC;
	unsigned int bCompletedFirstSave:1;
	void TryCachePC(bool bForce = false);
	TSet<AEncounterVolume*> AllEncounterVolumes;
};
