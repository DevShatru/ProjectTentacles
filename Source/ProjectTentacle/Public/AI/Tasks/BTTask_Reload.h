// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "BTTask_Reload.generated.h"

/**
 * Wait task for the Ranged unit's reload
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_Reload : public UBTTask_Wait
{
	GENERATED_BODY()
public:
	UBTTask_Reload();
};
