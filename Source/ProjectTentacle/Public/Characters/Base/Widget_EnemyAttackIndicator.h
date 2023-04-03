// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnumClassesForCharacters.h"
#include "Widget_EnemyBaseUserWidget.h"
#include "Widget_EnemyAttackIndicator.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTTENTACLE_API UWidget_EnemyAttackIndicator : public UWidget_EnemyBaseUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void OnReceivingNewAttackType(EEnemyAttackType NewAttackType);
	
};
