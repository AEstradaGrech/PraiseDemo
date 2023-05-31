#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EMachineState.generated.h"

UENUM(BlueprintType)
enum class EMachineState : uint8
{
	IDLE	UMETA(DisplayName = "Idle"),
	PATROL  UMETA(DisplayName = "Patrol"),
	CHASE   UMETA(DisplayName = "Chase"),
	ATTACK  UMETA(DisplayName = "Atack"),
	EVADE   UMETA(DisplayName = "Evade"),
	SNITCH  UMETA(DisplayName = "Snitch"),
	HIDE    UMETA(DisplayName = "Hide"),
	WANDER  UMETA(DisplayName = "Wander"),
	RUNAWAY	UMETA(DisplayName = "Runaway"),
	COMBAT	UMETA(DisplayName = "Combat"),
	TRAVEL	UMETA(DiplayName = "Travel")
};