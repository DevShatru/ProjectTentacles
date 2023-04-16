// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Characters/Enemies/EnemyBase.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Enemies/EnemyBaseController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	GetCharacterMovement()->bUseRVOAvoidance = true;

	// Set up indicator and target icon widget component
	EnemyAttackIndicatorWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT( "EnemyAttackIndicatorWidget" ));
	EnemyTargetedIconWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT( "EnemyTargetedIconWidget" ));
	EnemyAttackIndicatorWidgetComponent->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	EnemyTargetedIconWidgetComponent->AttachToComponent( RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AEnemyBase::RegisterOwningEncounter(AEncounterVolume* NewOwningEncounter)
{
	TryGetOwnController();
	OwnController->RegisterOwningEncounter(NewOwningEncounter);
}

void AEnemyBase::EngageTarget(AActor* Target)
{
	TryGetOwnController();
	OwnController->EngageTarget(Target);
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	InitializeWidgetComponents();

	InitializeEnemyControllerRef();
}

void AEnemyBase::InitializeWidgetComponents()
{
	// Get attack indicator widget reference
	UUserWidget* ReturnAttackIndicatorWidget = EnemyAttackIndicatorWidgetComponent->GetWidget();
	UWidget_EnemyAttackIndicator* CastedAttackIndicatorWidget = Cast<UWidget_EnemyAttackIndicator>(ReturnAttackIndicatorWidget);
	
	if(CastedAttackIndicatorWidget)
	{
		// Bind function and unshow the indicator
		AttackIndicatorRef = CastedAttackIndicatorWidget;
		OnUpdatingEnemyAttackIndicator.BindDynamic(AttackIndicatorRef, &UWidget_EnemyAttackIndicator::OnReceivingNewAttackType);
		AttackIndicatorRef->UnShowIndicator();
	}

	// Get target icon widget reference
	UUserWidget* ReturnTargetedIconWidget = EnemyTargetedIconWidgetComponent->GetWidget();
	UWidget_EnemyTargetIconWidget* CastedTargetedIconWidget = Cast<UWidget_EnemyTargetIconWidget>(ReturnTargetedIconWidget);

	if(CastedTargetedIconWidget)
	{
		// set variable to casted widget and hide indicator
		EnemyTargetWidgetRef = CastedTargetedIconWidget;
		EnemyTargetWidgetRef->UnShowIndicator();
	}

	// timeline binding
	FOnTimelineFloat MovingAttackPosUpdate;
	MovingAttackPosUpdate.BindDynamic(this, &AEnemyBase::UpdateAttackingPosition);
	AttackMovingTimeline.AddInterpFloat(AttackMovingCurve, MovingAttackPosUpdate);
}

void AEnemyBase::InitializeEnemyControllerRef()
{
	
	// Get enemy base controller
	AController* EnemyController = GetController();
	if(Controller == nullptr) return;
		
	AEnemyBaseController* CastedEnemyBaseController =  Cast<AEnemyBaseController>(EnemyController);
	if(CastedEnemyBaseController == nullptr) return;

	// Assign Enemy base controller
	if(CurrentEnemyBaseController == nullptr)
		CurrentEnemyBaseController = CastedEnemyBaseController;

	UBlackboardComponent* CastedBBComponent = CastedEnemyBaseController->GetBlackboardComponent();
	if(CastedBBComponent)
		BBComponent = CastedBBComponent;
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AttackMovingTimeline.TickTimeline(DeltaTime);
}

float AEnemyBase::GetAttackCompletionTime() const
{
	return AttackCompletionTime;
}

float AEnemyBase::GetAttackCounterableTime() const
{
	return AttackCounterableTime;
}

void AEnemyBase::OnHideAttackIndicator()
{
	if(AttackIndicatorRef)
		AttackIndicatorRef->UnShowIndicator();
}

void AEnemyBase::TryGetOwnController()
{
	if(OwnController) return;
	OwnController = Cast<AEnemyBaseController>(GetController());
}


// ====================================================== Attack ======================================================

void AEnemyBase::ExecuteAttack()
{
	// SetAttackType and get the result enum of it
	EEnemyAttackAnimMontages ResultEnemyAnimMontage = SetAttackType();

	
	if(AttackIndicatorRef)
	{
		// execute delegate function to update variables in Indicator widget class
		OnUpdatingEnemyAttackIndicator.Execute(CurrentAttackType);
	}

	// Save lerp start and end position for later timeline function usage
	SelfAttackStartPos = GetActorLocation();

	// Get Player Position
	const ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	FVector PlayerCurrentPos = PlayerCharacter->GetActorLocation();
	
	const FVector DestinationPos = CalculateDestinationForAttackMoving(PlayerCurrentPos);
	AttackMovingDestination = DestinationPos;

	IsAttacking = true;
	
	// switch case on current attack type to fire different animation 
	switch (CurrentAttackType)
	{
		case EEnemyAttackType::AbleToCounter:
			if(CounterableAttackMontage != nullptr)
				PlayAnimMontage(CounterableAttackMontage, 1, "Default");
			//AttackMovingTimeline.PlayFromStart();
			break;
		case EEnemyAttackType::UnableToCounter:
			if(NotCounterableAttackMontage != nullptr)
				PlayAnimMontage(NotCounterableAttackMontage, 1, "Default");
			//AttackMovingTimeline.PlayFromStart();
			break;
		default: break;
	}

}

EEnemyAttackAnimMontages AEnemyBase::SetAttackType()
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


FVector AEnemyBase::CalculateDestinationForAttackMoving(FVector PlayerPos)
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
	const bool bHit = UKismetSystemLibrary::LineTraceSingle(this, SelfAttackStartPos, SupposedAttackMovingDestination, UEngineTypes::ConvertToTraceType(ECC_Visibility),false, IgnoreActors,  EDrawDebugTrace::None,Hit,true);

	if(!bHit) return SupposedAttackMovingDestination;

	const FVector DirFromPlayerToSelf = DirFromSelfToPlayer * -1;
	
	return Hit.ImpactPoint + (DirFromPlayerToSelf * OffsetFromPlayer);
}


