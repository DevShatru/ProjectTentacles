// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidget_HitIndicator.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UUserWidget_HitIndicator : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	class UImage* HitIndicator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hit Indicator")
	UMaterialInterface* HitIndicatorMaterial;
	
private:
	UMaterialInstanceDynamic* HitIndicatorDynamicInst;
};
