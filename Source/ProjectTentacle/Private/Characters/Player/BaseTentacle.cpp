// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Player/BaseTentacle.h"

#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ABaseTentacle::ABaseTentacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

bool ABaseTentacle::CachingWorldRef()
{
	// early return if world ref is nullptr
	if(WorldRef) return true;

	UWorld* World = GetWorld();

	if(!World) return false;

	WorldRef = World;

	return true;
}

// Called when the game starts or when spawned
void ABaseTentacle::BeginPlay()
{
	Super::BeginPlay();

	LifeCycleBegin();
}

void ABaseTentacle::LifeCycleBegin()
{
	const bool IsCacheSuccess = CachingWorldRef();
	if(!IsCacheSuccess) return;
	
	// Set timer to end life cycle
	WorldRef->GetTimerManager().SetTimer(LifeCycleTimerHandle, this, &ABaseTentacle::OnLifeCycleEnd, TotalLifeCycleTime, false, -1);
}

void ABaseTentacle::OnLifeCycleEnd()
{
	// TODO: Change this to object pooling
	Destroy();
}

TArray<AActor*> ABaseTentacle::GetEnemyInRadius(float DetectionRadius, FVector OverlapStartPos)
{
	TArray<AActor*> FoundActorList;
	
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	
	UKismetSystemLibrary::SphereOverlapActors(WorldRef, OverlapStartPos, DetectionRadius, FilterType, FilteringClass, IgnoreActors,FoundActorList);
	
	return FoundActorList;
}

// Called every frame
void ABaseTentacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

