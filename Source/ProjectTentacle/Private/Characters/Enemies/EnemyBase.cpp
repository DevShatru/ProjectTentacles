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
	UnCounterMovingTimeline.AddInterpFloat(UncounterableAttackMovingCurve, MovingAttackPosUpdate);
	CounterableMovingTimeline.AddInterpFloat(CounterableAttackMovingCurve, MovingAttackPosUpdate);
	
	
}

void AEnemyBase::InitializeEnemyControllerRef()
{
	TryGetOwnController();
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UnCounterMovingTimeline.TickTimeline(DeltaTime);
	CounterableMovingTimeline.TickTimeline(DeltaTime);
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
	if(OwnController)
	{
		if(!BBComponent)
		{
			BBComponent = OwnController->GetBlackboardComponent();
		}
		return;
	}
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
	
	// Set is attacking
	TrySwitchEnemyState(EEnemyCurrentState::Attacking);

	// Set Character unable to move to prevent task early finish and enemy is able to move while animation montage
	TryStopMoving();

	// Debug Attack Moving timeline activate
	if(EnableAttackMovement)
	{
		PlaySpecificAttackMovingTimeline(CurrentAttackType);
	}
	
	// switch case on current attack type to fire different animation 
	switch (CurrentAttackType)
	{
		case EEnemyAttackType::AbleToCounter:
			if(CounterableAttackMontage != nullptr)
				PlayAnimMontage(CounterableAttackMontage, 1, "Default");
			break;
		case EEnemyAttackType::UnableToCounter:
			if(NotCounterableAttackMontage != nullptr)
				PlayAnimMontage(NotCounterableAttackMontage, 1, "Default");
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

void AEnemyBase::PlaySpecificAttackMovingTimeline(EEnemyAttackType AttackType)
{
	switch (AttackType)
	{
	case EEnemyAttackType::AbleToCounter:
		CounterableMovingTimeline.PlayFromStart();
		break;
	case EEnemyAttackType::UnableToCounter:
		UnCounterMovingTimeline.PlayFromStart();
		break;
	default: break;
	}
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

	TrySwitchEnemyState(EEnemyCurrentState::WaitToAttack);	

	StopAnimMontage();
	PlayAnimMontage(GetUpMontage,2,"Default");
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

void AEnemyBase::TryStopAttackMovement()
{
	if(EnableAttackMovement)
	{
		UnCounterMovingTimeline.Stop();
		CounterableMovingTimeline.Stop();
	}
}

// Finish attack task and switch to requested task
void AEnemyBase::TryFinishAttackTask(EEnemyCurrentState SwitchingState)
{
	// if BT component is valid and if current enemy state is attacking
	if(BTComponent && CurrentEnemyState == EEnemyCurrentState::Attacking)
	{
		// switch state to standing
		TrySwitchEnemyState(SwitchingState);
		const bool bIsBound = OnFinishAttackingTask.ExecuteIfBound(BTComponent, true, false);
	}
}

// ============================================= Timeline function ====================================================

void AEnemyBase::UpdateAttackingPosition(float Alpha)
{
	// Update lerping destination if enemy attack tracking is enable and current Alpha is not exceeding limit
	if(EnableEnemyAttackTracking)
		if(Alpha < AttackTrackingLimitInAlpha)
		{
			const ACharacter* PlayerCha = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
			if(!PlayerCha) return;

			const FVector PlayerPos = PlayerCha->GetActorLocation();
	
			AttackMovingDestination = CalculateDestinationForAttackMoving(PlayerPos);
		}

	const FVector CharacterCurrentPos = GetActorLocation();
	
	const FVector LerpPos = UKismetMathLibrary::VLerp(SelfAttackStartPos,AttackMovingDestination, Alpha);

	const FVector MovingPos = FVector(LerpPos.X, LerpPos.Y, CharacterCurrentPos.Z);

	SetActorLocation(MovingPos);
}


void AEnemyBase::ActionEnd_Implementation(bool BufferingCheck)
{
	ICharacterActionInterface::ActionEnd_Implementation(BufferingCheck);
	
	TryFinishAttackTask(EEnemyCurrentState::WaitToAttack);

	SetIsCountered(false);
}

void AEnemyBase::OnResumeMovement_Implementation()
{
	ICharacterActionInterface::OnResumeMovement_Implementation();

	TryResumeMoving();
}

void AEnemyBase::OnResetEnemyCurrentState_Implementation()
{
	ICharacterActionInterface::OnResetEnemyCurrentState_Implementation();

	CurrentEnemyState = EEnemyCurrentState::WaitToAttack;
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
		}
	}
	
	TryFinishAttackTask(EEnemyCurrentState::WaitToAttack);
}

void AEnemyBase::TryTriggerPlayerCounter_Implementation()
{
	ICharacterActionInterface::TryTriggerPlayerCounter_Implementation();

	TArray<AActor*> FoundActorList = GetActorsInFrontOfEnemy(false);

	if(FoundActorList.Num() != 0)
	{
		for (AActor* EachFoundActor : FoundActorList)
		{
			ICharacterActionInterface::Execute_TryStoreCounterTarget(EachFoundActor, this);
		}
	}

}

void AEnemyBase::OnCounterTimeEnd_Implementation()
{
	ICharacterActionInterface::OnCounterTimeEnd_Implementation();

	OnHideAttackIndicator();

	if(!IsCountered)
	{
		ACharacter* PlayerCharacterClass = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
		ICharacterActionInterface::Execute_TryRemoveCounterTarget(PlayerCharacterClass, this);
	}
}

// ===================================================== Damage Receive ========================================================

void AEnemyBase::ReceiveDamageFromPlayer_Implementation(int32 DamageAmount, AActor* DamageCauser,
                                                        EPlayerAttackType PlayerAttackType)
{
	IDamageInterface::ReceiveDamageFromPlayer_Implementation(DamageAmount, DamageCauser, PlayerAttackType);

	// // Cancel movement if we take damage
	// TryGetOwnController();
	// OwnController->StopMovement();

	bool StateChanged = false;
	
	// if enemy is attack, stop montage, flip bool to false, unshow attack indicator, and execute onfinish attack delegate
	if(CurrentEnemyState == EEnemyCurrentState::Attacking && PlayerAttackType != EPlayerAttackType::CounterAttack)
	{
		StopAnimMontage();
		
		OnHideAttackIndicator();

		// Stop attack movement
		TryStopAttackMovement();
		
		
		TryFinishAttackTask(EEnemyCurrentState::Damaged);

		// change StateChanged bool to true to prevent changing again
		StateChanged = true;

	}
	
	// if player is doing counter attack damage
	if(PlayerAttackType == EPlayerAttackType::CounterAttack)
	{
		TryFinishAttackTask(EEnemyCurrentState::Countered);

		SetIsCountered(false);
		
		// change StateChanged bool to true to prevent changing again
		StateChanged = true;
	}
	
	HealthReduction(DamageAmount);

	
	// if bool StateChanged is false, it means enemy is not taking damage when it get countered or get damaged while doing attack
	if(!StateChanged) TrySwitchEnemyState(EEnemyCurrentState::Damaged);
	
	if((Health - DamageAmount) > 0)
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
	TryGetOwnController();
	OwnController->OnDeath();
	
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


	// TODO: Maybe don't need
	//CurrentEnemyState = EEnemyCurrentState::Countered;
	
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