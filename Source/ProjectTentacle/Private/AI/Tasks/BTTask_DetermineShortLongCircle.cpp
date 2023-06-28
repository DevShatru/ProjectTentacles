// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Tasks/BTTask_DetermineShortLongCircle.h"

#include "Kismet/KismetMathLibrary.h"


UBTTask_DetermineShortLongCircle::UBTTask_DetermineShortLongCircle()
{
	NodeName = "Determine Short Long Circle";

	//Ensure selected key is a bool
	BBKey_CircleRangeIsClose.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DetermineShortLongCircle, BBKey_CircleRangeIsClose));
}

EBTNodeResult::Type UBTTask_DetermineShortLongCircle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Grab necessary references 
	const UWorld* World = GetWorld();
	if(World == nullptr) return EBTNodeResult::Failed;
	
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();
	if(BlackBoard == nullptr) return EBTNodeResult::Failed;

	if(DetermineCloseOrLongCircle()) BlackBoard->SetValueAsBool(BBKey_CircleRangeIsClose.SelectedKeyName, true);
	else BlackBoard->SetValueAsBool(BBKey_CircleRangeIsClose.SelectedKeyName, false);
		
	
	return EBTNodeResult::Succeeded;
}

bool UBTTask_DetermineShortLongCircle::DetermineCloseOrLongCircle()
{
	return ChanceToBeCloseCircle > UKismetMathLibrary::RandomFloat();
}
