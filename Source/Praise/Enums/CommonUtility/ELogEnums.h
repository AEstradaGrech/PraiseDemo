#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "ELogEnums.generated.h"

UENUM(BlueprintType)
enum class ELogLevel : uint8 {
	TRACE	UMETA(DisplayName = "Trace"),
	DEBUG	UMETA(DisplayName = "Debug"),
	INFO	UMETA(DisplayName = "Info"),
	WARNING UMETA(DisplayName = "Warning"),
	ERROR	UMETA(DisplayName = "Error")
};

UENUM(BlueprintType)
enum class ELogOutput : uint8 {
	ALL			UMETA(DisplayName = "All Levels"),
	OUTPUT_LOG	UMETA(DisplayName = "Output Log"),
	SCREEN		UMETA(DisplayName = "Screen")
};

