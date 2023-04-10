// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnemyType.h"
#include "GameFramework/Actor.h"
#include "UnitPool.generated.h"

UCLASS()
class PROJECTTENTACLE_API AUnitPool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUnitPool();
	void AddUnitToPool(class AEnemyBase* Unit);
	AEnemyBase* GetUnitFromPool(EEnemyType Type);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	UPROPERTY()
	TArray<AEnemyBase*> PooledUnits;
};
