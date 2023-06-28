// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ToggleStrafeSpeed.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_ToggleStrafeSpeed : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_ToggleStrafeSpeed();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bShouldStrafe;

	// Toggles unit to use walking vs strafing speed
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
