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

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBoxComponent* CheckpointTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsActive = true;

	// Triggers save on the game instance if valid
	UFUNCTION()
	void OnTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
private:
	UProjectTentacleGameInstance* InstanceRef;
};
