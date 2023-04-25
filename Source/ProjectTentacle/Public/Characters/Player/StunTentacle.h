// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/BaseTentacle.h"
#include "StunTentacle.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTTENTACLE_API AStunTentacle : public ABaseTentacle
{
	GENERATED_BODY()


protected:

	FTimerHandle PrepareStunTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeToExecuteStun = 5.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AbsorbingRadius = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AbsorbingPower = 1000.0f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	TArray<TEnumAsByte<EObjectTypeQuery>> FilterType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= Attack_Setting)
	UClass* FilteringClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunningRadius = 250.0f;


	virtual void LifeCycleBegin() override;

	UFUNCTION()
	void StunEnemy();

	virtual void OnLifeCycleEnd() override;	

public:
	
	
};