TArray<AActor*> AEnemyBase::GetActorsInFrontOfEnemy(bool IsDamaging)
{
	const UWorld* World = GetWorld();
	
	FName AttackSocketName;
	if(CurrentAttackType == EEnemyAttackType::AbleToCounter)
		AttackSocketName = "RightHand";
	else
		AttackSocketName = "RightFoot";
	
	const FVector HeadSocketLocation = GetMesh()->GetSocketLocation(AttackSocketName);

	TArray<AActor*> FoundActorList;


	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	
	TArray<AEnemyBase*> Allies = OwnController->GetAllies();
	
	if(Allies.Num() > 0)
		for (AEnemyBase* EachAlly : Allies)
		{
			IgnoreActors.Add(EachAlly);
		}

	float Radius;
	float Height;
	
	if(IsDamaging)
	{
		Radius = DamageTriggerRadius;
		Height = DamageTriggerHeight;
	}
	else
	{
		Radius = CounterTriggerRadius;
		Height = CounterTriggerHeight;
	}
	
	
	UKismetSystemLibrary::CapsuleOverlapActors(World, HeadSocketLocation, Radius, Height, FilterType, FilteringClass, IgnoreActors, FoundActorList);

	return FoundActorList;
}

void AEnemyBase::StartCounterAttackAnimation()
{
	if(!CounterVictimMontage) return;
	
	StopAnimMontage();
	OnHideAttackIndicator();
	PlayAnimMontage(CounterVictimMontage,1,"Start");
}

// ===================================================== Stunning ===========================================
void AEnemyBase::BeginLyingCountDown()
{
	const UWorld* World = GetWorld();
	if(World == nullptr) return;

	PlayLyingMontage();
	
	World->GetTimerManager().SetTimer(GettingUpTimerHandle,this, &AEnemyBase::RecoverFromLying, TimeToGetUp, false, -1);
}

void AEnemyBase::PlayLyingMontage()
{
	if(!CounterVictimMontage) return;
	
	StopAnimMontage();
	PlayAnimMontage(CounterVictimMontage,1,"LyingLoop");
}


void AEnemyBase::RecoverFromLying()
{
	if(!GetUpMontage) return;

	CurrentEnemyState = EEnemyCurrentState::Standing;
	
	StopAnimMontage();
	PlayAnimMontage(GetUpMontage,1,"Default");
}

EEnemyType AEnemyBase::GetType() const
{
	return UnitType;
}

void AEnemyBase::PlayFinishedAnimation()
{
	PlayAnimMontage(FinishedAnimation,1,NAME_None);
}

void AEnemyBase::TryStopMoving()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if(MovementComp->MovementMode == EMovementMode::MOVE_Walking)
		MovementComp->DisableMovement();
}

void AEnemyBase::TryResumeMoving()
{
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if(MovementComp->MovementMode == EMovementMode::MOVE_None)
		MovementComp->SetMovementMode(EMovementMode::MOVE_Walking);
}

