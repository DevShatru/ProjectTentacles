// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Player/StunTentacle.h"

#include "Kismet/KismetSystemLibrary.h"

void AStunTentacle::LifeCycleBegin()
{
	Super::LifeCycleBegin();

	// Set timer to stun enemy
	WorldRef->GetTimerManager().SetTimer(PrepareStunTimerHandle, this, &AStunTentacle::StunEnemy, TimeToExecuteStun, false, -1);

}

void AStunTentacle::StunEnemy()
{
	TArray<AActor*> FoundActorList;
	
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	
	UKismetSystemLibrary::SphereOverlapActors(WorldRef, GetActorLocation(), AbsorbingRadius, FilterType, FilteringClass, IgnoreActors,FoundActorList);

	// TODO: Stun Enemy
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Stun Enemy!!"));	
	
}

void AStunTentacle::OnLifeCycleEnd()
{
	Super::OnLifeCycleEnd();

	// TODO: Maybe implement Stun Tentacle End Cycle
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Stun Tentacle Life Cycle End!!"));	

}


