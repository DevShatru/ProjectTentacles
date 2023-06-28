// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsOnNavMesh.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UBTDecorator_IsOnNavMesh : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_IsOnNavMesh();

protected:
	// Check if owning pawn's current location can be projected onto the nav mesh
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
