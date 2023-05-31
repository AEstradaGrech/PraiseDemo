#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EEnvQueryCheck.generated.h"

UENUM(BlueprintType)
enum class EEnvQueryCheck : uint8
{
	GENERIC					UMETA(DisplayName = "Generic"),
	VISIBILITY				UMETA(DisplayName = "Visibility"),
	DISTANCE				UMETA(DisplayName = "Distance")
};