#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EMiscSFX.generated.h"

UENUM(BlueprintType)
enum class EMiscSFX : uint8
{
	FOOTSTEPS			UMETA(DisplayName = "FootSteps"),
	DRINK				UMETA(DisplayName = "Drink"),
	JUMP				UMETA(DisplayName = "Jump"),
	BONFIRE				UMETA(DisplayName = "Bonfire"),
	TORCH_FIRE			UMETA(DisplayName = "Torch Fire"),
	NONE	= 255		UMETA(DisplayName = "None")
};