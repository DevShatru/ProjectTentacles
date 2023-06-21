// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "ProjectTentacleGameInstance.h"

#include "EngineUtils.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Encounter/EncounterVolume.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"

void UProjectTentacleGameInstance::Init()
{
	Super::Init();
	bCompletedFirstSave = false;
	SaveObject = Cast<UCheckpointSave>(UGameplayStatics::CreateSaveGameObject(SaveObjectClass));
}

void UProjectTentacleGameInstance::OnSaveLoad(const FString& SlotName, const int32 SlotID, USaveGame* Save)
{
	if(!Save) return;

	UCheckpointSave* AsCheckpointSave = Cast<UCheckpointSave>(Save);
	if(!AsCheckpointSave) return;

	SaveObject = AsCheckpointSave;

	AllEncounterVolumes.Empty();
	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevelName));

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UProjectTentacleGameInstance::WaitForLevelLoad);
}

void UProjectTentacleGameInstance::OnLevelLoad()
{
	TryCachePC();
	PC->SetActorLocation(SaveObject->PlayerLocation);
	PC->SetActorRotation(SaveObject->PlayerRotation);
	PC->SetCurrentCharacterHealth(SaveObject->PlayerHealth);
	for(AEncounterVolume* Volume: AllEncounterVolumes)
	{
		if(SaveObject->CompletedEncounters.Contains(Volume->GetName())) Volume->MarkComplete();
	}
}

void UProjectTentacleGameInstance::WaitForLevelLoad()
{
	APlayerCharacter* InitialRef = PC;
	TryCachePC(true);
	if(PC == InitialRef)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UProjectTentacleGameInstance::WaitForLevelLoad);
		return;
	}
	OnLevelLoad();
}

void UProjectTentacleGameInstance::TryCachePC(bool bForce)
{
	if(PC && !bForce) return;
	
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
	SaveObject->PlayerRotation = PC->GetActorRotation();
	
	for(const AEncounterVolume* Volume: AllEncounterVolumes)
	{
		if(Volume->IsComplete()) SaveObject->CompletedEncounters.Add(Volume->GetName());	
	}
	
	UGameplayStatics::AsyncSaveGameToSlot(SaveObject, SaveObject->GetSlotName(), SaveObject->GetSlotIndex());
	if(!bCompletedFirstSave) bCompletedFirstSave = true;
}

void UProjectTentacleGameInstance::RegisterEncounterVolume(AEncounterVolume* Volume)
{
	AllEncounterVolumes.Add(Volume);
}

bool UProjectTentacleGameInstance::ShouldSaveAtPCSpawn() const
{
	return !bCompletedFirstSave;
}

void UProjectTentacleGameInstance::KillActiveUnits()
{
	for(AEncounterVolume* Volume: AllEncounterVolumes)
	{
		if(Volume->IsActive()) Volume->KillUnits();
	}
}

void UProjectTentacleGameInstance::ReloadLastSave()
{
	FAsyncLoadGameFromSlotDelegate LoadDelegate;
	LoadDelegate.BindUFunction(this, FName("OnSaveLoad"));

	UGameplayStatics::AsyncLoadGameFromSlot(SaveObject->GetSlotName(), SaveObject->GetSlotIndex(), LoadDelegate);
}
