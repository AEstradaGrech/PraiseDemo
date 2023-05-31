#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EBrainType.generated.h"

UENUM(BlueprintType)
enum class EBrainType : uint8 {
	DEFAULT				UMETA(DisplayName = "Default"), 
	T200				UMETA(DisplayName = "T-200"),
	BT_BRAIN			UMETA(DisplayName = "Behavior Tree")
};
