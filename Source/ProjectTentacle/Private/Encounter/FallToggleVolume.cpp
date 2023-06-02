// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Encounter/FallToggleVolume.h"

#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AFallToggleVolume::AFallToggleVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Volume"));

	SetRootComponent(TriggerVolume);
}

void AFallToggleVolume::BeginPlay()
{
	Super::BeginPlay();
	ContainedMoveableActors.Empty();

	// Bind UFunctions to begin/end overlap
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AFallToggleVolume::OnEnter);
	TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &AFallToggleVolume::OnExit);
}

void AFallToggleVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unbind overlap methods
	TriggerVolume->OnComponentBeginOverlap.RemoveDynamic(this, &AFallToggleVolume::OnEnter);
	TriggerVolume->OnComponentEndOverlap.RemoveDynamic(this, &AFallToggleVolume::OnExit);
	
	Super::EndPlay(EndPlayReason);
}

void AFallToggleVolume::OnEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check that actor has char movement comp
	UCharacterMovementComponent* ActorMovement = GetCharMovementComp(OtherActor);
	if(!ActorMovement) return;

	// Save reference and initial value of bCanWalkOffLedges
	ContainedMoveableActors.Add(OtherActor, ActorMovement->bCanWalkOffLedges);
	ActorMovement->bCanWalkOffLedges = bShouldAllowFall;
}

void AFallToggleVolume::OnExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!ContainedMoveableActors.Contains(OtherActor)) return;
	
	UCharacterMovementComponent* ActorMovement = GetCharMovementComp(OtherActor);
	if(!ActorMovement) return;

	// Pop actor from TMap and reassign initial value of bCanWalkOffLedges 
	ActorMovement->bCanWalkOffLedges = ContainedMoveableActors.FindAndRemoveChecked(OtherActor);
}

UCharacterMovementComponent* AFallToggleVolume::GetCharMovementComp(const AActor* Actor) const
{
	return Cast<UCharacterMovementComponent>(Actor->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
}
