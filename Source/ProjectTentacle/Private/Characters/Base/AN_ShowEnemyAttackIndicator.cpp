// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Base/AN_ShowEnemyAttackIndicator.h"

#include "Characters/Base/EnemyWidgetInterface.h"

void UAN_ShowEnemyAttackIndicator::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	// valid check
	if(!MeshComp || !MeshComp->GetOwner()) return;

	// get owner actor reference
	AActor* OwnerRef = MeshComp->GetOwner();

	// check if owner class has character action interface
	if(OwnerRef->GetClass()->ImplementsInterface(UEnemyWidgetInterface::StaticClass()))
	{
		// if it has character action interface, it means its base character, execute its SwitchToIdleState function
		IEnemyWidgetInterface::Execute_ShowEnemyAttackIndicator(OwnerRef);
	}	

}
