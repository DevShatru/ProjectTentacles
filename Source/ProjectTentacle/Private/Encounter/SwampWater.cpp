// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Encounter/SwampWater.h"

#include "NavModifierComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ASwampWater::ASwampWater()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WaterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Water Mesh"));
	DamageVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Volume"));
	NavModifier = CreateDefaultSubobject<UNavModifierComponent>(TEXT("Navigation Modifier"));

	SetRootComponent(WaterMesh);
	DamageVolume->SetupAttachment(WaterMesh);

	DamageVolume->SetRelativeLocation(FVector(0.f, 0.f, 16.f));
	DamageVolume->SetBoxExtent(FVector(50.f, 50.f, 16.f));
	DamageVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	DamageVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	NavModifier->FailsafeExtent = FVector(50.f, 50.f, 10.f);
}

// Called when the game starts or when spawned
void ASwampWater::BeginPlay()
{
	Super::BeginPlay();
	
}

