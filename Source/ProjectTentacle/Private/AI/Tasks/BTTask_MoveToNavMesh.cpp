// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_MoveToNavMesh.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"

UBTTask_MoveToNavMesh::UBTTask_MoveToNavMesh()
{
	NodeName = "MoveToNavMesh";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_MoveToNavMesh::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	NavSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	Controller = OwnerComp.GetAIOwner();
	Character = Controller->GetCharacter();
	
	return EBTNodeResult::InProgress;
}

void UBTTask_MoveToNavMesh::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const FVector CurrentLocation = Character->GetActorLocation();

	if (TargetLocation == FVector::ZeroVector)
	{
		FNavLocation NearestPoint;
		NavSystem->ProjectPointToNavigation(CurrentLocation, NearestPoint);
		TargetLocation = NearestPoint.Location;
		TargetDirection = (TargetLocation - CurrentLocation).GetSafeNormal();
	}

	if(FVector::DistSquared(CurrentLocation, TargetLocation) <= Tolerance * Tolerance)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	Character->AddMovementInput(TargetDirection, 1.f);
}
