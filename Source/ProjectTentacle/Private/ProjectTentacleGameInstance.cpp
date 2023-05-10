// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "ProjectTentacleGameInstance.h"

#include "EngineUtils.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void UProjectTentacleGameInstance::Init()
{
	Super::Init();
	SaveObject = Cast<UCheckpointSave>(UGameplayStatics::CreateSaveGameObject(SaveObjectClass));
	SaveGame();
}

void UProjectTentacleGameInstance::OnSaveLoad(const FString& SlotName, const int32 SlotID, USaveGame* Save)
{
	if(!Save) return;

	UCheckpointSave* AsCheckpointSave = Cast<UCheckpointSave>(Save);
	if(!AsCheckpointSave) return;

	SaveObject = AsCheckpointSave;

	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName), true);
	
	// Create a level streaming instance for the new level
	//ULevelStreamingDynamic* LevelStreamingInstance = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(GetWorld(), SoftObjectPtrToLevel);

	// Register a delegate to be called when the level is loaded
	//LevelStreamingInstance->OnLevelLoaded.AddDynamic(this, &UMyGameInstance::OnLevelLoaded, NewLocation);
	
	PC->SetActorLocation(AsCheckpointSave->PlayerLocation);
	PC->SetCurrentCharacterHealth(AsCheckpointSave->PlayerHealth);
}

void UProjectTentacleGameInstance::OnLevelLoad(ULevelStreamingDynamic* LoadedLevel, bool bIsSuccess,
	const FString& Error, const FVector& NewLocation)
{
}

void UProjectTentacleGameInstance::TryCachePC()
{
	if(PC) return;
	
	for (TActorIterator<APlayerCharacter> It(GetWorld(), APlayerCharacter::StaticClass()); It; ++It)
	{
		PC = *It;
		if(PC) break;
	}
}

void UProjectTentacleGameInstance::SaveGame()
{
	TryCachePC();
	if(!PC) return;
	SaveObject->PlayerHealth = PC->GetCurrentCharacterHealth();
	SaveObject->PlayerLocation = PC->GetActorLocation();
	UGameplayStatics::AsyncSaveGameToSlot(SaveObject, SaveObject->GetSlotName(), SaveObject->GetSlotIndex());
}

void UProjectTentacleGameInstance::ReloadLastSave()
{
	FAsyncLoadGameFromSlotDelegate LoadDelegate;
	LoadDelegate.BindUFunction(this, FName("OnSaveLoad"));

	UGameplayStatics::AsyncLoadGameFromSlot(SaveObject->GetSlotName(), SaveObject->GetSlotIndex(), LoadDelegate);
}
