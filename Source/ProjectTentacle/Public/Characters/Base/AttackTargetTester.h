// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterActionInterface.h"
#include "DamageInterface.h"
#include "GameFramework/Character.h"
#include "AttackTargetTester.generated.h"



UCLASS()
class PROJECTTENTACLE_API AAttackTargetTester : public ACharacter , public ICharacterActionInterface, public IDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAttackTargetTester();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ReceiveDamageAnimations)
	UAnimMontage* ReceiveShortFlipKick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ReceiveDamageAnimations)
	UAnimMontage* ReceiveFlyingKick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ReceiveDamageAnimations)
	UAnimMontage* ReceiveFlyingPunch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ReceiveDamageAnimations)
	UAnimMontage* ReceiveSpinKick;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ReceiveDamageAnimations)
	UAnimMontage* ReceiveDashingDoubleKick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FinishedAnimations)
	UAnimMontage* FinishedAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnemyProperty)
	int32 Health = 3;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting)
	TArray<TEnumAsByte<EObjectTypeQuery>> FilterType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting)
	UClass* FilteringClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting)
	TSubclassOf<UDamageType> DamageType;

	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting)
	EEnemyAttackType CurrentAttackType;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category= AttackSetting)
	int32 BaseDamageAmount = 2;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void InstantRotation(FVector RotatingVector);

	void PlayDamageReceiveAnimation(int32 AttackTypIndex);

	void PlayFinishedAnimation();


	int32 GetEnemyHealth() const { return Health;}
	void SetEnemyHealth(int32 NewHealth) {Health = NewHealth;}

	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
	virtual void TryToDamagePlayer_Implementation() override;

	virtual void ReceiveDamageFromPlayer_Implementation(int32 DamageAmount, AActor* DamageCauser, EPlayerAttackType PlayerAttackType) override;
	
	

	
	// ==================================== Testing Functions ======================================================

	
};

