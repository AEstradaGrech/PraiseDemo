#pragma once
#include "Engine/Engine.h"
#include "CoreMinimal.h"
#include "ECharVector.generated.h"

UENUM(BlueprintType)
enum class ECharVector : uint8 {
	NONE		UMETA(DisplayName = "None"),
	FWD			UMETA(DisplayName = "Forward"),
	BWD			UMETA(DisplayName = "Backward"),
	LEFT		UMETA(DisplayName = "Left"),
	RIGHT		UMETA(DisplayName = "Right"),
	FWD_L		UMETA(DisplayName = "Left Forward"),
	FWD_R		UMETA(DisplayName = "Right Forward"),
	BWD_L		UMETA(DisplayName = "Left Backward"),
	BWD_R		UMETA(DisplayName = "Right Backward")
};