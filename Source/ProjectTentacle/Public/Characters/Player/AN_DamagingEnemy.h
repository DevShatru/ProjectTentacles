// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_DamagingEnemy.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UAN_DamagingEnemy : public UAnimNotify
{
	GENERATED_BODY()
protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
