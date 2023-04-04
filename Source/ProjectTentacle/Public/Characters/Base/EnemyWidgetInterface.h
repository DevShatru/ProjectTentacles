// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyWidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTTENTACLE_API IEnemyWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayIndicatorAnimation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowEnemyAttackIndicator();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UnShowEnemyAttackIndicator();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowPlayerTargetIndicator();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UnShowPlayerTargetIndicator();
};
