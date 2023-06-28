// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_ToggleStrafeSpeed.h"

#include "Characters/Enemies/EnemyBase.h"

UBTTask_ToggleStrafeSpeed::UBTTask_ToggleStrafeSpeed()
{
	NodeName = "Toggle Strafe Speed";
}

// Toggles unit to use walking vs strafing speed
EBTNodeResult::Type UBTTask_ToggleStrafeSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* Controller = OwnerComp.GetAIOwner();
	if(!Controller) return EBTNodeResult::Failed;
	
	const AEnemyBase* Unit = Cast<AEnemyBase>(Controller->GetPawn());
	if(!Unit) return EBTNodeResult::Failed;
	
	Unit->EnableStrafe(bShouldStrafe);
	return EBTNodeResult::Succeeded;
}
