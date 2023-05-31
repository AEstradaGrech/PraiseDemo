#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EWeaponType.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	UNARMED					UMETA(DisplayName = "Unarmed"),
	SHARP					UMETA(DisplayName = "Sharp"),		
	BLUNT					UMETA(DisplayName = "Blunt"),		
	POINTED					UMETA(DisplayName = "Pointed"),		
	FIREARM					UMETA(DisplayName = "Firearm"),
	SHIELD					UMETA(DisplayName = "Shield"),
	NONE					UMETA(DisplayName = "None")
};