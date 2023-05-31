#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EAttackType.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	NONE					UMETA(DisplayName = "None"),
	PRIMARY_ATTACK			UMETA(DisplayName = "Primary"),
	SECONDARY_ATTACK		UMETA(DisplayName = "Secondary"),
	STRONG_ATTACK			UMETA(DisplayName = "Strong"),
	SPRINT_ATTACK			UMETA(DisplayName = "Sprint Attack"),
	KICK					UMETA(DisplayName = "Kick"),
	EXECUTION				UMETA(DisplayName = "Execution"),
	BREAK_GUARD				UMETA(DisplayName = "Break Guard"),
	PARRY					UMETA(DisplayName = "Parry")
};