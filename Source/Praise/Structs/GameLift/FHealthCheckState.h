#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "FHealthCheckState.generated.h"

USTRUCT()
struct PRAISE_API FHealthCheckState
{
	GENERATED_BODY()

public:

	FHealthCheckState()
	{
		Status = false;
	};

	UPROPERTY()
		bool Status;
};