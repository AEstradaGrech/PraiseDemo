#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EWanderMode.generated.h"

UENUM(BlueprintType)
enum class EWanderMode : uint8 {
	RANDOM			UMETA(DisplayName = "Random"),
	INSPECT			UMETA(DisplayName = "Inspect")
};
