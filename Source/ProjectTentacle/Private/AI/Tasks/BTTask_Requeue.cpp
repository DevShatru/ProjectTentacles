// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_Requeue.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Enemies/EnemyBaseController.h"

UBTTask_Requeue::UBTTask_Requeue()
{
	NodeName = "Requeue";

	IsQueuedKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_Requeue, IsQueuedKey));
	IsAttackingKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_Requeue, IsAttackingKey));
}

EBTNodeResult::Type UBTTask_Requeue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* OwnBlackboard = OwnerComp.GetBlackboardComponent();
	if(!OwnBlackboard) return EBTNodeResult::Failed;
	
	AEnemyBaseController* OwnController = Cast<AEnemyBaseController>(OwnerComp.GetAIOwner());
	if(!OwnController) return EBTNodeResult::Failed;

	// Register with encounter that attack has been completed, allowing it to select next attacker
	OwnController->RegisterCompletedAttack();

	// Clear flags
	OwnBlackboard->ClearValue(IsQueuedKey.SelectedKeyName);
	OwnBlackboard->ClearValue(IsAttackingKey.SelectedKeyName);
	return EBTNodeResult::Succeeded;
}
