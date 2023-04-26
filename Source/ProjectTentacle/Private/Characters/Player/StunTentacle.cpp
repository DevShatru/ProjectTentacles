// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Player/StunTentacle.h"

#include "Characters/Base/CharacterActionInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

TArray<AActor*> AStunTentacle::GetEnemyInRadius(float DetectionRadius)
{
	TArray<AActor*> FoundActorList;
	
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	const FVector GroundLocation = GetActorLocation();
	
	UKismetSystemLibrary::SphereOverlapActors(WorldRef, GroundLocation, DetectionRadius, FilterType, FilteringClass, IgnoreActors,FoundActorList);
	
	return FoundActorList;
}

void AStunTentacle::LifeCycleBegin()
{
	Super::LifeCycleBegin();

	// Set timer to stun enemy
	WorldRef->GetTimerManager().SetTimer(AbsorbingEnemyTimerHandle, this, &AStunTentacle::AbsorbEnemy, EachAbsorbPerTick, true, -1);

}

void AStunTentacle::AbsorbEnemy()
{
	// Get enemy in range
	TArray<AActor*> FoundEnemyActors = GetEnemyInRadius(AbsorbingRadius);

	if(FoundEnemyActors.Num() < 1) return;
	
	const FVector CurrentTentaclePos = GetActorLocation();

	float WorldDelta = WorldRef->GetDeltaSeconds();

	for (AActor* EachFoundEnemyActor : FoundEnemyActors)
	{
		if(EachFoundEnemyActor->GetClass()->ImplementsInterface(UCharacterActionInterface::StaticClass()))
		{
			// Try Stop Movement and pull enemy over here
			ICharacterActionInterface::Execute_OnPullingEnemy(EachFoundEnemyActor, CurrentTentaclePos, AbsorbingPower, WorldDelta);
		}
	}
	
	// running absorb timer
	CurrentExecutionTime += EachAbsorbPerTick;

	// Early return if timer is not up
	if(CurrentExecutionTime < TimeToExecuteStun) return;

	// clear timer and stun enemy
	WorldRef->GetTimerManager().ClearTimer(AbsorbingEnemyTimerHandle);
	StunEnemy();
}

void AStunTentacle::StunEnemy()
{
	// Get enemy in range
	TArray<AActor*> FoundEnemyActors = GetEnemyInRadius(AbsorbingRadius);

	for (AActor* EachFoundEnemyActor : FoundEnemyActors)
	{
		if(EachFoundEnemyActor->GetClass()->ImplementsInterface(UCharacterActionInterface::StaticClass()))
		{
			// Try Stop Movement and pull enemy over here
			// TODO: Delegate to Stun Enemies
			ICharacterActionInterface::Execute_OnResumeMovement(EachFoundEnemyActor);
		}
	}
}

void AStunTentacle::OnLifeCycleEnd()
{
	Super::OnLifeCycleEnd();

	// TODO: Maybe implement Stun Tentacle End Cycle
}


