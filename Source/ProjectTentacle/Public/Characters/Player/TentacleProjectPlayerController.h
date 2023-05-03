// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "TentacleProjectPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API ATentacleProjectPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	APlayerCharacter* PossessedPlayerRef;
	
public:
	ATentacleProjectPlayerController();
	
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void OnMoveForward(float Value);

	UFUNCTION(BlueprintCallable)
	void OnMoveRight(float Value);

	UFUNCTION(BlueprintCallable)
	void OnEvade();
	
	UFUNCTION(BlueprintCallable)
	void OnMeleeAttack();
	
	UFUNCTION(BlueprintCallable)
	void OnDodge();

	UFUNCTION(BlueprintCallable)
	void OnSpecial1();

	UFUNCTION(BlueprintCallable)
	void OnSpecial2();

	// UFUNCTION(BlueprintCallable)
	// void OnSpecial3();
	
	
	
	UFUNCTION(BlueprintCallable)
	void OnAddControllerYawInput(float Value);

	UFUNCTION(BlueprintCallable)
	void OnTurnAtRate(float Rate);
	
	UFUNCTION(BlueprintCallable)
	void OnAddControllerPitchInput(float Value);

	UFUNCTION(BlueprintCallable)
	void OnLookUpAtRate(float Rate);
};