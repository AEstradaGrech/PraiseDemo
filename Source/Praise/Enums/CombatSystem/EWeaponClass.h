#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EWeaponClass.generated.h"

UENUM(BlueprintType)
enum class EWeaponClass : uint8
{
	UNARMED = 0				UMETA(DisplayName = "Unarmed"),
	SWORD = 1				UMETA(DisplayName = "Sword"),
	AXE = 2					UMETA(DisplayName = "Axe"),
	HAMMER = 3				UMETA(DisplayName = "Hammer"),
	PIKE = 4				UMETA(DisplayName = "Pike"),
	HANDGUN = 5				UMETA(DisplayName = "Handgun"),
	SHOTGUN = 6				UMETA(DisplayName = "Shotgun"),
	RIFLE = 7				UMETA(DisplayName = "Rifle"),
	SHIELD = 8				UMETA(DisplayName = "Shield"),
	NONE					UMETA(DisplayName = "None")
};