// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Player/AN_OnSwitchBackToDefaultCamera.h"

#include "Characters/Player/PlayerCameraInterface.h"

void UAN_OnSwitchBackToDefaultCamera::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	
	// get owner actor reference
	AActor* OwnerRef = MeshComp->GetOwner();

	// check if owner class has character action interface
	if(OwnerRef->GetClass()->ImplementsInterface(UPlayerCameraInterface::StaticClass()))
	{
		// if it has character action interface, it means its base character, execute its SwitchToIdleState function
		IPlayerCameraInterface::Execute_OnSwitchingBackToDefaultCamera(OwnerRef);
	}

}
