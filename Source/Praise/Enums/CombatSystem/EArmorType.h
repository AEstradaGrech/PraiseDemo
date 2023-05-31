#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EArmorType.generated.h"

UENUM(BlueprintType)
enum class EArmorType : uint8
{
	NONE	= 0					UMETA(DisplayName = "None"),
	LIGHT	= 1					UMETA(DisplayName = "Light"),
	MID		= 2					UMETA(DisplayName = "Mid"),
	HEAVY	= 3					UMETA(DisplayName = "Heavy")
};