// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

class UProjectTentacleGameInstance;
UCLASS()
class PROJECTTENTACLE_API ACheckpoint : public ATargetPoint
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint();
	virtual void BeginPlay() override;
	FVector GetOffsetLocation() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector HeightOffset = FVector(0.f, 0.f, 88.f);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBoxComponent* CheckpointTrigger;
	
	UFUNCTION()
	void OnTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
private:
	UProjectTentacleGameInstance* InstanceRef;
	unsigned int bIsActive;
};
