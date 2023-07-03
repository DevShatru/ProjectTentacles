// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Player/AttachingTentacle.h"

#include "Kismet/KismetMaterialLibrary.h"

void AAttachingTentacle::SetAppearingMaterialInstance()
{
	UMaterialInterface* GlowMaterial = TentacleMesh->GetMaterial(0);
	MaterialInstanceREF = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this,GlowMaterial);
	TentacleMesh->SetMaterial(0, MaterialInstanceREF);
}

// Sets default values
AAttachingTentacle::AAttachingTentacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a UMeshComponent
	USkeletalMeshComponent* Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Tentacle Mesh"));
	if(Mesh)
	{
		Mesh->SetupAttachment(RootComponent);
		TentacleMesh = Mesh;
	}
	

}

// Called when the game starts or when spawned
void AAttachingTentacle::BeginPlay()
{
	Super::BeginPlay();

	SetAppearingMaterialInstance();
	SetTentacleInvisible();
}

// Called every frame
void AAttachingTentacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAttachingTentacle::SetMaterialValue(float Alpha)
{
	MaterialInstanceREF->SetScalarParameterValue("Param_Spawning", Alpha);
}

void AAttachingTentacle::SetTentacleInvisible()
{
	MaterialInstanceREF->SetScalarParameterValue("Param_Spawning", 0);
}

