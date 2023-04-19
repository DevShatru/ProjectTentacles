// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Decorators/BTDecorator_CanMoveTo.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

void UBTDecorator_CanMoveTo::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (BBAsset)
	{
		LocationBlackboardKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("Can't initialize task: %s, make sure that behavior tree specifies blackboard asset!"), *GetName());
	}
}

UBTDecorator_CanMoveTo::UBTDecorator_CanMoveTo()
{
	NodeName = "Can Move To";
	//Ensure selected key is a vector
	LocationBlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_CanMoveTo, LocationBlackboardKey), AActor::StaticClass());
	LocationBlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_CanMoveTo, LocationBlackboardKey));
}

bool UBTDecorator_CanMoveTo::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	FVector MoveToLocation;
	if(LocationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		const AActor* AsActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(LocationBlackboardKey.SelectedKeyName));
		if(!AsActor) return false;
		MoveToLocation = AsActor->GetActorLocation();
	}
	
	if(LocationBlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		MoveToLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(LocationBlackboardKey.SelectedKeyName);
	}
	UNavigationSystemV1* NavSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	if(!NavSystem) return false;
	FNavLocation OutLocation;
	const bool bResult = NavSystem->ProjectPointToNavigation(MoveToLocation, OutLocation);
	return bResult;
}
