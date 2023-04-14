// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Base/AN_TryDamagePlayer.h"

#include "Characters/Base/CharacterActionInterface.h"

void UAN_TryDamagePlayer::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	// valid check
	if(!MeshComp || !MeshComp->GetOwner()) return;

	// get owner actor reference
	AActor* OwnerRef = MeshComp->GetOwner();

	// check if owner class has character action interface
	if(OwnerRef->GetClass()->ImplementsInterface(UCharacterActionInterface::StaticClass()))
	{
		// if it has character action interface, it means its base character, execute its SwitchToIdleState function
		ICharacterActionInterface::Execute_TryToDamagePlayer(OwnerRef);
	}


}
