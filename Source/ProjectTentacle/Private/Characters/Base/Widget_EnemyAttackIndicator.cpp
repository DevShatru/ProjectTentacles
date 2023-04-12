// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Base/Widget_EnemyAttackIndicator.h"

void UWidget_EnemyAttackIndicator::OnReceivingNewAttackType(EEnemyAttackType NewAttackType, EEnemyAttackAnimMontages NewAnimMontage)
{
	CurrentPlayingAnimMontage = NewAnimMontage;
}

void UWidget_EnemyAttackIndicator::StoreWidgetAnimation()
{
	// Clear map first
	WidgetAnimMap.Empty();

	// Get Uproperty class reference as start point
	FProperty* Prop = GetClass()->PropertyLink;

	// while the property is not nullptr
	while (Prop)
	{
		// continue if if Property Class is FObjectProperty
		if(Prop->GetClass() == FProperty::StaticClass())
		{
			// Cast propert to FObjectProperty class
			FObjectProperty* ObjectProp = CastField<FObjectProperty>(Prop);

			// Check if property class is UWidgetAnimation class
			if (ObjectProp->PropertyClass == UWidgetAnimation::StaticClass())
			{
				// Get Uobject in this property container and cast it to UWidgetAnimation pointer
				UObject* ObjectInContainer = ObjectProp->GetObjectPropertyValue_InContainer(this);
				UWidgetAnimation* WidgetAnimation = Cast<UWidgetAnimation>(ObjectInContainer);

				// Validation check to casted result
				if (WidgetAnimation && WidgetAnimation->MovieScene)
				{
					// get FName and pointer and allocate into WidgetAnimMap
					const FName AnimName = WidgetAnimation->MovieScene->GetFName();
					WidgetAnimMap.Add(AnimName, WidgetAnimation);
				}
			}
		}

		// cheange to next property
		Prop = Prop->PropertyLinkNext;
	}
	
	
}

UWidgetAnimation* UWidget_EnemyAttackIndicator::GetAnimationByName(FName AnimationName)
{
	UWidgetAnimation* const* WidgetAnimation = WidgetAnimMap.Find(AnimationName);

	// return Widget Animation reference when it is valid, else, return nullptr
	if(WidgetAnimation) return *WidgetAnimation;
		
	return nullptr;
}

void UWidget_EnemyAttackIndicator::TryPlayIndicationAnimation()
{
	StopAllAnimations();
	
	switch (CurrentPlayingAnimMontage)
	{
		case EEnemyAttackAnimMontages::MMAKick:
			if(YellowToRedAnim_MMAKick)
				PlayAnimation(YellowToRedAnim_MMAKick, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, true);
			break;
		
		case EEnemyAttackAnimMontages::HeadButt:
			if(AllGreenAnim_HeadButt)
				PlayAnimation(AllGreenAnim_HeadButt, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, true);
			break;
		default: break;
	}
}

void UWidget_EnemyAttackIndicator::NativeConstruct()
{
	Super::NativeConstruct();

	StoreWidgetAnimation();

	YellowToRedAnim_MMAKick = GetAnimationByName(TEXT("YellowToRedAnim_MMAKick"));
	AllGreenAnim_HeadButt = GetAnimationByName(TEXT("AllGreenAnim_HeadButt"));
}

void UWidget_EnemyAttackIndicator::ShowIndicator()
{
	Super::ShowIndicator();
	
	TryPlayIndicationAnimation();
}
