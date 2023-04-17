// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterActionInterface.generated.h"



class AEnemyBase;


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCharacterActionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */



class PROJECTTENTACLE_API ICharacterActionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ActionEnd(bool BufferingCheck);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartWaitForCombo();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TryTriggerPlayerCounter();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DetachEnemyTarget();


	// Player Delegate
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TryStoreCounterTarget(AEnemyBase* CounterTarget);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TryRemoveCounterTarget(AEnemyBase* CounterTarget);


	// Enemy Delegate
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnCounterTimeEnd();
	
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ReceiveAttackInCounterState(AActor* CounteringTarget);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartLyingOnTheGround();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void RepeatLyingOnTheGround();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TryToDamagePlayer();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DamagingTarget();
};