// ============================================= Timeline function ====================================================

void AEnemyBase::UpdateAttackingPosition(float Alpha)
{
	const FVector CharacterCurrentPos = GetActorLocation();
	
	const FVector LerpPos = UKismetMathLibrary::VLerp(SelfAttackStartPos,AttackMovingDestination, Alpha);

	const FVector MovingPos = FVector(LerpPos.X, LerpPos.Y, CharacterCurrentPos.Z);

	SetActorLocation(MovingPos);
}


void AEnemyBase::ActionEnd_Implementation(bool BufferingCheck)
{
	ICharacterActionInterface::ActionEnd_Implementation(BufferingCheck);

	
	if(BTComponent && IsAttacking)
	{
		IsAttacking = false;
		const bool bIsBound = OnFinishAttackingTask.ExecuteIfBound(BTComponent, true, false);
	}

}

// ================================================== Interface Functions ============================================
void AEnemyBase::TryToDamagePlayer_Implementation()
{
	ICharacterActionInterface::TryToDamagePlayer_Implementation();
	
	TArray<AActor*> FoundActorList = GetActorsInFrontOfEnemy(true);

	if(FoundActorList.Num() != 0)
	{
		for (AActor* EachFoundActor : FoundActorList)
		{
			IDamageInterface::Execute_ReceiveDamageFromEnemy(EachFoundActor, BaseDamageAmount, this, CurrentAttackType);
			//UGameplayStatics::ApplyDamage(EachFoundActor, 30, GetController(), this, DamageType);
		}
	}
}

void AEnemyBase::TryTriggerPlayerCounter_Implementation()
{
	ICharacterActionInterface::TryTriggerPlayerCounter_Implementation();

	TArray<AActor*> FoundActorList = GetActorsInFrontOfEnemy(false);

	if(FoundActorList.Num() != 0)
	{
		for (AActor* EachFoundActor : FoundActorList)
		{
			ICharacterActionInterface::Execute_ReceiveAttackInCounterState(EachFoundActor, this);
		}
	}

}

// ===================================================== Damage Receive ========================================================

void AEnemyBase::ReceiveDamageFromPlayer_Implementation(int32 DamageAmount, AActor* DamageCauser,
                                                        EPlayerAttackType PlayerAttackType)
{
	IDamageInterface::ReceiveDamageFromPlayer_Implementation(DamageAmount, DamageCauser, PlayerAttackType);

	// // Cancel movement if we take damage
	// OwnController->StopMovement();
	
	// if enemy is attack, stop montage, flip bool to false, unshow attack indicator, and execute onfinish attack delegate 
	if(IsAttacking && PlayerAttackType != EPlayerAttackType::CounterAttack)
	{
		StopAnimMontage();
		
		OnHideAttackIndicator();
		
		if(BTComponent)
			const bool bIsBound = OnFinishAttackingTask.ExecuteIfBound(BTComponent, true, false);
	}
	
	HealthReduction(DamageAmount);

	TryResumeMoving();
	
	if(Health >= 1)
	{
		PlayReceiveDamageAnimation(PlayerAttackType);
		return;
	}
	
	OnDeath();
}

void AEnemyBase::HealthReduction(float DamageAmount)
{
	// clamp health that is deducted 
	Health = UKismetMathLibrary::Clamp((Health - DamageAmount),0,MaxHealth);
}

void AEnemyBase::OnDeath()
{
	// remove self from Encounter volume list
	CurrentEnemyBaseController->QuitFromEncounter();
	
	// TODO: detach controller and reset behaviour tree
	
	
	// Clear from player's target reference
	TryClearFromPlayerTarget();
	
	// flip death bool to prevent getting selected
	IsDead = true;

	// Clear Collision
	TurnCollisionOffOrOn(true);
	
	// Ragdoll Physics
	RagDollPhysicsOnDead();

	// TODO: Dissolve after few seconds
}

void AEnemyBase::TryClearFromPlayerTarget()
{
	ACharacter* PlayerCharacterClass = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if(PlayerCharacterClass->GetClass()->ImplementsInterface(UCharacterActionInterface::StaticClass()))
		ICharacterActionInterface::Execute_DetachEnemyTarget(PlayerCharacterClass);
}

