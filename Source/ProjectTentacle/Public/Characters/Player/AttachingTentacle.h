// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttachingTentacle.generated.h"

UCLASS()
class PROJECTTENTACLE_API AAttachingTentacle : public AActor
{
	GENERATED_BODY()

private:
	void SetAppearingMaterialInstance();


	
public:	
	// Sets default values for this actor's properties
	AAttachingTentacle();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* TentacleMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Materials)
	UMaterialInstanceDynamic* MaterialInstanceREF;

	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetMaterialValue(float Alpha);

	void SetTentacleInvisible();
};
