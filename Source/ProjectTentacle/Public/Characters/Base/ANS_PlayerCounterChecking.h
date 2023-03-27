// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_PlayerCounterChecking.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UANS_PlayerCounterChecking : public UAnimNotifyState
{
	GENERATED_BODY()
protected:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	
};
