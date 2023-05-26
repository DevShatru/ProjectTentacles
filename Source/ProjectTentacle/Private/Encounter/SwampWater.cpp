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
	if(ContainedActors.Contains(OtherActor)) return;
	APlayerCharacter* AsPC = Cast<APlayerCharacter>(OtherActor);
	if(!AsPC) return;

	ContainedActors.Add(AsPC);
	AsPC->StartSwampDamageTick(DamagePerTick, TickInterval);
}

void ASwampWater::ExitSwamp(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!ContainedActors.Contains(OtherActor)) return;
	APlayerCharacter* AsPC = Cast<APlayerCharacter>(OtherActor);
	if(!AsPC) return;

	AsPC->StopSwampDamageTick();
	ContainedActors.Remove(AsPC);
}

