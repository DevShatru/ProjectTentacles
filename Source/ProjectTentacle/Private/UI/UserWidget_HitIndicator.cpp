// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "UI/UserWidget_HitIndicator.h"

#include "Components/Image.h"
#include "Kismet/KismetMaterialLibrary.h"

void UUserWidget_HitIndicator::NativeConstruct()
{
	Super::NativeConstruct();
	HitIndicator->SetVisibility(ESlateVisibility::Collapsed);
	HitIndicatorDynamicInst = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), HitIndicatorMaterial);
	HitIndicator->SetBrushFromMaterial(HitIndicatorDynamicInst);
}

void UUserWidget_HitIndicator::NativeDestruct()
{
	Super::NativeDestruct();
}
