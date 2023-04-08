// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Widget_EnemyBaseUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UWidget_EnemyBaseUserWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY( meta = ( BindWidget ))
	UImage* IndicationImage;

	

public:
	
	virtual void ShowIndicator();

	virtual void UnShowIndicator();
};
