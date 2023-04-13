// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Base/ANS_PlayerCounterChecking.h"

#include "Characters/Base/CharacterActionInterface.h"

void UANS_PlayerCounterChecking::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                            float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

	// valid check
	if(!MeshComp || !MeshComp->GetOwner()) return;

	// get owner actor reference
	AActor* OwnerRef = MeshComp->GetOwner();

	// check if owner class has character action interface
	if(OwnerRef->GetClass()->ImplementsInterface(UCharacterActionInterface::StaticClass()))
	{
		// if it has character action interface, it means its base character, execute its TryTriggerPlayerCounter function
		ICharacterActionInterface::Execute_TryTriggerPlayerCounter(OwnerRef);
	}
	







}
