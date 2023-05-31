#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "ECurveType.generated.h"

UENUM(BlueprintType)
enum class ECurveType : uint8
{
	LINEAR					UMETA(DisplayName = "Linear"),
	LOG						UMETA(DisplayName = "Log"),
	ANTILOG					UMETA(DisplayName = "Anti-Log"),
	S_LOG					UMETA(DisplayName = "Log S"),
	S_ANTILOG				UMETA(DisplayName = "Anti-Log S"),
	EQUIP_SHOULDER_1H		UMETA(DisplayName = "Equip Shoulder 1H")
};