// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Base/AttackTargetTester.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

enum class EPlayerAttackType : uint8;

// Sets default values
AAttackTargetTester::AAttackTargetTester()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAttackTargetTester::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAttackTargetTester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAttackTargetTester::InstantRotation(FVector RotatingVector)
{
	const FRotator InputRotation = UKismetMathLibrary::MakeRotFromX(RotatingVector);

	SetActorRotation(InputRotation);
}

void AAttackTargetTester::PlayDamageReceiveAnimation(int32 AttackTypIndex)
{
	if(AttackTypIndex > 4) return;
	
	switch (AttackTypIndex)
	{
		case 0:
			PlayAnimMontage(ReceiveShortFlipKick,1,NAME_None);
			break;
		case 1:
			PlayAnimMontage(ReceiveFlyingKick,1,NAME_None);
			break;
		case 2:
			PlayAnimMontage(ReceiveFlyingPunch,1,NAME_None);
			break;
		case 3:
			PlayAnimMontage(ReceiveSpinKick,1,NAME_None);
			break;
		case 4:
			PlayAnimMontage(ReceiveDashingDoubleKick,1,NAME_None);
			break;
		default: break;
	}
}

void AAttackTargetTester::PlayFinishedAnimation()
{
	PlayAnimMontage(FinishedAnimation,1,NAME_None);

}

void AAttackTargetTester::TryToDamagePlayer_Implementation()
{
	ICharacterActionInterface::TryToDamagePlayer_Implementation();
	
	
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	const FVector HeadSocketLocation = GetMesh()->GetSocketLocation("Head");

	TArray<AActor*> FoundActorList;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	
	UKismetSystemLibrary::SphereOverlapActors(World, HeadSocketLocation, 120, FilterType, FilteringClass, IgnoreActors,FoundActorList);

	if(FoundActorList.Num() != 0)
	{
		for (AActor* EachFoundActor : FoundActorList)
		{
			IDamageInterface::Execute_ReceiveDamageFromEnemy(EachFoundActor, BaseDamageAmount, this, CurrentAttackType);
			//UGameplayStatics::ApplyDamage(EachFoundActor, 30, GetController(), this, DamageType);
		}
	}

}

void AAttackTargetTester::ReceiveDamageFromPlayer_Implementation(int32 DamageAmount, AActor* DamageCauser,
	EPlayerAttackType PlayerAttackType)
{
	IDamageInterface::ReceiveDamageFromPlayer_Implementation(DamageAmount, DamageCauser, PlayerAttackType);

	switch (PlayerAttackType)
	{
	case EPlayerAttackType::ShortFlipKick:
		PlayDamageReceiveAnimation(0);
		break;
	case EPlayerAttackType::FlyingKick:
		PlayDamageReceiveAnimation(1);
		break;
	case EPlayerAttackType::FlyingPunch:
		PlayDamageReceiveAnimation(2);
		break;
	case EPlayerAttackType::SpinKick:
		PlayDamageReceiveAnimation(3);
		break;
	case EPlayerAttackType::DashingDoubleKick:
		PlayDamageReceiveAnimation(4);
		break;
	default: ;
	}

}

// Called to bind functionality to input
void AAttackTargetTester::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

