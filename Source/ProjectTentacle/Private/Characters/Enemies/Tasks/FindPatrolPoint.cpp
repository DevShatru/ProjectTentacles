// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemies/Tasks/FindPatrolPoint.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

//Static defaults
unsigned int UFindPatrolPoint::bAttemptNavCache = 0;
UNavigationSystemV1* UFindPatrolPoint::NavSystem = nullptr;

UFindPatrolPoint::UFindPatrolPoint()
{
	NodeName = "Find Patrol Point";

	//Ensure selected key is a vector
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UFindPatrolPoint, BlackboardKey));
}

EBTNodeResult::Type UFindPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(!NavSystem)
	{
		// If NavSystem isn't cached, check if caching has been attempted and fail if it has
		if(bAttemptNavCache) return EBTNodeResult::Failed;

		// Otherwise, attempt cache and flip bit flag
		NavSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
		bAttemptNavCache = 1;
		if(!NavSystem) return EBTNodeResult::Failed;
	}

	// Fail if owner doesn't exist
	const AActor* Owner = OwnerComp.GetOwner();
	if(!Owner) return EBTNodeResult::Failed;

	// Fail if we can't find a location
	FNavLocation PatrolPoint;
	NavSystem->GetRandomReachablePointInRadius(Owner->GetActorLocation(), FindRadius, PatrolPoint);
	if(PatrolPoint.Location == FVector::ZeroVector) return EBTNodeResult::Failed;

	// Save location to blackboard
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(BlackboardKey.SelectedKeyName, PatrolPoint.Location);
	return EBTNodeResult::Succeeded;
}
