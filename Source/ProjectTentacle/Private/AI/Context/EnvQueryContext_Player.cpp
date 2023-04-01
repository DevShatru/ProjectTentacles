// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Context/EnvQueryContext_Player.h"

#include "EngineUtils.h"
#include "Characters/Player/PlayerCharacter.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

AActor* UEnvQueryContext_Player::Player = nullptr;

void UEnvQueryContext_Player::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	if(!Player->IsValidLowLevelFast())
	{
		Player = nullptr;
		for (TActorIterator<APlayerCharacter> It(GetWorld(), APlayerCharacter::StaticClass()); It; ++It)
		{
			Player = *It;
		}
	}
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, Player);
}
