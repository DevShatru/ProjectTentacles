// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Encounter/SpawnPoint.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/Enemies/UnitPool.h"
#include "Components/BoxComponent.h"
#include "Encounter/EncounterVolume.h"
FActorSpawnParameters* ASpawnPoint::SpawnParams = nullptr;

// Sets default values
ASpawnPoint::ASpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ShackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shack Mesh"));
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Black Pane"));
	DoorOpeningVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Door Opening Volume"));
	SpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Location"));
	MovingOutLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Moving Out Location"));

	DoorMesh->SetCanEverAffectNavigation(false);
	PlaneMesh->SetCanEverAffectNavigation(false);

	PlaneMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	DoorOpeningVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	DoorOpeningVolume->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	
	SetRootComponent(ShackMesh);
	DoorMesh->SetupAttachment(RootComponent);
	PlaneMesh->SetupAttachment(RootComponent);
	DoorOpeningVolume->SetupAttachment(RootComponent);
	SpawnLocation->SetupAttachment(RootComponent);
}

void ASpawnPoint::StartSpawningUnits()
{
	TimerManager->SetTimer(SpawnTimerHandle, this, &ASpawnPoint::SpawnUnit, TimeBetweenSpawns, true, TimeBetweenSpawns);
}

void ASpawnPoint::StopSpawningUnits()
{
	TimerManager->ClearTimer(SpawnTimerHandle);
	ResetSpawnPoint();
}

void ASpawnPoint::SetUnitPool(AUnitPool* NewUnitPool)
{
	UnitPool = NewUnitPool;
}

void ASpawnPoint::SpawnUnit()
{
	// Do nothing if not part of an encounter
	if(!OwningEncounterVolume) return;

	if (bSpawnComplete) {
		StopSpawningUnits();
		return;
	}
	
	// Determine what unit to spawn based on weight
	const float MeleeSpawnWeight = bShouldSpawnMelee ? MeleeUnitsSpawnWeight : 0.f,
				RangedSpawnWeight = bShouldSpawnRanged ? RangedUnitsSpawnWeight : 0.f,
				BruteSpawnWeight = bShouldSpawnBrute ? BruteUnitsSpawnWeight : 0.f,
				HealerSpawnWeight = bShouldSpawnHealer ? HealerUnitsSpawnWeight : 0.f;
	const float SpawnWeightSum = MeleeSpawnWeight + RangedSpawnWeight + BruteSpawnWeight + HealerSpawnWeight;

	const float WeightedRandom = FMath::FRandRange(0.f, SpawnWeightSum);

	// Determine type of unit to spawn based on which band WeightedRandom is in
	const EEnemyType TypeToSpawn = WeightedRandom <= MeleeSpawnWeight ? EEnemyType::Melee :
								   WeightedRandom <= MeleeSpawnWeight + RangedSpawnWeight ? EEnemyType::Ranged :
								   WeightedRandom <= MeleeSpawnWeight + RangedSpawnWeight + BruteSpawnWeight ? EEnemyType::Brute : EEnemyType::Healer;

	// Get Unit from pool
	// Spawn if pool is unavailable
	AEnemyBase* Unit = UnitPool ? UnitPool->GetUnitFromPool(TypeToSpawn, SpawnParams) :
					   World->SpawnActor<AEnemyBase>(TypeToSpawn == EEnemyType::Melee ? MeleeUnitClass :
														  TypeToSpawn == EEnemyType::Ranged ? RangedUnitClass :
														  TypeToSpawn == EEnemyType::Brute ? BruteUnitClass : HealerUnitClass, *SpawnParams);
	Unit->OnSpawn();
	SpawnedUnits.Add(Unit);
	Unit->SetActorLocation(SpawnLocation->GetComponentLocation());
	++UnitsSpawned[TypeToSpawn];
	OwningEncounterVolume->AddSpawnedUnitToEncounter(Unit);
	CheckUnitsToSpawn();
	OnBeginMovingOut(Unit);
}

void ASpawnPoint::RegisterOwningEncounter(AEncounterVolume* EncounterVolume)
{
	OwningEncounterVolume = EncounterVolume;
}

bool ASpawnPoint::IsSpawningComplete() const
{
	return bSpawnComplete;
}

// Called when the game starts or when spawned
void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	Setup();
}