void AEnemyBase::TurnCollisionOffOrOn(bool TurnCollisionOff)
{
	
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	if(TurnCollisionOff)
		CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	else
		CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	
	USkeletalMeshComponent* SkeleMeshComp = GetMesh();
	
	if(TurnCollisionOff)
		SkeleMeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	else
		SkeleMeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void AEnemyBase::RagDollPhysicsOnDead()
{
	USkeletalMeshComponent* SelfCharacterMesh = GetMesh();

	SelfCharacterMesh->SetSimulatePhysics(true);
	
	
	const ACharacter* PlayerCharacterClass = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
	const FVector PlayerPos = PlayerCharacterClass->GetActorLocation();
	const FVector SelfPos = GetActorLocation();
	const FVector DirFromPlayerToSelf = UKismetMathLibrary::Normal((SelfPos - PlayerPos));
	
	SelfCharacterMesh->AddImpulse(DirFromPlayerToSelf * 5000,"Spine1",true);
	
	UCharacterMovementComponent* SelfCharacterMovement = GetCharacterMovement();
	SelfCharacterMovement->DisableMovement();
}

void AEnemyBase::PlayReceiveDamageAnimation(EPlayerAttackType ReceivedAttackType)
{
	// Switch case on player's attack type to play different damage receive animation
	switch (ReceivedAttackType)
	{
		case EPlayerAttackType::ShortFlipKick:
			PlayAnimMontage(ReceiveShortFlipKick,1,NAME_None);
		case EPlayerAttackType::FlyingKick:
			PlayAnimMontage(ReceiveFlyingKick,1,NAME_None);
			break;
		case EPlayerAttackType::FlyingPunch:
			PlayAnimMontage(ReceiveFlyingPunch,1,NAME_None);
			break;
		case EPlayerAttackType::SpinKick:
			PlayAnimMontage(ReceiveSpinKick,1,NAME_None);
			break;
		case EPlayerAttackType::DashingDoubleKick:
			PlayAnimMontage(ReceiveDashingDoubleKick,1,NAME_None);
			break;
		case EPlayerAttackType::FastKick:
			PlayAnimMontage(ReceiveFlyingPunch,1,NAME_None);
			break;
		case EPlayerAttackType::FastPunch:
			PlayAnimMontage(ReceiveFlyingPunch,1,NAME_None);
			break;
		default: break;
	}
}

// ===================================================== On Death ========================================================





void AEnemyBase::PlayDeathAnimation(EPlayerAttackType ReceivedAttackType)
{
	switch (ReceivedAttackType)
	{
		case EPlayerAttackType::ShortFlipKick: break;
		case EPlayerAttackType::FlyingKick: break;
		case EPlayerAttackType::FlyingPunch: break;
		case EPlayerAttackType::SpinKick: break;
		case EPlayerAttackType::DashingDoubleKick: break;
		case EPlayerAttackType::CounterAttack: break;
		case EPlayerAttackType::FastKick: break;
		case EPlayerAttackType::FastPunch: break;
		default: ;
	}
}

void AEnemyBase::StartLyingOnTheGround_Implementation()
{
	ICharacterActionInterface::StartLyingOnTheGround_Implementation();

	CurrentEnemyState = EEnemyCurrentState::Lying;
	
	BeginLyingCountDown();
}

void AEnemyBase::RepeatLyingOnTheGround_Implementation()
{
	ICharacterActionInterface::RepeatLyingOnTheGround_Implementation();

	PlayLyingMontage();
}

void AEnemyBase::ShowEnemyAttackIndicator_Implementation()
{
	IEnemyWidgetInterface::ShowEnemyAttackIndicator_Implementation();

	if(AttackIndicatorRef)
		AttackIndicatorRef->ShowIndicator();
}

void AEnemyBase::UnShowEnemyAttackIndicator_Implementation()
{
	IEnemyWidgetInterface::UnShowEnemyAttackIndicator_Implementation();

	OnHideAttackIndicator();
}

void AEnemyBase::ShowPlayerTargetIndicator_Implementation()
{
	IEnemyWidgetInterface::ShowPlayerTargetIndicator_Implementation();

	if(EnemyTargetWidgetRef)
		EnemyTargetWidgetRef->ShowIndicator();
}

void AEnemyBase::UnShowPlayerTargetIndicator_Implementation()
{
	IEnemyWidgetInterface::UnShowPlayerTargetIndicator_Implementation();

	if(EnemyTargetWidgetRef)
		EnemyTargetWidgetRef->UnShowIndicator();
}



void AEnemyBase::InstantRotation(FVector RotatingVector)
{
	const FRotator InputRotation = UKismetMathLibrary::MakeRotFromX(RotatingVector);

	SetActorRotation(InputRotation);
}