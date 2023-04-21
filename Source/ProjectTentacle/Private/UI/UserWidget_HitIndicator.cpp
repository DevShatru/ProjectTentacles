// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "UI/UserWidget_HitIndicator.h"

#include "Components/Image.h"
#include "Kismet/KismetMaterialLibrary.h"

void UUserWidget_HitIndicator::PopIndicator(float TransitionTime, FVector HitLocation)
{
	HitIndicator->SetVisibility(ESlateVisibility::HitTestInvisible);
	PlayAnimationForward(ShowHitIndicator);
}

void UUserWidget_HitIndicator::CollapseIndicator()
{
	PlayAnimationForward(HideHitIndicator);
}


void UUserWidget_HitIndicator::NativeConstruct()
{
	Super::NativeConstruct();
	HideIndicator();
	HitIndicatorDynamicInst = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), HitIndicatorMaterial);
	HitIndicator->SetBrushFromMaterial(HitIndicatorDynamicInst);
	HideAnimationDelegate.BindUFunction(this, "HideIndicator");
	BindToAnimationFinished(HideHitIndicator, HideAnimationDelegate);
}

void UUserWidget_HitIndicator::NativeDestruct()
{
	UnbindFromAnimationFinished(HideHitIndicator, HideAnimationDelegate);
	Super::NativeDestruct();
}

void UUserWidget_HitIndicator::HideIndicator()
{
	HitIndicator->SetRenderOpacity(1.f);
	HitIndicator->SetVisibility(ESlateVisibility::Collapsed);
}
