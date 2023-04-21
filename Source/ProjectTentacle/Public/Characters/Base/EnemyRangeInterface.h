// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyRangeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyRangeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTTENTACLE_API IEnemyRangeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnRifleBeginAiming();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnRifleFinishFiring();
	
};
