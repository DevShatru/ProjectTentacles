// Copyright (C) The Tentacle Zone 2023. All Rights Reserved.


#pragma once

UENUM(BlueprintType)
enum class EPlayerAttackType: uint8
{
	ShortFlipKick = 0 UMETA(DisplayName = "ShortFlipKick"),
	FlyingKick = 1 UMETA(DisplayName = "FlyingKick"),
	FlyingPunch = 2 UMETA(DisplayName = "FlyingPunch"),
	SpinKick = 3 UMETA(DisplayName = "SpinKick"),
	DashingDoubleKick = 4 UMETA(DisplayName = "FlyingKick"),
	None = 5 UMETA(DisplaName = "None")
};

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

UENUM(BlueprintType)
enum class EEnemyAttackType: uint8
{
	AbleToCounter = 0 UMETA(DisplayName = "AbleToCounter"),
	UnableToCounter = 1 UMETA(DisplayName = "UnableToCounter"),
};

UENUM(BlueprintType)
enum class EEnemyAttackAnimMontages: uint8
{
	HeadButt = 0 UMETA(DisplayName = "HeadButt"),
	MMAKick = 1 UMETA(DisplayName = "MMAKick"),
};