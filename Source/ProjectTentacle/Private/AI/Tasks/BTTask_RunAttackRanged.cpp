// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_RunAttackRanged.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

UBTTask_RunAttackRanged::UBTTask_RunAttackRanged()
{
	NodeName = "Run Ranged Attack";
}

EBTNodeResult::Type UBTTask_RunAttackRanged::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// TODO Pop up indicator
	TryCacheRefs(OwnerComp);
	if(!bHasCachedRefs) return EBTNodeResult::Failed;

	Target->ShowHitIndicator(OwnPawn->GetAttackCounterableTime(), OwnPawn->GetActorLocation());
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UBTTask_RunAttackRanged::AttackCounterTimeLimit()
{
	Super::AttackCounterTimeLimit();
	
	if(!Target) return;

	FHitResult TraceResult;
	const FVector OwnLocation = OwnPawn->GetActorLocation();
	FVector Direction2D = Target->GetActorLocation() - OwnLocation;
	Direction2D.Z = 0;
	Direction2D.Normalize();
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), OwnLocation, OwnLocation + Direction2D * AttackRange, UEngineTypes::ConvertToTraceType(ECC_Camera), false, { OwnPawn }, EDrawDebugTrace::ForDuration, TraceResult, true, FColor::Yellow, FColor::Purple);

	if(TraceResult.bBlockingHit)
	{
		// GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Purple, FString::Printf(TEXT("Hit %s"), *TraceResult.Actor->GetHumanReadableName()));
		OwnPawn->ExecuteRangedAttack(TraceResult.Actor.Get());
	}
}

void UBTTask_RunAttackRanged::AttackCompletionTimeLimit()
{
	Target->CollapseHitIndicator();
	Super::AttackCompletionTimeLimit();
}

EBTNodeResult::Type UBTTask_RunAttackRanged::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Target->CollapseHitIndicator();
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_RunAttackRanged::TryCacheRefs(UBehaviorTreeComponent& OwnerComp)
{
	Super::TryCacheRefs(OwnerComp);
	Target = Cast<APlayerCharacter>(OwningComp->GetBlackboardComponent()->GetValueAsObject("Target"));
	bHasCachedRefs = bHasCachedRefs && Target;
}
