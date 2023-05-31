// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Base/AttackTargetTester.h"

#include "Characters/Player/PlayerDamageInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

enum class EPlayerAttackType : uint8;

// Sets default values
AAttackTargetTester::AAttackTargetTester()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	EnemyAttackIndicatorWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT( "EnemyAttackIndicatorWidget" ));
	EnemyTargetedIconWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT( "EnemyTargetedIconWidget" ));
	EnemyAttackIndicatorWidgetComponent->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	EnemyTargetedIconWidgetComponent->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AAttackTargetTester::BeginPlay()
{
	Super::BeginPlay();
	UUserWidget* ReturnAttackIndicatorWidget = EnemyAttackIndicatorWidgetComponent->GetWidget();
	UWidget_EnemyAttackIndicator* CastedAttackIndicatorWidget = Cast<UWidget_EnemyAttackIndicator>(ReturnAttackIndicatorWidget);
	
	if(CastedAttackIndicatorWidget)
	{
		AttackIndicatorRef = CastedAttackIndicatorWidget;
		//OnUpdatingEnemyAttackIndicator.BindDynamic(AttackIndicatorRef, &UWidget_EnemyAttackIndicator::OnReceivingNewAttackType);
		AttackIndicatorRef->UnShowIndicator();
	}

	UUserWidget* ReturnTargetedIconWidget = EnemyTargetedIconWidgetComponent->GetWidget();
	UWidget_EnemyTargetIconWidget* CastedTargetedIconWidget = Cast<UWidget_EnemyTargetIconWidget>(ReturnTargetedIconWidget);

	if(CastedTargetedIconWidget)
	{
		EnemyTargetWidgetRef = CastedTargetedIconWidget;
		EnemyTargetWidgetRef->UnShowIndicator();
	}

	// timeline binding
	FOnTimelineFloat MovingAttackPosUpdate;
	MovingAttackPosUpdate.BindDynamic(this, &AAttackTargetTester::UpdateAttackingPosition);
	AttackMovingTimeline.AddInterpFloat(AttackMovingCurve, MovingAttackPosUpdate);
	
}

void AAttackTargetTester::ExecuteAttack()
{
	// SetAttackType and get the result enum of it
	EEnemyAttackAnimMontages ResultEnemyAnimMontage = SetAttackType();

	
	if(AttackIndicatorRef)
	{
		// execute delegate function to update variables in Indicator widget class
		OnUpdatingEnemyAttackIndicator.Execute(CurrentAttackType, ResultEnemyAnimMontage);
	}

	// Save lerp start and end position for later timeline function usage
	SelfAttackStartPos = GetActorLocation();

	// Get Player Position
	const ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	FVector PlayerCurrentPos = PlayerCharacter->GetActorLocation();
	
	const FVector DestinationPos = CalculateDestinationForAttackMoving(PlayerCurrentPos);
	AttackMovingDestination = DestinationPos;
	
	// switch case on current attack type to fire different animation 
	switch (CurrentAttackType)
	{
		case EEnemyAttackType::AbleToCounter:
			if(CounterableAttackMontage == nullptr) return;
			PlayAnimMontage(CounterableAttackMontage, 1, "Default");
			AttackMovingTimeline.PlayFromStart();
			break;
		case EEnemyAttackType::UnableToCounter:
			if(NotCounterableAttackMontage == nullptr) return;
			PlayAnimMontage(NotCounterableAttackMontage, 1, "Default");
			AttackMovingTimeline.PlayFromStart();
			break;
		default: break;
	}

}

EEnemyAttackAnimMontages AAttackTargetTester::SetAttackType()
{
	int32 CounterableRndInt = UKismetMathLibrary::RandomInteger(2);

	if(CounterableRndInt == 0)
	{
		CurrentAttackType = EEnemyAttackType::UnableToCounter;
		return EEnemyAttackAnimMontages::MMAKick;
	}
	
	CurrentAttackType = EEnemyAttackType::AbleToCounter;
	return EEnemyAttackAnimMontages::HeadButt;
}


