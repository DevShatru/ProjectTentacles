// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

UCLASS()
class PROJECTTENTACLE_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this character's properties
	AEnemyBase();

	// Register encounters and targets with owning controller
	void RegisterOwningEncounter(class AEncounterVolume* NewOwningEncounter);
	void EngageTarget(AActor* Target);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	class AEnemyBaseController* OwnController;
	void TryGetOwnController();
};
