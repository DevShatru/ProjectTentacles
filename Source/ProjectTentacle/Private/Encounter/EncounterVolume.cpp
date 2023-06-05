// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Encounter/EncounterVolume.h"

#include "NavigationInvokerComponent.h"
#include "ProjectTentacleGameInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Characters/Enemies/EnemyBaseController.h"
#include "Characters/Enemies/UnitPool.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Encounter/SpawnPoint.h"

FTimerManager* AEncounterVolume::WorldTimerManager = nullptr;
// Sets default values
AEncounterVolume::AEncounterVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("Nav Invoker"));
	NavInvoker->SetGenerationRadii(1000.f, 1000.f);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Encounter Root"));
	Root->SetMobility(EComponentMobility::Static);
	SetRootComponent(Root);
}


void AEncounterVolume::TryTriggerEncounter(AActor* Target)
{
	// Early return if encounter has already begun
	if(bIsEncounterActive || bIsEncounterComplete) return;
	bIsEncounterActive = true;
	EngageContainedUnits(Target);
	TriggerNextWave();
}

TArray<AEnemyBase*> AEncounterVolume::GetAlliesForPawn(APawn* Pawn)
{
	TArray<AEnemyBase*> AlliesForPawn;
	for(AEnemyBase* ContainedUnit : ContainedUnits)
	{
		if(ContainedUnit == Pawn) continue;
		AlliesForPawn.Add(ContainedUnit);
	}

	return AlliesForPawn;
}

void AEncounterVolume::RegisterOnAttackQueue(AEnemyBaseController* RegisteringController)
{
	TArray<AEnemyBaseController*>* AttackQueue = GetAttackQueue(RegisteringController->IsBasic());
	
	if(AttackQueue->Num() == 0)
	{
		// Start timer to handle the queue
		StartQueueTimer(RegisteringController->IsBasic());
	}

	AttackQueue->Add(RegisteringController);
}

void AEncounterVolume::RegisterCompletedAttack(AEnemyBaseController* RegisteringController)
{
	(RegisteringController->IsBasic() ? LastAttackerBasic : LastAttackerHeavy) = RegisteringController;
	StartQueueTimer(RegisteringController->IsBasic());
}

void AEncounterVolume::RegisterUnitDestroyed(AEnemyBaseController* Unit, bool bForceDespawn)
{
	// Remove units from queues and set
	TArray<AEnemyBaseController*>* AttackQueue = GetAttackQueue(Unit->IsBasic());
	if(AttackQueue->Contains(Unit)) AttackQueue->Remove(Unit);
	if(ContainedUnits.Contains(Unit->GetOwnPawn())) ContainedUnits.Remove(Unit->GetOwnPawn());

	// Check if the encounter is complete
	if(ContainedUnits.Num() <= 0 && AllSpawnsComplete())
	{
		bIsEncounterComplete = true;
		if(EncounterComplete.IsBound()) EncounterComplete.Broadcast();
	}

	if(bForceDespawn)
	{
		DespawnUnit(Unit);	
	} else
	{
		FTimerHandle DespawnHandle;
		FTimerDelegate DespawnDelegate;
		DespawnDelegate.BindUFunction(this, FName("DespawnUnit"), Unit);
		WorldTimerManager->SetTimer(DespawnHandle, DespawnDelegate, DespawnTimer, false);
	}
	
	// Check if spawn has started yet
	if(bWaveStartedSpawning) return;
	// If not
	++DefeatedUnits;
	
	// Check if should start
	if(!CurrentWaveParams) return;
	
	const float CompletionPercentage = InitialUnits == 0.f ? InitialUnits : static_cast<float>(DefeatedUnits / InitialUnits);
	if(CompletionPercentage > CurrentWaveParams->SpawnStartEncounterCompletionPercent / 100.f)
	{
		TryCacheTimerManager();
		WorldTimerManager->ClearTimer(SpawnStartTimer);
		StartSpawn();
	}
}

void AEncounterVolume::AddSpawnedUnitToEncounter(AEnemyBase* Unit)
{
	ContainedUnits.Add(Unit);
	Unit->RegisterOwningEncounter(this);
	if(bIsEncounterActive)
	{
		Unit->EngageTarget(EncounterTarget);
	}
}

