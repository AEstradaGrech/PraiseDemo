#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EEnvQueryMode.generated.h"

UENUM(BlueprintType)
enum class EEnvQueryMode : uint8
{
	PLANE					UMETA(DisplayName = "Plane"),
	SPHERIC					UMETA(DisplayName = "Spheric"),
	CYLINDRIC				UMETA(DisplayName = "Cylindric")
};