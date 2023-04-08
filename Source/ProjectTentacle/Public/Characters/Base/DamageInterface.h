// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnumClassesForCharacters.h"
#include "UObject/Interface.h"
#include "DamageInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTTENTACLE_API IDamageInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveDamageFromPlayer(int32 DamageAmount, AActor* DamageCauser, EPlayerAttackType PlayerAttackType);

	UFUNCTION(BlueprintNativeEvent)
	void ReceiveDamageFromEnemy(int32 DamageAmount, AActor* DamageCauser, EEnemyAttackType EnemyAttackType);
};
