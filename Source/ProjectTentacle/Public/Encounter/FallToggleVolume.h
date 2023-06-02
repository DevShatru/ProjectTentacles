// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FallToggleVolume.generated.h"

UCLASS()
class PROJECTTENTACLE_API AFallToggleVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFallToggleVolume();

protected:
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Volume used to trigger enable/disable of bCanWalkOffLedges
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBoxComponent* TriggerVolume;

	// Whether this volume should allow walking off ledges
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bShouldAllowFall = true;

	UFUNCTION()
	void OnEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	// List of currently contained actors with a UCharacterMovementComponent, mapped to initial value of bCanWalkOffLedges
	TMap<AActor*, bool> ContainedMoveableActors;

	// Helper to get Character movement comp and cast to right type
	class UCharacterMovementComponent* GetCharMovementComp(const AActor* Actor) const;
};
