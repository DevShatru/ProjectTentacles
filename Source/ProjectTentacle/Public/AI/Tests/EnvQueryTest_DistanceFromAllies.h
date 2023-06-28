// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_DistanceFromAllies.generated.h"

/**
 * UNUSED & UNFUNCTIONAL
 * Tried to create an EQS test to determine a unit's distance from it's nearest allies
 * Abandoned because this was more straightforward to do, if less performant, with an overlap sphere
 */
UCLASS()
class PROJECTTENTACLE_API UEnvQueryTest_DistanceFromAllies : public UEnvQueryTest
{
	GENERATED_BODY()
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
	virtual FText GetDescriptionDetails() const override;
	virtual FText GetDescriptionTitle() const override;
};
