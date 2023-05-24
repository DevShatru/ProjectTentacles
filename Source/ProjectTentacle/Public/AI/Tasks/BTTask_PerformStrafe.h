// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PerformStrafe.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API UBTTask_PerformStrafe : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_PerformStrafe();
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
protected:
	UFUNCTION()
	void Interrupt();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Strafe")
	bool bUseStrafeSpeed = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Strafe")
	int32 StrafeLoopsAttempted = 10;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Strafe")
	float DistancePerLoop = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Strafe")
	FBlackboardKeySelector TargetKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Strafe")
	FBlackboardKeySelector CircleDirectionKey;

private:
	void CompleteStrafe(UBehaviorTreeComponent* BTComp, EBTNodeResult::Type Result) const;
	FVector GetStrafeDirection() const;
	
	class AEnemyBase* OwnPawn;
	FVector StrafeLoopStartLocation;
	FVector StrafeLoopDirection;
	float LoopDistSqr;
	int8 LoopIterationsComplete;
	UBehaviorTreeComponent* OwningComponent;
	unsigned int bStrafeRight:1, bIsTargetActor:1;
	AActor* StrafeActorTarget;
	FVector StrafeLocationTarget;
	
};
