// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_FinishRepositioning.h"

#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_FinishRepositioning::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	UBlackboardComponent* OwnerBBComp = OwnerComp.GetBlackboardComponent();

	if(!OwnerBBComp) return EBTNodeResult::Failed;

	OwnerBBComp->SetValueAsBool(BBKey_bNeedToReposition.SelectedKeyName, false);

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Repositioning Done!"));	

	
	return EBTNodeResult::Succeeded;
}

UBTTask_FinishRepositioning::UBTTask_FinishRepositioning()
{
	NodeName = "Finish Repositioning";
}

