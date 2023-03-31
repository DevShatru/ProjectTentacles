// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.

#pragma once


#include "CoreMinimal.h"
//#include "CharacterActionInterface.h"
#include "CharacterActionInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UENUM(BlueprintType)
enum class EActionState: uint8
{
	Idle = 0 UMETA(DisplayName = "IDLE"),
	Evade = 1 UMETA(DisplayName = "EVADE"),
	Attack = 2 UMETA(DisplayName = "ATTACK"),
	Recovering = 3 UMETA(DisplayName = "RECOVERING"),
	ReceivedDamage = 4 UMETA(DisplayName = "RECEIVEDDAMAGE"),
	SpecialAttack = 5 UMETA(DisplayName = "SPECIALATTACK"),
	WaitForCombo = 6 UMETA(DisplayName = "WAITFORCOMBO"),
	Dodge = 7 UMETA(DisplayName = "DODGE")
};


UCLASS()
class PROJECTTENTACLE_API ABaseCharacter : public ACharacter , public ICharacterActionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CharacterProperty)
	EActionState CurrentActionState = EActionState::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CharacterProperty)
	float CharacterCurrentHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CharacterProperty)
	float CharacterMaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* CurrentPlayingMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AnimMontages)
	UAnimMontage* EvadeAnimMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AnimMontages)
	TArray<UAnimMontage*> MeleeAttackMontages;

	UPROPERTY()
	ACharacter* TargetCharacter;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	ACharacter* GetCurrentTarget() const {return TargetCharacter;}
	void SetCurrentTarget(ACharacter* Target) {TargetCharacter = Target;}
	
	// ================================================= Interface implementation =========================================
	
	virtual void ActionEnd_Implementation(bool BufferingCheck) override;

	virtual void StartWaitForCombo_Implementation() override;
	
	
};
