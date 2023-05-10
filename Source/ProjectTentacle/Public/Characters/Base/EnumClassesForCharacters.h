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
	CounterAttack = 5 UMETA(DisplayName = "CounterAttack"),
	FastKick = 6 UMETA(DisplayName = "FastKick"),
	FastPunch = 7 UMETA(DisplayName = "FastPunch"),
	None = 8 UMETA(DisplaName = "None")
};

UENUM(BlueprintType)
enum class EPlayerCameraType: uint8
{
	OutOfCombat = 0 UMETA(DisplayName = "OutOfCombat"),
	InCombat = 1 UMETA(DisplayName = "InCombat"),
	Execution = 2 UMETA(DisplayName = "Execution"),
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
	PreAction = 6 UMETA(DisplayName = "PreAction"),
	Dodge = 7 UMETA(DisplayName = "DODGE"),
	BeforeAttack = 8 UMETA(DisplayName = "BeforeAttack"),
	SpecialAbility1 = 9 UMETA(DisplayName = "SpecialAbility1"),
	SpecialAbility2 = 10 UMETA(DisplayName = "SpecialAbility2"),
};

UENUM(BlueprintType)
enum class EEnemyAttackType: uint8
{
	AbleToCounter = 0 UMETA(DisplayName = "AbleToCounter"),
	UnableToCounter = 1 UMETA(DisplayName = "UnableToCounter"),
};

UENUM(BlueprintType)
enum class EEnemyCurrentState: uint8
{
	WaitToAttack = 0 UMETA(DisplayName = "WaitToAttack"),
	Attacking = 1 UMETA(DisplayName = "Attacking"),
	Countered = 2 UMETA(DisplayName = "Countered"),
	GettingUp = 3 UMETA(DisplayName = "GettingUp"),
	Damaged = 4 UMETA(DisplayName = "Damaged"),
	Dead = 5 UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EEnemyAttackAnimMontages: uint8
{
	HeadButt = 0 UMETA(DisplayName = "HeadButt"),
	MMAKick = 1 UMETA(DisplayName = "MMAKick"),
};