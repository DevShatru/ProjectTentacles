// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Enemies/Tasks/FindPatrolPoint.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UFindPatrolPoint::UFindPatrolPoint()
{
	NodeName = "Find Patrol Point";

	//Ensure selected key is a vector
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UFindPatrolPoint, BlackboardKey));
}

EBTNodeResult::Type UFindPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Cache NavSystem Reference
	NavSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	if(!NavSystem) return EBTNodeResult::Failed;

	// Fail if owner doesn't exist
	const AActor* Owner = OwnerComp.GetOwner();
	if(!Owner) return EBTNodeResult::Failed;

	// Fail if we can't find a location
	FNavLocation PatrolPoint;
	NavSystem->GetRandomReachablePointInRadius(Owner->GetActorLocation(), FindRadius, PatrolPoint);
	if (PatrolPoint.Location == FVector::ZeroVector) return EBTNodeResult::Failed;

	// Save location to blackboard
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, PatrolPoint.Location);
	return EBTNodeResult::Succeeded;
}