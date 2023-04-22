// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnumClassesForCharacters.h"
#include "Widget_EnemyBaseUserWidget.h"
#include "Characters/Enemies/EnemyType.h"
#include "Widget_EnemyAttackIndicator.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTTENTACLE_API UWidget_EnemyAttackIndicator : public UWidget_EnemyBaseUserWidget
{
	GENERATED_BODY()

private:
	
protected:
	EEnemyAttackType CurrentAttackType;

	UPROPERTY(BlueprintReadOnly)
	EEnemyType OwnerEnemyType;

	UPROPERTY()
	TMap<FName, UWidgetAnimation*> WidgetAnimMap;

	// UPROPERTY(Transient, meta = (BindWidgetAnim))
	// UWidgetAnimation* YellowToRedAnim_MMAKick;
	//
	// UPROPERTY(Transient, meta = (BindWidgetAnim))
	// UWidgetAnimation* AllGreenAnim_HeadButt;
	
public:
	UFUNCTION()
	void OnReceivingNewAttackType(EEnemyAttackType NewAttackType);

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySpecificAnimation(EEnemyAttackType NewAttackType);

	void StoreWidgetAnimation();

	void SetUnitType(EEnemyType OwnerType) {OwnerEnemyType = OwnerType;}

	
	UWidgetAnimation* GetAnimationByName(FName AnimationName);

	void TryPlayIndicationAnimation();

	virtual void NativeConstruct() override;

	virtual void ShowIndicator() override;

	
};
