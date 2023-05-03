#pragma once

#pragma once

UENUM(BlueprintType)
enum class EEnemyType: uint8
{
	Melee = 0 UMETA(DisplayName = "Melee"),
	Ranged = 1 UMETA(DisplayName = "Ranged"),
	Brute = 2 UMETA(DisplayName = "Brute"),
	Healer = 3 UMETA(DisplayName = "Healer"),
	Count UMETA(Hidden)
};
ENUM_RANGE_BY_COUNT(EEnemyType, EEnemyType::Count);
