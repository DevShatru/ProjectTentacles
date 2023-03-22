// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Enemies/EnemyBaseController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"


AEnemyBaseController::AEnemyBaseController()
{
	bAttachToPawn = true;
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
}

void AEnemyBaseController::BeginPlay()
{
	Super::BeginPlay();
	
	if(!BehaviorTree) return;
	Blackboard->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	RunBehaviorTree(BehaviorTree);
}