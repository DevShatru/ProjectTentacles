// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Decorators/BTDecorator_IsOnNavMesh.h"

#include "AIController.h"
#include "NavigationSystem.h"

UBTDecorator_IsOnNavMesh::UBTDecorator_IsOnNavMesh()
{
	NodeName = "IsOnNavMesh?";
}

bool UBTDecorator_IsOnNavMesh::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UNavigationSystemV1* NavSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	if(!NavSystem) return false;

	const AAIController* Controller = OwnerComp.GetAIOwner();
	if(!Controller) return false;

	const APawn* Pawn = Controller->GetPawn();
	if(!Pawn) return false;

	FNavLocation OutLocation;
	return NavSystem->ProjectPointToNavigation(Pawn->GetActorLocation(), OutLocation);
}
