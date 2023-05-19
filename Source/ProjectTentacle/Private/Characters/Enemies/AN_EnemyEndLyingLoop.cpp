// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Enemies/AN_EnemyEndLyingLoop.h"

void UAN_EnemyEndLyingLoop::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	
	// valid check
	if(!MeshComp || !MeshComp->GetOwner()) return;

	// get owner actor reference
	APawn* PawnRef = Cast<APawn>(MeshComp->GetOwner());
	if(!PawnRef) return;
	PawnRef->bUseControllerRotationYaw = true;
}
