// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "AI/Tests/DistanceFromAllies.h"

#include "Characters/Enemies/EnemyBase.h"
#include "Characters/Enemies/EnemyBaseController.h"

void UDistanceFromAllies::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* OwnerAsObject = QueryInstance.Owner.Get();
	if(!OwnerAsObject) return;

	const AEnemyBaseController* Owner = Cast<AEnemyBaseController>(OwnerAsObject);
	if(!Owner) return;

	TArray<AEnemyBase*> Allies = Owner->GetAllies();
	
	float NearestSquareDistance = INFINITY;
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		const FVector ItemLocation = GetItemLocation(QueryInstance, It.GetIndex());
		for(const AEnemyBase* Ally : Allies)
		{
			NearestSquareDistance = INFINITY;
			const float CurrentSquareDistance = FVector::DistSquared2D(ItemLocation, Ally->GetActorLocation());
			if(CurrentSquareDistance < NearestSquareDistance)
			{
				NearestSquareDistance = CurrentSquareDistance;
			}
		}
		It.SetScore(TestPurpose, FilterType, NearestSquareDistance, 0.f, 0.f);
	}
}

FText UDistanceFromAllies::GetDescriptionDetails() const
{
	return FText::FromString("Calculates square distance from all allies of the querier in the same encounter");
}

FText UDistanceFromAllies::GetDescriptionTitle() const
{
	return FText::FromString("Distance from Allies");
}
