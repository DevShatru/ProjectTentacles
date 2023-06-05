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
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* WaterMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBoxComponent* DamageVolume;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UNavModifierComponent* NavModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Swamp Damage")
	float DamagePerTick = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Swamp Damage")
	float TickInterval = 3.0f;

	UFUNCTION()
	void EnterSwamp(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void ExitSwamp(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	TMap<AActor*, TArray<UPrimitiveComponent*>> ContainedActors;
};
