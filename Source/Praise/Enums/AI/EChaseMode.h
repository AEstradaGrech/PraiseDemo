#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EChaseMode.generated.h"

UENUM(BlueprintType)
enum class EChaseMode : uint8 {
	FOLLOW			UMETA(DisplayName = "Follow"),
	PURSUE			UMETA(DisplayName = "Pursue"),
	SEEK			UMETA(DisplayName = "Seek")
};
