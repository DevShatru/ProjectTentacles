// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Player/AN_OnUnableToCancelAttack.h"

#include "Characters/Base/CharacterActionInterface.h"

void UAN_OnUnableToCancelAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	// valid check
	if(!MeshComp || !MeshComp->GetOwner()) return;

	// get owner actor reference
	AActor* OwnerRef = MeshComp->GetOwner();

	// check if owner class has character action interface
	if(OwnerRef->GetClass()->ImplementsInterface(UCharacterActionInterface::StaticClass()))
	{
		// if it has UEnemyRangeInterface, it means its Ranged character, execute its OnRifleBeginAiming function
		ICharacterActionInterface::Execute_EnterUnableCancelAttack(OwnerRef);
	}
}
