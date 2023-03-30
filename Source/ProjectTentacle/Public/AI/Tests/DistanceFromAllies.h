// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "DistanceFromAllies.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UDistanceFromAllies : public UEnvQueryTest
{
	GENERATED_BODY()
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
	virtual FText GetDescriptionDetails() const override;
	virtual FText GetDescriptionTitle() const override;
};