FVector AAttackTargetTester::CalculateDestinationForAttackMoving(FVector PlayerPos)
{
	// Get direction from self to player
	FVector OffsetWithoutZ = PlayerPos - SelfAttackStartPos;
	OffsetWithoutZ.Z = 0;
	const FVector DirFromSelfToPlayer = UKismetMathLibrary::Normal(OffsetWithoutZ);

	const FVector SupposedAttackMovingDestination = SelfAttackStartPos + (DirFromSelfToPlayer * AttackMovingDistance);

	
	// Hit result
	FHitResult Hit;
	// Empty array of ignoring actor, maybe add Enemies classes to be ignored
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	
	// Capsule trace by channel
	const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, SelfAttackStartPos, SupposedAttackMovingDestination, UEngineTypes::ConvertToTraceType(ECC_Visibility),false, IgnoreActors,  EDrawDebugTrace::Persistent,Hit,true);

	if(!bHit) return SupposedAttackMovingDestination;

	const FVector DirFromPlayerToSelf = DirFromSelfToPlayer * -1;
	
	return Hit.ImpactPoint + (DirFromPlayerToSelf * OffsetFromPlayer);
}

// Called every frame
void AAttackTargetTester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AttackMovingTimeline.TickTimeline(DeltaTime);
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

// ============================================= Timeline function ====================================================

void AAttackTargetTester::UpdateAttackingPosition(float Alpha)
{
	const FVector CharacterCurrentPos = GetActorLocation();
	
	const FVector LerpPos = UKismetMathLibrary::VLerp(SelfAttackStartPos,AttackMovingDestination, Alpha);

	const FVector MovingPos = FVector(LerpPos.X, LerpPos.Y, CharacterCurrentPos.Z);

	SetActorLocation(MovingPos);
}

// ================================================== Interface Functions ============================================
void AAttackTargetTester::TryToDamagePlayer_Implementation()
{
	ICharacterActionInterface::TryToDamagePlayer_Implementation();
	
	
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	
	FName AttackSocketName;
	if(CurrentAttackType == EEnemyAttackType::AbleToCounter)
		AttackSocketName = "Head";
	else
		AttackSocketName = "RightLeg";
	
	const FVector HeadSocketLocation = GetMesh()->GetSocketLocation(AttackSocketName);

	TArray<AActor*> FoundActorList;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	
	UKismetSystemLibrary::SphereOverlapActors(World, HeadSocketLocation, 120, FilterType, FilteringClass, IgnoreActors,FoundActorList);

	if(FoundActorList.Num() != 0)
	{
		for (AActor* EachFoundActor : FoundActorList)
		{
			IPlayerDamageInterface::Execute_ReceiveDamageFromEnemy(EachFoundActor, BaseDamageAmount, this, CurrentAttackType);
			//UGameplayStatics::ApplyDamage(EachFoundActor, 30, GetController(), this, DamageType);
		}
	}

}

void AAttackTargetTester::ReceiveDamageFromPlayer_Implementation(int32 DamageAmount, AActor* DamageCauser,
	EPlayerAttackType PlayerAttackType)
{
	IDamageInterface::ReceiveDamageFromPlayer_Implementation(DamageAmount, DamageCauser, PlayerAttackType);

	// switch (PlayerAttackType)
	// {
	// case EPlayerAttackType::ShortFlipKick:
	// 	PlayDamageReceiveAnimation(0);
	// 	break;
	// case EPlayerAttackType::FlyingKick:
	// 	PlayDamageReceiveAnimation(1);
	// 	break;
	// case EPlayerAttackType::FlyingPunch:
	// 	PlayDamageReceiveAnimation(2);
	// 	break;
	// case EPlayerAttackType::SpinKick:
	// 	PlayDamageReceiveAnimation(3);
	// 	break;
	// case EPlayerAttackType::DashingDoubleKick:
	// 	PlayDamageReceiveAnimation(4);
	// 	break;
	// default: ;
	// }

}

void AAttackTargetTester::ShowEnemyAttackIndicator_Implementation()
{
	IEnemyWidgetInterface::ShowEnemyAttackIndicator_Implementation();

	if(AttackIndicatorRef)
		AttackIndicatorRef->ShowIndicator();
}

void AAttackTargetTester::UnShowEnemyAttackIndicator_Implementation()
{
	IEnemyWidgetInterface::UnShowEnemyAttackIndicator_Implementation();

	if(AttackIndicatorRef)
		AttackIndicatorRef->UnShowIndicator();
}

void AAttackTargetTester::ShowPlayerTargetIndicator_Implementation()
{
	IEnemyWidgetInterface::ShowPlayerTargetIndicator_Implementation();

	if(EnemyTargetWidgetRef)
		EnemyTargetWidgetRef->ShowIndicator();
}

void AAttackTargetTester::UnShowPlayerTargetIndicator_Implementation()
{
	IEnemyWidgetInterface::UnShowPlayerTargetIndicator_Implementation();

	if(EnemyTargetWidgetRef)
		EnemyTargetWidgetRef->UnShowIndicator();
}
