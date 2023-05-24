// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Base/EnumClassesForCharacters.h"
#include "UObject/Interface.h"
#include "PlayerDamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTTENTACLE_API IPlayerDamageInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void ReceiveDamageFromEnemy(int32 DamageAmount, AActor* DamageCauser, EEnemyAttackType EnemyAttackType);
};
