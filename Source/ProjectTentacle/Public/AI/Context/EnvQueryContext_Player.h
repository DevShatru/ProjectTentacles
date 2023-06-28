// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_Player.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UEnvQueryContext_Player : public UEnvQueryContext
{
	GENERATED_BODY()

protected:
	// EQS context to provide a reference to the PC
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
	virtual void BeginDestroy() override;

private:
	static AActor* Player;
};