void AEncounterVolume::RemoveDeadUnitFromEncounter(AEnemyBaseController* DeadUnit)
{
	// Remove units from queues and set
	if(AttackQueueBasic.Contains(DeadUnit)) AttackQueueBasic.Remove(DeadUnit);
	if(AttackQueueHeavy.Contains(DeadUnit)) AttackQueueHeavy.Remove(DeadUnit);
	if(ContainedUnits.Contains(DeadUnit->GetOwnPawn())) ContainedUnits.Remove(DeadUnit->GetOwnPawn());
}

// Called when the game starts or when spawned
void AEncounterVolume::BeginPlay()
{
	Super::BeginPlay();
	Setup();
}

void AEncounterVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(EncounterTarget)
	{
		EncounterTarget->OnCounterStart.Unbind();
		EncounterTarget->OnCounterStart.Unbind();
	}
	Super::EndPlay(EndPlayReason);
}

void AEncounterVolume::BeginAttack(bool bIsBasic)
{
	if(bIsPCCountering) {
		StartQueueTimer(bIsBasic);
		return;
	}
	TArray<AEnemyBaseController*>* AttackQueue = GetAttackQueue(bIsBasic);
	const int8 QueueSize = AttackQueue->Num();
	if (QueueSize == 0) return;
	if (QueueSize == 1 && Cast<AEnemyBase>((*AttackQueue)[0]->GetPawn())->GetCurrentEnemyState() == EEnemyCurrentState::Stunned) StartQueueTimer(bIsBasic);
	
	int8 RandomIndex = FMath::RandRange(0, QueueSize - 1);

	while(QueueSize > 1 && ((bIsBasic ? LastAttackerBasic : LastAttackerHeavy) == (*AttackQueue)[RandomIndex] || Cast<AEnemyBase>((*AttackQueue)[RandomIndex]->GetPawn())->GetCurrentEnemyState() == EEnemyCurrentState::Stunned))
	{
		RandomIndex = FMath::RandRange(0, QueueSize - 1);
	}
	
	if((*AttackQueue)[RandomIndex]) (*AttackQueue)[RandomIndex]->BeginAttack();
	AttackQueue->RemoveAt(RandomIndex);
}

void AEncounterVolume::StartSpawn()
{
	if(bWaveStartedSpawning) return;
	bWaveStartedSpawning = true;
	if (CurrentWaveParams)
	{
		// Reset Spawn Points
		ResetSpawnPoints();
	}

	for(ASpawnPoint* SpawnPoint : CurrentWaveParams->ContainedSpawnPoints)
	{
		if(!SpawnPoint) continue;
		SpawnPoint->SetUnitPool(UnitPool);
		SpawnPoint->StartSpawningUnits();
	}
	TriggerNextWave();
}

void AEncounterVolume::DespawnUnit(AEnemyBaseController* Unit)
{
	if(!UnitPool)
	{
		Unit->GetOwnPawn()->Destroy();
		return;
	}
	UnitPool->AddUnitToPool(Unit->GetOwnPawn());
}

void AEncounterVolume::PCCounterStart()
{
	bIsPCCountering = true;
}

void AEncounterVolume::PCCounterStop()
{
	bIsPCCountering = false;
}

void AEncounterVolume::Setup()
{
	bIsPCCountering = false;
	WorldTimerManager = &GetWorldTimerManager();
	bIsEncounterActive = false;
	bIsEncounterComplete = false;
	LastAttackerBasic = LastAttackerHeavy = nullptr;
	BasicQueueDelegate.BindUFunction(this, FName("BeginAttack"));
	HeavyQueueDelegate.BindUFunction(this, FName("BeginAttack"), false);
	EncounterTarget = nullptr;
	CurrentWaveParams = nullptr;
	InitialUnits = ContainedUnits.Num();
	CurrentWave = -1;
	RegisterEncounterForUnits();
	Cast<UProjectTentacleGameInstance>(GetGameInstance())->RegisterEncounterVolume(this);
}

// for loop to send all enemy to reposition
void AEncounterVolume::SendAllEnemyToReposition(bool DoesIncludeHeavy)
{
	for(const AEnemyBaseController* EachEnemyBaseController: AttackQueueBasic)
	{
		const AEnemyBase* EachOwnPawnRef = EachEnemyBaseController->GetOwnPawn();

		if(!EachOwnPawnRef) continue; 
		
		UBlackboardComponent* EachOwnBBComp = EachOwnPawnRef->GetBBComponent();
		if(!EachOwnBBComp) continue; 

		FName BBRepositionBoolName = "bNeedToReposition";
		
		EachOwnBBComp->SetValueAsBool(BBRepositionBoolName, true);

		// Clear the circling state if they need to reposition
		EachOwnBBComp->ClearValue("bShouldCircle");
	}
}

