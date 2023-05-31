#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EActivationFunction.generated.h"

UENUM(BlueprintType)
enum class EActivationFunction : uint8 {
	SIGMOIDAL,
	TANH,
	ReLU,
	LEAKY_ReLU
};
