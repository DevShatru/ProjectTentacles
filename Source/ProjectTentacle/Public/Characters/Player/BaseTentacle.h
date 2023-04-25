// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseTentacle.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PROJECTTENTACLE_API ABaseTentacle : public AActor
{
	GENERATED_BODY()
	
private:

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TotalLifeCycleTime = 15.0f;

	FTimerHandle LifeCycleTimerHandle;


	UWorld* WorldRef;


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	virtual void LifeCycleBegin();

	bool CachingWorldRef();

	UFUNCTION()
	virtual void OnLifeCycleEnd();

	
	
public:
	
	// Sets default values for this actor's properties
	ABaseTentacle();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
