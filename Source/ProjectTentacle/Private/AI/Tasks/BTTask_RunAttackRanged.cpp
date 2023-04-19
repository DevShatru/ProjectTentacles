// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_RunAttackRanged.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Kismet/KismetSystemLibrary.h"

UBTTask_RunAttackRanged::UBTTask_RunAttackRanged()
{
	NodeName = "Run Ranged Attack";
}

void UBTTask_RunAttackRanged::AttackCounterTimeLimit()
{
	Super::AttackCounterTimeLimit();
	
	const AActor* Target = Cast<AActor>(OwningComp->GetBlackboardComponent()->GetValueAsObject("Target"));
	if(!Target) return;

	FHitResult TraceResult;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), OwnPawn->GetActorLocation(), Target->GetActorLocation(), UEngineTypes::ConvertToTraceType(ECC_Camera), false, { OwnPawn }, EDrawDebugTrace::ForDuration, TraceResult, true, FColor::Yellow, FColor::Purple);

	if(TraceResult.bBlockingHit)
	{
		// GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Purple, FString::Printf(TEXT("Hit %s"), *TraceResult.Actor->GetHumanReadableName()));
		OwnPawn->ExecuteRangedAttack(TraceResult.Actor.Get());
	}
}
