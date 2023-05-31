#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "../../Enums/AI/EMachineState.h"
#include "FDecisionOutput.generated.h"

USTRUCT()
struct PRAISE_API FDecisionOutput
{
	GENERATED_BODY()

	bool bIsTransitionDecision = false;
	EMachineState OutputState;
	TFunction<void(const bool)> DecisionCallback;
	bool bCallbackResult;
};