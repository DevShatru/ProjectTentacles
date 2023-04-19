// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_RunAttackMelee.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Characters/Enemies/EnemyBaseController.h"

UBTTask_RunAttackMelee::UBTTask_RunAttackMelee()
{
	NodeName = "Run Melee Attack";
}

EBTNodeResult::Type UBTTask_RunAttackMelee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Fail early if we can't get a reference to the controller or pawn
	// FTimerHandle CounterTimer, CompletionTimer;
	AEnemyBaseController* OwnController = Cast<AEnemyBaseController>(OwnerComp.GetAIOwner());
	if(!OwnController) return EBTNodeResult::Failed;
	
	OwnPawn = OwnController->GetOwnPawn();
	if(!OwnPawn) return EBTNodeResult::Failed;

	// Allocate Owner's behaviour tree component and bind with signature
	if(!OwnPawn->GetBehaviourTreeComponent())
		OwnPawn->SetBehaviourTreeComponent(&OwnerComp);
		
	OwnPawn->OnFinishAttackingTask.BindDynamic(this, &UBTTask_RunAttackMelee::AttackFinishTask);


	OwnPawn->ExecuteAttack();
	
	OwningComp = &OwnerComp;

	
	
	// Create timers for counter timeout and completion timeout
	FTimerManager* WorldTimerManager = &GetWorld()->GetTimerManager();
	WorldTimerManager->SetTimer(CounterTimer, this, &UBTTask_RunAttackMelee::AttackCounterTimeLimit,OwnPawn->GetAttackCounterableTime(), false, OwnPawn->GetAttackCounterableTime());
	WorldTimerManager->SetTimer(CompletionTimer, this, &UBTTask_RunAttackMelee::AttackCompletionTimeLimit,OwnPawn->GetAttackCompletionTime(), false, OwnPawn->GetAttackCompletionTime());

	return EBTNodeResult::InProgress;
}

// Log counter timeout
void UBTTask_RunAttackMelee::AttackCounterTimeLimit()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Purple, FString::Printf(TEXT("%s cannot be countered"), *OwnPawn->GetHumanReadableName()));
}

// Log completion timeout
void UBTTask_RunAttackMelee::AttackCompletionTimeLimit()
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Purple, FString::Printf(TEXT("%s completed attack"), *OwnPawn->GetHumanReadableName()));
	FinishLatentTask(*OwningComp, EBTNodeResult::Succeeded);
}

void UBTTask_RunAttackMelee::AttackFinishTask(UBehaviorTreeComponent* BehaviorTreeReference, bool bIsSuccess, bool DoesGetInterupted)
{
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Purple, FString::Printf(TEXT("%s completed attack"), *OwnPawn->GetHumanReadableName()));
	FinishLatentTask(*BehaviorTreeReference, EBTNodeResult::Succeeded);
}
