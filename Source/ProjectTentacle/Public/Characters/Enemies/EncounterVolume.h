// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EncounterVolumeInterface.h"
#include "GameFramework/Actor.h"
#include "EncounterVolume.generated.h"

class UNavigationInvokerComponent;
class AEnemyBase;

UCLASS()
class PROJECTTENTACLE_API AEncounterVolume : public AActor, public IEncounterVolumeInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEncounterVolume();

	// Try to start the encounter, if it hasn't already
	void TryTriggerEncounter(AActor* Target);

	// Return list of contained units excluding passed pawn
	TArray<AEnemyBase*> GetAlliesForPawn(APawn* Pawn);

	// Register basic unit (melee or ranged) on attack queue
	void RegisterOnBasicAttackQueue(class AEnemyBaseController* RegisteringController);
	
	// Register when a basic unit has completed it's attack
	void RegisterCompletedBasicAttack(AEnemyBaseController* RegisteringController);
	
	// Delegate function to be execute to send all enemy to reposition
	UFUNCTION()
	void SendAllEnemyToReposition(bool DoesIncludeHeavy);

	virtual void AssignQueueEnemyToReposition_Implementation(bool DoesIncludeHeavy) override;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Area around the encounter to generate navigation
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UNavigationInvokerComponent* NavInvoker;

	// Root component
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	// Set of all contained units at any time
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSet<AEnemyBase*> ContainedUnits;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat)
	float AttackStartDelay = 3.0f;

	// Attack queue for melee and ranged units
	TArray<AEnemyBaseController*> AttackQueueBasic;

	// Attack queue for brutes
	TArray<AEnemyBaseController*> AttackQueueHeavy;

	// Pop random enemy from the queue and command them to attack
	UFUNCTION()
	void BeginAttackBasic();

	
private:
	// Register the encounter object with each contained units
	void RegisterEncounterForUnits();
	// Engage contained units when encounter starts
	void EngageContainedUnits(AActor* Target);
	// Track whether the encounter has started yet
	unsigned int bIsEncounterActive:1;
	// Timer handle for basic attack queue
	FTimerHandle BasicQueueTimer;

	// Cache reference to world timer manager
	static FTimerManager* WorldTimerManager;
	// Start timer for basic queue
	void StartBasicQueueTimer();

	AEnemyBaseController* LastAttacker;
};
