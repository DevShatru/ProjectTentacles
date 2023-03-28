// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyBaseController.generated.h"

class UAISenseConfig_Sight;
/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API AEnemyBaseController : public AAIController
{
	GENERATED_BODY()
	
public:
	AEnemyBaseController();
	virtual void BeginPlay() override;

	// Register encounters and targets
	void RegisterOwningEncounter(class AEncounterVolume* NewOwningEncounter);
	void EngageTarget(AActor* Target);

protected:
	// Base behavior tree, run on start
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBehaviorTree* BehaviorTree;
	
	// Perception component and sense config
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAIPerceptionComponent* Perception;
	UPROPERTY(BlueprintReadOnly)
	UAISenseConfig_Sight* Sight;

	// Bound to perception updated delegate
	UFUNCTION()
	void UpdatePerception(AActor* Actor, FAIStimulus Stimulus);
	
private:
	class AEncounterVolume* OwningEncounter;
	AActor* EncounterTarget;
};
