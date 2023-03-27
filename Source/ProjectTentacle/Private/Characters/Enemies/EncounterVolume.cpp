// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Enemies/EncounterVolume.h"

#include "NavigationInvokerComponent.h"

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

// Called when the game starts or when spawned
void AEncounterVolume::BeginPlay()
{
	Super::BeginPlay();
}