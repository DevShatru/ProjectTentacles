// Fill out your copyright notice in the Description page of Project Settings.


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
