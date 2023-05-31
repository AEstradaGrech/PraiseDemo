#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "ECharAction.generated.h"

UENUM(BlueprintType)
enum class ECharAction : uint8
{
	RUN					UMETA(DisplayName = "Run"),
	ROLL				UMETA(DisplayName = "Roll"),
	ATTACK				UMETA(DisplayName = "Attack"),
	BLOCK				UMETA(DisplayName = "Block"),
	JUMP				UMETA(DisplayName = "Jump"),
	PARRY				UMETA(DisplayName = "Parry"),
	EVADE				UMETA(DisplayName = "Evade"),
	EXECUTE				UMETA(DisplayName = "Execute"),
	TRIGGER_ABILITY		UMETA(DisplayName = "Trigger Ability"),
	DROP_WEAPON			UMETA(DisplayName = "Drop Weapon"),
	EQUIP_WEAPON		UMETA(DisplayName = "Equip Weapon"),
	PICKUP_ITEM			UMETA(DisplayName = "Pickup Item")
};