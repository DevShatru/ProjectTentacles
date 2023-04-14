// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Enemies/EnemyBaseController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Encounter/EncounterVolume.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


AEnemyBaseController::AEnemyBaseController()
{
	bAttachToPawn = true;
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));

	// Set up perception system and sight sense
	Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	
	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectFriendlies = false;
	Sight->DetectionByAffiliation.bDetectNeutrals = false;
	
	Perception->SetDominantSense(UAISense_Sight::StaticClass());
	Perception->ConfigureSense(*Sight);

	// Configure to team 2
	SetGenericTeamId(FGenericTeamId(2));
}

void AEnemyBaseController::BeginPlay()
{
	Super::BeginPlay();
	EncounterTarget = nullptr;
	if(!BehaviorTree) return;

	// Init blackboard and run master behaviour
	Blackboard->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	RunBehaviorTree(BehaviorTree);

	// Bind UFunction to perception updated delegate
	Perception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyBaseController::UpdatePerception);
}

void AEnemyBaseController::RegisterOwningEncounter(AEncounterVolume* NewOwningEncounter)
{
	OwningEncounter = NewOwningEncounter;
}

// Engage target and disable further perception
void AEnemyBaseController::EngageTarget(AActor* Target)
{
	Blackboard->SetValueAsBool("bIsEncounterActive", true);
	Blackboard->SetValueAsObject("Target", Target);
	EncounterTarget = Target;
	Perception->SetSenseEnabled(UAISense_Sight::StaticClass(), false);
}

TArray<AEnemyBase*> AEnemyBaseController::GetAllies() const
{
	return OwningEncounter ? OwningEncounter->GetAlliesForPawn(OwnPawn) : TArray<AEnemyBase*>();
}

// Add unit to attack queue
void AEnemyBaseController::RegisterOnAttackQueue()
{
	if(!OwningEncounter) return;
	OwningEncounter->RegisterOnBasicAttackQueue(this);
}

// Trigger attack state
void AEnemyBaseController::BeginAttack()
{
	// Begin attack montage here
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Purple, FString::Printf(TEXT("%s began attack"), *OwnPawn->GetHumanReadableName()));
	TryCacheBlackboardComp();
	Blackboard->SetValueAsBool("bIsAttacking", true);
}

void AEnemyBaseController::QuitFromEncounter()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Purple, FString::Printf(TEXT("%s left queue list"), *OwnPawn->GetHumanReadableName()));

	// TODO: Change to RegisterUnitDestroyed 
	OwningEncounter->RemoveDeadUnitFromEncounter(this);
}

// Register after attack has completed
void AEnemyBaseController::RegisterCompletedAttack()
{
	if(!OwningEncounter) return;
	OwningEncounter->RegisterCompletedBasicAttack(this);
}

AEnemyBase* AEnemyBaseController::GetOwnPawn() const
{
	return OwnPawn;
}

void AEnemyBaseController::Reset()
{
	Super::Reset();
	OwnPawn->Reset();
	ClearBlackboard();
}

void AEnemyBaseController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	OwnPawn = Cast<AEnemyBase>(InPawn);
}

void AEnemyBaseController::UpdatePerception(AActor* Actor, FAIStimulus Stimulus)
{
	// GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Purple, Actor->GetHumanReadableName());
	if(!OwningEncounter) return;
	OwningEncounter->TryTriggerEncounter(Actor);
}

void AEnemyBaseController::OnDeath()
{
	Reset();
	if(!OwningEncounter) return;
	OwningEncounter->RegisterUnitDestroyed(this);
}

void AEnemyBaseController::ClearBlackboard()
{
	TryCacheBlackboardComp();
	for (UBlackboardData* It = Blackboard->GetBlackboardAsset(); It; It = It->Parent)
	{
		for (int32 KeyIndex = 0; KeyIndex < It->Keys.Num(); KeyIndex++)
		{
			const UBlackboardKeyType* KeyType = It->Keys[KeyIndex].KeyType;
			if (KeyType)
			{
				const int32 UseIdx = KeyIndex + It->GetFirstKeyID();
				Blackboard->ClearValue(UseIdx);
			}
		}
	}
}

void AEnemyBaseController::TryCacheBlackboardComp()
{
	if(!Blackboard) Blackboard = GetBlackboardComponent();
}
