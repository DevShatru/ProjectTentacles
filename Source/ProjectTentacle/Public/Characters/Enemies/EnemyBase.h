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

	UPROPERTY(EditDefaultsOnly, Category=Combat)
	float AttackCompletionTime = 2.0f;
	
	UPROPERTY(EditDefaultsOnly, Category=Combat)
	float AttackCounterableTime = 1.4f;
	

public:	
	// Sets default values for this character's properties
	AEnemyBase();

	// Register encounters and targets with owning controller
	void RegisterOwningEncounter(class AEncounterVolume* NewOwningEncounter);
	void EngageTarget(AActor* Target);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float GetAttackCompletionTime() const;
	float GetAttackCounterableTime() const;

private:
	class AEnemyBaseController* OwnController;
	void TryGetOwnController();
};