void AEncounterVolume::AssignQueueEnemyToReposition_Implementation(bool DoesIncludeHeavy)
{
	IEncounterVolumeInterface::AssignQueueEnemyToReposition_Implementation(DoesIncludeHeavy);

	SendAllEnemyToReposition(DoesIncludeHeavy);
}

void AEncounterVolume::MarkComplete()
{
	bIsEncounterComplete = true;
	for(AEnemyBase* Unit: ContainedUnits)
	{
		ContainedUnits.Remove(Unit);
		Unit->OnDeath();
	}
}

bool AEncounterVolume::IsComplete() const
{
	return bIsEncounterComplete;
}

// Register this encounter with contained units
void AEncounterVolume::RegisterEncounterForUnits()
{
	for(AEnemyBase* ContainedUnit : ContainedUnits)
	{
		if(!ContainedUnit) continue;
		ContainedUnit->RegisterOwningEncounter(this);
	}
}

void AEncounterVolume::RegisterEncounterForSpawnPoints()
{
	for(ASpawnPoint* SpawnPoint : CurrentWaveParams->ContainedSpawnPoints)
	{
		if(!SpawnPoint) continue;
		SpawnPoint->RegisterOwningEncounter(this);
	}
}

// Trigger for contained units to engage the target
void AEncounterVolume::EngageContainedUnits(AActor* Target)
{
	EncounterTarget = Cast<APlayerCharacter>(Target);
	if(EncounterTarget)
	{
		EncounterTarget->OnCounterStart.BindDynamic(this, &AEncounterVolume::PCCounterStart);
		EncounterTarget->OnCounterStop.BindDynamic(this, &AEncounterVolume::PCCounterStop);
	}
	
	for(AEnemyBase* ContainedUnit : ContainedUnits)
	{
		ContainedUnit->EngageTarget(Target);
	}
}

// Check if all spawn points have completed
bool AEncounterVolume::AllSpawnsComplete() const
{
	bool bAllComplete = true;
	if (!CurrentWaveParams) return bAllComplete;

	for(const ASpawnPoint* SpawnPoint : CurrentWaveParams->ContainedSpawnPoints)
	{
		if(!SpawnPoint) continue;
		bAllComplete = bAllComplete && SpawnPoint->IsSpawningComplete();
	}

	return bAllComplete;
}

void AEncounterVolume::TryCacheTimerManager() const
{
	if(WorldTimerManager) return;
	
	WorldTimerManager = &GetWorldTimerManager();
}

void AEncounterVolume::StartQueueTimer(bool bIsBasic)
{
	TryCacheTimerManager();
	bIsBasic ? WorldTimerManager->SetTimer(BasicQueueTimer, BasicQueueDelegate, AttackStartDelayBasic, false, AttackStartDelayBasic)
	: WorldTimerManager->SetTimer(HeavyQueueTimer, HeavyQueueDelegate, AttackStartDelayHeavy, false, AttackStartDelayHeavy);
}

TArray<AEnemyBaseController*>* AEncounterVolume::GetAttackQueue(bool bIsBasic)
{
	return bIsBasic ? &AttackQueueBasic : &AttackQueueHeavy;
}

void AEncounterVolume::TriggerNextWave()
{
	bWaveStartedSpawning = false;
	// Increment wave number
	++CurrentWave;
	if(CurrentWave >= WaveParameters.Num()) return;
	CurrentWaveParams = &WaveParameters[CurrentWave];
	if(!CurrentWaveParams) return;
	// Register Spawn Points
	RegisterEncounterForSpawnPoints();
	// Start timer
	TryCacheTimerManager();
	WorldTimerManager->SetTimer(SpawnStartTimer, this, &AEncounterVolume::StartSpawn, CurrentWaveParams->SpawnStartTime, false, CurrentWaveParams->SpawnStartTime);
	// Check percentage based start
}

void AEncounterVolume::ResetSpawnPoints() const
{
	for(ASpawnPoint* SpawnPoint : CurrentWaveParams->ContainedSpawnPoints)
	{
		if(!SpawnPoint) continue;
		SpawnPoint->StopSpawningUnits();
	}
}
