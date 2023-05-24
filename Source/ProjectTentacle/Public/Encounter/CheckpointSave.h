// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CheckpointSave.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UCheckpointSave : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FVector PlayerLocation;
	
	UPROPERTY()
	FRotator PlayerRotation;
	
	UPROPERTY()
	float PlayerHealth;

	UPROPERTY()
	TSet<FString> CompletedEncounters;
	
	FString GetSlotName();
	int32 GetSlotIndex() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString SlotName = "Checkpoint Save";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 SlotIndex = 0;
};
