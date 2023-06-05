// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Encounter/SwampWater.h"

#include "NavModifierComponent.h"
#include "Characters/Player/PlayerCharacter.h"
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
	ContainedActors.Empty();
	DamageVolume->OnComponentBeginOverlap.AddDynamic(this, &ASwampWater::EnterSwamp);
	DamageVolume->OnComponentEndOverlap.AddDynamic(this, &ASwampWater::ExitSwamp);
}

void ASwampWater::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DamageVolume->OnComponentBeginOverlap.RemoveDynamic(this, &ASwampWater::EnterSwamp);
	DamageVolume->OnComponentEndOverlap.RemoveDynamic(this, &ASwampWater::ExitSwamp);
	Super::EndPlay(EndPlayReason);
}

void ASwampWater::EnterSwamp(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check that actor is PC and return early if not
	APlayerCharacter* AsPC = Cast<APlayerCharacter>(OtherActor);
	if(!AsPC) return;

	// Check if actor is a key in map, and create key with empty array if not
	if(!ContainedActors.Contains(AsPC))
	{
		ContainedActors.Add(OtherActor, TArray<UPrimitiveComponent*>());
	}

	// Get a reference to mapped array, if it already contained the overlapped component do nothing (this shouldn't ever happen)
	TArray<UPrimitiveComponent*>* ContainedComponents = ContainedActors.Find(OtherActor);
	if(ContainedComponents->Contains(OtherComp)) return;

	// Otherwise, add the new overlapped component to the map and start damage tick
	ContainedComponents->Add(OtherComp);
	
	AsPC->StartSwampDamageTick(DamagePerTick, TickInterval);
}

void ASwampWater::ExitSwamp(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// If the exiting actor isn't a key in our map or isn't PC, do nothing
	if(!ContainedActors.Contains(OtherActor)) return;
	APlayerCharacter* AsPC = Cast<APlayerCharacter>(OtherActor);
	if(!AsPC) return;

	// Get array reference and remove overlapped component if it exists
	TArray<UPrimitiveComponent*>* ContainedComponents = ContainedActors.Find(OtherActor);
	if(ContainedComponents->Contains(OtherComp)) ContainedComponents->Remove(OtherComp);

	// If no mapped components, we're fully outside the water so stop damage
	if(ContainedComponents->Num() == 0) AsPC->StopSwampDamageTick();
}

