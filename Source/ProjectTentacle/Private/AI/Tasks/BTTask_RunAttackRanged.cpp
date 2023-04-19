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
