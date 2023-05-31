#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "FStartGameSessionState.generated.h"

USTRUCT()
struct PRAISE_API FStartGameSessionState
{
	GENERATED_BODY()

public:

	FStartGameSessionState()
	{
		Status = false;
	};

	UPROPERTY()
		bool Status;
};