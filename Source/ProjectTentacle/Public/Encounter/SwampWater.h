// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwampWater.generated.h"

UCLASS()
class PROJECTTENTACLE_API ASwampWater : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwampWater();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* WaterMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBoxComponent* DamageVolume;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UNavModifierComponent* NavModifier;
};
