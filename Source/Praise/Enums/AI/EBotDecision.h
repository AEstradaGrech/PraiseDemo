#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EBotDecision.generated.h"

UENUM(BlueprintType)
enum class EBotDecision : uint8 {
	AUTODESTROY				UMETA(DisplayName = "Autodestroy"),
	STAY_IN_IDLE			UMETA(DisplayName = "Stay in Idle"),
	BOOLEAN					UMETA(DisplayName = "Generic boolean"),
	LOCOMOTION				UMETA(DisplayName = "Generic Locomotion"),
	START_PATROL			UMETA(DisplayName = "Start Patrol"),
	RUN_AWAY				UMETA(DisplayName = "Run away"),
	WANDER_AROUND			UMETA(DisplayName = "Wander around"),
	CHASE_TARGET			UMETA(DisplayName = "Chase Target"),
	GO_HIDE					UMETA(DisplayName = "Go hide"),
	INSPECT_AREA			UMETA(DisplayName = "Inspect Area"),
	ATTACK					UMETA(DisplayName = "Attack Target"),
	BLOCK					UMETA(DisplayName = "Block"),
	EVADE					UMETA(DisplayName = "Evade"),
	SPRINT					UMETA(DisplayName = "Sprint"),
	SEEK					UMETA(DisplayName = "Seek"),
	SELECT_TARGET			UMETA(DisplayName = "Select Target"),
	ENGAGE_COMBAT			UMETA(DisplayName = "Engage Combat"),
	EQUIP_WEAPON			UMETA(DisplayName = "EquipWeapon"),
	LOCK_TARGET				UMETA(DisplayName = "Lock Target"),
	SNITCH					UMETA(DisplayName = "Snitch Location"),
	UNBLOCK_PATH			UMETA(DisplayName = "Unblock Path")
};