void ASpawnPoint::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Handle door orientation
	// Tick starts disabled, only gets enabled when the door needs to open or close
	if(!bIsDoorOpen)
	{
		Alpha -= DeltaSeconds / DoorOpeningTime;

	} else
	{
		Alpha += DeltaSeconds / DoorOpeningTime;
	}
		
	const FRotator CurrentRotation = DoorMesh->GetRelativeRotation();
	if(DoorOpenCurve)
	{
		const float Delta = FMath::Clamp(DoorOpenCurve ? DoorOpenCurve->GetFloatValue(Alpha) : Alpha, 0.f, 1.f);
		DoorMesh->SetRelativeRotation(FRotator(CurrentRotation.Pitch,  FMath::Lerp(DoorClosedYaw, DoorOpenedYaw, Delta), CurrentRotation.Roll));
	}
	else
	{
		DoorMesh->SetRelativeRotation(FRotator(CurrentRotation.Pitch,  FMath::Lerp(DoorOpenedYaw, DoorClosedYaw, Alpha), CurrentRotation.Roll));
	}
	if (Alpha >= 1.f && bIsDoorOpen || Alpha <= 0.f && !bIsDoorOpen) SetActorTickEnabled(false);
}

void ASpawnPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DoorOpeningVolume->OnComponentBeginOverlap.RemoveDynamic(this, &ASpawnPoint::TryOpenDoor);
	DoorOpeningVolume->OnComponentEndOverlap.RemoveDynamic(this, &ASpawnPoint::TryCloseDoor);
	if(SpawnParams)
	{
		delete SpawnParams;
		SpawnParams = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

void ASpawnPoint::TryOpenDoor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the unit is moving out of the shack
	const ACharacter* AsCharacter = Cast<ACharacter>(OtherActor);
	if(!AsCharacter) return;
	AAIController* Controller = Cast<AAIController>(AsCharacter->GetController());

	// Check against blackboard condition to exit shack (So it doesn't open for units trying to come in from outside)
	if(!Controller || !Controller->GetBlackboardComponent()->GetValueAsBool("bHasNotMovingOut")) return;
	
	if(OtherActor != this && !Occupants.Contains(OtherActor)) Occupants.Add(OtherActor);
	
	if(Occupants.Num() <= 0) return;
	OpenDoor();
}

void ASpawnPoint::TryCloseDoor(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(Occupants.Contains(OtherActor)) Occupants.Remove(OtherActor);
	
	if(Occupants.Num() > 0) return;
	TimerManager->SetTimer(DoorTimerHandle, this, &ASpawnPoint::CloseDoor, DoorHeldOpenTime, false);
}

void ASpawnPoint::CloseDoor()
{
	bIsDoorOpen = false;
	SetActorTickEnabled(true);
}

void ASpawnPoint::Setup()
{
	DoorOpeningVolume->OnComponentBeginOverlap.AddDynamic(this, &ASpawnPoint::TryOpenDoor);
	DoorOpeningVolume->OnComponentEndOverlap.AddDynamic(this, &ASpawnPoint::TryCloseDoor);
	bIsDoorOpen = false;
	World = GetWorld();
	SpawnedUnits.Empty();
	TimerManager = &World->GetTimerManager();
	if(!SpawnParams) SpawnParams = new FActorSpawnParameters();
	SpawnParams->bNoFail = true;
	SpawnParams->SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ResetSpawnPoint();
	CheckUnitsToSpawn();
	SetActorTickEnabled(false);
}

void ASpawnPoint::ResetSpawnPoint()
{
	for(const EEnemyType Type : TEnumRange<EEnemyType>())
	{
		UnitsSpawned.Add(Type, 0);
	}
}

void ASpawnPoint::OnBeginMovingOut(AEnemyBase* SpawningUnit)
{
	
	// Get enemy controller
	AEnemyBaseController* SpawnUnitController = SpawningUnit->GetEnemyController();
	if(!SpawnUnitController)
	{
		SpawningUnit->TryGetOwnController();
		SpawnUnitController = SpawningUnit->GetEnemyController();
	}

	// Get Enemy Blackboard value
	UBlackboardComponent* SpawnUnitBB = SpawnUnitController->GetBlackboardComponent();
	if(!SpawnUnitBB) return;
	
	// Overwrite values in BB
	SpawnUnitBB->SetValueAsVector("MovingOutPosition", MovingOutLocation->GetComponentLocation());
	SpawnUnitBB->SetValueAsBool("bHasNotMovingOut", true);
}

void ASpawnPoint::OpenDoor()
{
	bIsDoorOpen = true;
	SetActorTickEnabled(true);
}
void ASpawnPoint::CheckUnitsToSpawn()
{
	bShouldSpawnMelee = UnitsSpawned[EEnemyType::Melee] < NumMeleeUnitsSpawned;
	bShouldSpawnRanged = UnitsSpawned[EEnemyType::Ranged] < NumRangedUnitsSpawned;
	bShouldSpawnBrute = UnitsSpawned[EEnemyType::Brute] < NumBruteUnitsSpawned;
	bShouldSpawnHealer = UnitsSpawned[EEnemyType::Healer] < NumHealerUnitsSpawned;
	bSpawnComplete = !(bShouldSpawnMelee || bShouldSpawnBrute || bShouldSpawnRanged || bShouldSpawnHealer);
}

