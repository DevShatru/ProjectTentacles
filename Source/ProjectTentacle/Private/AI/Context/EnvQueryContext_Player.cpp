// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Context/EnvQueryContext_Player.h"

#include "EngineUtils.h"
#include "Characters/Player/PlayerCharacter.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

AActor* UEnvQueryContext_Player::Player = nullptr;
TArray<UEnvQueryContext_Player*> UEnvQueryContext_Player::AllPlayerContexts = TArray<UEnvQueryContext_Player*>();

UEnvQueryContext_Player::UEnvQueryContext_Player() : Super()
{
	AllPlayerContexts.Add(this);
}

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

void UEnvQueryContext_Player::BeginDestroy()
{
	AllPlayerContexts.Remove(this);
	if(AllPlayerContexts.Num() == 0)
	{
		Player = nullptr;
	}
	
	Super::BeginDestroy();
}
