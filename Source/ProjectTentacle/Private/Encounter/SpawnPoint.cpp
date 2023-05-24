// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#include "Encounter/SpawnPoint.h"

#include "Characters/Enemies/EnemyBase.h"
#include "Characters/Enemies/UnitPool.h"
#include "Components/BoxComponent.h"
#include "Encounter/EncounterVolume.h"
FActorSpawnParameters* ASpawnPoint::SpawnParams = nullptr;

// Sets default values
ASpawnPoint::ASpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ShackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shack Mesh"));
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Black Pane"));
	DoorOpeningVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Door Opening Volume"));
	SpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Location"));

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
	
	// Determine what unit to spawn based on weight
	const float MeleeSpawnWeight = bShouldSpawnMelee ? MeleeUnitsSpawnWeight : 0.f,
				RangedSpawnWeight = bShouldSpawnRanged ? RangedUnitsSpawnWeight : 0.f,
				BruteSpawnWeight = bShouldSpawnBrute ? BruteUnitsSpawnWeight : 0.f,
				HealerSpawnWeight = bShouldSpawnHealer ? HealerUnitsSpawnWeight : 0.f;
	const float SpawnWeightSum = MeleeSpawnWeight + RangedSpawnWeight + BruteSpawnWeight + HealerSpawnWeight;

	if (SpawnWeightSum <= 0.f) {
		StopSpawningUnits();
		return;
	}

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
	SpawnedUnits.Add(Unit);
	Unit->SetActorLocation(SpawnLocation->GetComponentLocation());
	++UnitsSpawned[TypeToSpawn];
	OwningEncounterVolume->AddSpawnedUnitToEncounter(Unit);
	CheckUnitsToSpawn();
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

void ASpawnPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DoorOpeningVolume->OnComponentBeginOverlap.RemoveDynamic(this, &ASpawnPoint::TryOpenDoor);
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
	// If open, extend open time
	if(bIsDoorOpen)
	{
		TimerManager->ClearTimer(DoorTimerHandle);
		TimerManager->SetTimer(DoorTimerHandle, this, &ASpawnPoint::TryCloseDoor, DoorHeldOpenTime, false);
		return;
	}	
	// If closed or partly open, start opening timer
	CalculateDoorOpenness();
	DoorTransitionStartTime = World->GetTimeSeconds();
	TimerManager->SetTimerForNextTick(this, &ASpawnPoint::DoorOpenTransition);
}

void ASpawnPoint::DoorOpenTransition()
{
	const float Alpha = DoorOpenness + (World->GetTimeSeconds() - DoorTransitionStartTime) / DoorOpeningTime;
	const float Delta = FMath::Clamp(DoorOpenCurve ? DoorOpenCurve->GetFloatValue(Alpha) : Alpha, 0.f, 1.f);
	const FRotator CurrentRotation = DoorMesh->GetRelativeRotation();
	DoorMesh->SetRelativeRotation(FRotator(CurrentRotation.Pitch,  FMath::Lerp(DoorClosedYaw, DoorOpenedYaw, Delta), CurrentRotation.Roll));

	if(Alpha >= 1.f)
	{
		bIsDoorOpen = true;
		TimerManager->SetTimer(DoorTimerHandle, this, &ASpawnPoint::TryCloseDoor, DoorHeldOpenTime, false);
		return;	
	}
	TimerManager->SetTimerForNextTick(this, &ASpawnPoint::DoorOpenTransition);
}

void ASpawnPoint::TryCloseDoor()
{
	bIsDoorOpen = false;
	DoorTransitionStartTime = World->GetTimeSeconds();
	TimerManager->SetTimerForNextTick(this, &ASpawnPoint::DoorCloseTransition);
}

void ASpawnPoint::DoorCloseTransition()
{
	const float Alpha = (World->GetTimeSeconds() - DoorTransitionStartTime) / DoorClosingTime;
	const float Delta = FMath::Clamp(DoorOpenCurve ? DoorOpenCurve->GetFloatValue(Alpha) : Alpha, 0.f, 1.f);
	const FRotator CurrentRotation = DoorMesh->GetRelativeRotation();
	DoorMesh->SetRelativeRotation(FRotator(CurrentRotation.Pitch,  FMath::Lerp(DoorClosedYaw, DoorOpenedYaw, 1.f - Delta), CurrentRotation.Roll));

	if(Alpha >= 1.f) return;
	TimerManager->SetTimerForNextTick(this, &ASpawnPoint::DoorCloseTransition);
}

void ASpawnPoint::Setup()
{
	DoorOpeningVolume->OnComponentBeginOverlap.AddDynamic(this, &ASpawnPoint::TryOpenDoor);
	bIsDoorOpen = false;
	World = GetWorld();
	SpawnedUnits.Empty();
	TimerManager = &World->GetTimerManager();
	if(!SpawnParams) SpawnParams = new FActorSpawnParameters();
	SpawnParams->bNoFail = true;
	SpawnParams->SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ResetSpawnPoint();
	CheckUnitsToSpawn();
}

void ASpawnPoint::ResetSpawnPoint()
{
	for(const EEnemyType Type : TEnumRange<EEnemyType>())
	{
		UnitsSpawned.Add(Type, 0);
	}
}

void ASpawnPoint::CalculateDoorOpenness()
{
	const float ClampedYaw = DoorClosedYaw > DoorOpenedYaw ? FMath::Clamp(DoorMesh->GetRelativeRotation().Yaw, DoorOpenedYaw,DoorClosedYaw) : FMath::Clamp(DoorMesh->GetRelativeRotation().Yaw, DoorClosedYaw, DoorOpenedYaw);
	DoorOpenness = ClampedYaw / FMath::Abs(DoorOpenedYaw - DoorClosedYaw);
}

void ASpawnPoint::CheckUnitsToSpawn()
{
	bShouldSpawnMelee = UnitsSpawned[EEnemyType::Melee] < NumMeleeUnitsSpawned;
	bShouldSpawnRanged = UnitsSpawned[EEnemyType::Ranged] < NumRangedUnitsSpawned;
	bShouldSpawnBrute = UnitsSpawned[EEnemyType::Brute] < NumBruteUnitsSpawned;
	bShouldSpawnHealer = UnitsSpawned[EEnemyType::Healer] < NumHealerUnitsSpawned;
	bSpawnComplete = !(bShouldSpawnMelee || bShouldSpawnBrute || bShouldSpawnRanged);
}

