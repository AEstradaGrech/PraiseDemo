#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "ECharStatus.generated.h"

UENUM(BlueprintType)
enum class ECharStatus : uint8
{
	NORMAL				UMETA(DisplayName = "Normal"),
	ALERTED				UMETA(DisplayName = "Alerted"),
	DAMAGED				UMETA(DisplayName = "Damaged"),
	VERY_DAMAGED		UMETA(DisplayName = "Very Damaged"),
	STUNNED				UMETA(DisplayName = "Stunned"),
	BLINDED				UMETA(DisplayName = "Blinded"),
	SCARED				UMETA(DisplayName = "Scared"),
	IN_PANIC			UMETA(DisplayName = "In Panic")
};