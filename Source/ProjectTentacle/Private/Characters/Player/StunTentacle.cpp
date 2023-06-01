// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Player/StunTentacle.h"

#include "Characters/Base/CharacterActionInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void AStunTentacle::LifeCycleBegin()
{
	Super::LifeCycleBegin();


	StunEnemy();
	
	// // Set timer to stun enemy
	// WorldRef->GetTimerManager().SetTimer(StunEnemyTimerHandle, this, &AStunTentacle::StunEnemy, TimeToExecuteStun, false, -1);

}

// void AStunTentacle::AbsorbEnemy()
// {
// 	// Get enemy in range
// 	const FVector CurrentTentaclePos = GetActorLocation();
// 	
// 	TArray<AActor*> FoundEnemyActors = GetEnemyInRadius(AbsorbingRadius, CurrentTentaclePos);
//
// 	if(FoundEnemyActors.Num() < 1) return;
//
// 	const float WorldDelta = WorldRef->GetDeltaSeconds();
//
// 	for (AActor* EachFoundEnemyActor : FoundEnemyActors)
// 	{
// 		if(EachFoundEnemyActor->GetClass()->ImplementsInterface(UCharacterActionInterface::StaticClass()))
// 		{
// 			// Try Stop Movement and pull enemy over here
// 			ICharacterActionInterface::Execute_OnPullingEnemy(EachFoundEnemyActor, CurrentTentaclePos, AbsorbingPower, WorldDelta);
// 		}
// 	}
// 	
// 	// running absorb timer
// 	CurrentExecutionTime += EachAbsorbPerTick;
//
// 	// Early return if timer is not up
// 	if(CurrentExecutionTime < TimeToExecuteStun) return;
//
// 	// clear timer and stun enemy
// 	WorldRef->GetTimerManager().ClearTimer(StunEnemyTimerHandle);
// 	StunEnemy();
// }

void AStunTentacle::StunEnemy()
{
	// Get enemy in range
	const FVector CurrentTentaclePos = GetActorLocation();

	TArray<AActor*> FoundEnemyActors = GetEnemyInRadius(AbsorbingRadius, CurrentTentaclePos);

	for (AActor* EachFoundEnemyActor : FoundEnemyActors)
	{
		if(EachFoundEnemyActor->GetClass()->ImplementsInterface(UCharacterActionInterface::StaticClass()))
		{
			// TODO: Delegate to Stun Enemies
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Stun Enemy!"));
			
		}
	}
}

void AStunTentacle::OnLifeCycleEnd()
{
	Super::OnLifeCycleEnd();
}


