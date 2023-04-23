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
	void ChangeVisibility(bool IsVisible);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	class UImage* HitIndicator;
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* ShowHitIndicator;
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	UWidgetAnimation* HideHitIndicator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hit Indicator")
	UMaterialInterface* HitIndicatorMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hit Indicator")
	FLinearColor StartColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hit Indicator")
	FLinearColor FinalColor;
	
	UFUNCTION()
	void UpdateIndicatorLocation();

	UFUNCTION()
	void UpdateIndicatorColor();
	
private:
	void HideIndicator();
	UMaterialInstanceDynamic* HitIndicatorDynamicInst;
	FWidgetAnimationDynamicEvent HideAnimationDelegate;

	UWorld* World;
	float DisplayStartTime;
	float ColorTransitionTime;
	FVector CurrentHitLocation;
	FTimerManager* WorldTimerManager;
	APawn* PawnOwner;
	APlayerCameraManager* PawnCamera;
	unsigned int bUpdateDirection:1;
};
