#pragma once
#include "Engine/Engine.h"
#include "CoreMinimal.h"
#include "FNeuronConnection.generated.h"

USTRUCT()
struct PRAISE_API FNeuronConnection
{
	GENERATED_BODY()

	float Weight;
	float DeltaWeight;
};
