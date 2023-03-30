// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_QueueForAttack.h"
#include "Characters/Enemies/EnemyBaseController.h"

UBTTask_QueueForAttack::UBTTask_QueueForAttack()
{
	NodeName = "Queue for Attack";

	BlackboardKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_QueueForAttack, BlackboardKey));
}

EBTNodeResult::Type UBTTask_QueueForAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AEnemyBaseController* OwnController = Cast<AEnemyBaseController>(OwnerComp.GetAIOwner());

	if(!OwnController) return EBTNodeResult::Failed;

	OwnController->RegisterOnAttackQueue();
	return EBTNodeResult::Succeeded;
}
