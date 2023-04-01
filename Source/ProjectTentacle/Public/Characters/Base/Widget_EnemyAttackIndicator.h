// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnumClassesForCharacters.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Widget_EnemyAttackIndicator.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTTENTACLE_API UWidget_EnemyAttackIndicator : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY( meta = ( BindWidget ))
	UImage* IndicationImage;


public:
	UFUNCTION()
	void OnReceivingNewAttackType(EEnemyAttackType NewAttackType);

	
};
