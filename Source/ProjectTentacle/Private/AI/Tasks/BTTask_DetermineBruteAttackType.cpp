// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tasks/BTTask_DetermineBruteAttackType.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Base/EnumClassesForCharacters.h"
#include "Characters/Enemies/EnemyBrute.h"

UBTTask_DetermineBruteAttackType::UBTTask_DetermineBruteAttackType()
{
	NodeName = "Determine Brute Attack Type";

	AttackTypeKey.AddEnumFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_DetermineBruteAttackType, AttackTypeKey), StaticEnum<EBruteAttackType>());
}

EBTNodeResult::Type UBTTask_DetermineBruteAttackType::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* OwnController = OwnerComp.GetAIOwner();
	if(!OwnController) return EBTNodeResult::Failed;
	
	AEnemyBrute* OwnPawn = Cast<AEnemyBrute>(OwnController->GetPawn());
	if(!OwnPawn) return EBTNodeResult::Failed;

	EBruteAttackType DeterminedAttackType = OwnPawn->FindExecutingAttackType();
	OwnerComp.GetBlackboardComponent()->SetValueAsEnum(AttackTypeKey.SelectedKeyName, static_cast<uint8>(DeterminedAttackType));
	OwnPawn->SetExecutingAttackTypes(DeterminedAttackType);
	return EBTNodeResult::Succeeded;
}
