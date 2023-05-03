// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTreeTypes.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyBaseController.generated.h"

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

	// Get list of allies for Encounter
	TArray<class AEnemyBase*> GetAllies() const;

	virtual void RegisterOnAttackQueue();

	virtual void BeginAttack();

	void QuitFromEncounter();

	void RegisterCompletedAttack();

	AEnemyBase* GetOwnPawn() const;

	UBehaviorTree* GetBehaviorTree() const {return BehaviorTree;}

	virtual void Reset() override;

	void OnDeath();

	void HealEncounterTarget(float HealAmount);
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	// Base behavior tree, run on start
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=AI)
	UBehaviorTree* BehaviorTree;
	
	// Perception component and sense config
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Perception)
	UAIPerceptionComponent* Perception;
	UPROPERTY(BlueprintReadOnly)
	class UAISenseConfig_Sight* Sight;

	// Bound to perception updated delegate
	UFUNCTION()
	void UpdatePerception(AActor* Actor, FAIStimulus Stimulus);	
	
private:
	void ClearBlackboard();
	void TryCacheBlackboardComp();

	UBlackboardComponent* Blackboard;
	AEnemyBase* OwnPawn;
	AEncounterVolume* OwningEncounter;
	AActor* EncounterTarget;
};