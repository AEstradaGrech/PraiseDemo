#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "FProcessTerminateState.generated.h"

USTRUCT()
struct PRAISE_API FProcessTerminateState
{
	GENERATED_BODY()

public:

	FProcessTerminateState()
	{
		Status = false;
	};

	UPROPERTY()
		bool Status;

	long TerminationTime;
};