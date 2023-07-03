// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Player/AN_OnShowAttachingTentacle.h"

#include "Characters/Base/CharacterActionInterface.h"

void UAN_OnShowAttachingTentacle::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	// get owner actor reference
	AActor* OwnerRef = MeshComp->GetOwner();

	// check if owner class has character action interface
	if(OwnerRef->GetClass()->ImplementsInterface(UCharacterActionInterface::StaticClass()))
	{
		// if it has character action interface, it means its base character, execute its SwitchToIdleState function
		ICharacterActionInterface::Execute_OnMakingTentacleVisible(OwnerRef, true);
	}


}
