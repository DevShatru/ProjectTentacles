// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "UI/UserWidget_HitIndicator.h"

#include "Components/Image.h"

void UUserWidget_HitIndicator::PopIndicator(const float TransitionTime, const FVector HitLocation)
{
	bUpdateDirection = true;
	HitIndicator->SetRenderOpacity(1.f);
	HitIndicator->SetVisibility(ESlateVisibility::HitTestInvisible);
	DisplayStartTime = GetWorld()->GetTimeSeconds();
	ColorTransitionTime = TransitionTime;
	CurrentHitLocation = HitLocation;
	WorldTimerManager->SetTimerForNextTick(this, &UUserWidget_HitIndicator::UpdateIndicatorColor);
	WorldTimerManager->SetTimerForNextTick(this, &UUserWidget_HitIndicator::UpdateIndicatorLocation);
	PlayAnimationForward(ShowHitIndicator);
}

void UUserWidget_HitIndicator::CollapseIndicator()
{
	PlayAnimationForward(HideHitIndicator);
}


void UUserWidget_HitIndicator::NativeConstruct()
{
	Super::NativeConstruct();
	World = GetWorld();
	WorldTimerManager = &World->GetTimerManager();
	PawnOwner = GetOwningPlayerPawn();
	PawnCamera = GetOwningPlayerCameraManager();
	HideIndicator();
	// Create Dynamic material
	HitIndicatorDynamicInst = UMaterialInstanceDynamic::Create(HitIndicatorMaterial, World, NAME_None);
	HitIndicator->SetBrushFromMaterial(HitIndicatorDynamicInst);
	HideAnimationDelegate.BindUFunction(this, "HideIndicator");
	BindToAnimationFinished(HideHitIndicator, HideAnimationDelegate);
}

void UUserWidget_HitIndicator::NativeDestruct()
{
	UnbindFromAnimationFinished(HideHitIndicator, HideAnimationDelegate);
	Super::NativeDestruct();
}

void UUserWidget_HitIndicator::UpdateIndicatorLocation()
{
	const FVector PawnLocation = PawnOwner->GetActorLocation();
	const FVector CameraForward = PawnCamera->GetActorForwardVector();
	const FVector HitToPawnDirection= (PawnLocation - CurrentHitLocation).GetSafeNormal();
	const float ForwardDotLocationToCamera = FVector::DotProduct(CameraForward, HitToPawnDirection);
	const float RawAngleInDegrees = (180.f)/PI * FMath::Atan2(CameraForward.X * HitToPawnDirection.Y - CameraForward.Y * HitToPawnDirection.X, ForwardDotLocationToCamera);
	const float ClampedAngle =  FMath::GetMappedRangeValueClamped(FVector2D(180.f, -180.f), FVector2D(0, 1), RawAngleInDegrees);

	HitIndicatorDynamicInst->SetScalarParameterValue("Angle", ClampedAngle);
	
	if(bUpdateDirection) WorldTimerManager->SetTimerForNextTick(this, &UUserWidget_HitIndicator::UpdateIndicatorLocation);
}

void UUserWidget_HitIndicator::UpdateIndicatorColor()
{
	const float ElapsedTime = World->GetTimeSeconds() - DisplayStartTime;
	HitIndicatorDynamicInst->SetVectorParameterValue("Color", FLinearColor::LerpUsingHSV(StartColor, FinalColor, ElapsedTime/ColorTransitionTime));
	if (ElapsedTime < ColorTransitionTime) WorldTimerManager->SetTimerForNextTick(this, &UUserWidget_HitIndicator::UpdateIndicatorColor);
}

void UUserWidget_HitIndicator::HideIndicator()
{
	HitIndicator->SetVisibility(ESlateVisibility::Collapsed);
	bUpdateDirection = false;
}
