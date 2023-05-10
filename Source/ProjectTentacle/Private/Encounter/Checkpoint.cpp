// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Encounter/Checkpoint.h"

#include "ProjectTentacleGameInstance.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "ProjectTentacle/ProjectTentacleGameModeBase.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	CheckpointTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	CheckpointTrigger->SetupAttachment(RootComponent);
	CheckpointTrigger->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnTrigger);
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	bIsActive = true;
	InstanceRef = Cast<UProjectTentacleGameInstance>(GetWorld()->GetGameInstance());
}

FVector ACheckpoint::GetOffsetLocation() const
{
	return GetActorLocation() + HeightOffset;
}

void ACheckpoint::OnTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!bIsActive) return;
	
	if(const APlayerCharacter* PC = Cast<APlayerCharacter>(OtherActor))
	{
		bIsActive = false;
		if(InstanceRef) InstanceRef->SaveGame();
		if(CheckpointTrigger->OnComponentBeginOverlap.IsAlreadyBound(this, &ACheckpoint::OnTrigger)) CheckpointTrigger->OnComponentBeginOverlap.RemoveDynamic(this, &ACheckpoint::OnTrigger);
	}
}
