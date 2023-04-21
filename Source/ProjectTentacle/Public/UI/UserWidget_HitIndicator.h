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
public:
	void PopIndicator(float TransitionTime, FVector HitLocation);
	void CollapseIndicator();
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	class UImage* HitIndicator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hit Indicator")
	UMaterialInterface* HitIndicatorMaterial;

	UPROPERTY( Transient, meta = ( BindWidgetAnim ) )
	UWidgetAnimation* ShowHitIndicator;

	UPROPERTY( Transient, meta = ( BindWidgetAnim ) )
	UWidgetAnimation* HideHitIndicator;
	
private:
	void HideIndicator();
	UMaterialInstanceDynamic* HitIndicatorDynamicInst;
	FWidgetAnimationDynamicEvent HideAnimationDelegate;
};
