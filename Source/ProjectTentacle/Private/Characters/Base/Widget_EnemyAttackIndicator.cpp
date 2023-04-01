// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Base/Widget_EnemyAttackIndicator.h"

void UWidget_EnemyAttackIndicator::OnReceivingNewAttackType(EEnemyAttackType NewAttackType)
{
	FLinearColor LinearColor = FLinearColor(0,0,0,1);

	LinearColor.R = 1.0f;
	
	if(NewAttackType == EEnemyAttackType::AbleToCounter)
	{
		LinearColor.G = 1.0f;
	}
	
	IndicationImage->Brush.TintColor = FSlateColor(LinearColor);
}
