#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "ECharType.generated.h"

UENUM(BlueprintType)
enum class ECharType : uint8
{
	HUMAN_MALE			UMETA(DisplayName = "Male Human"),
	DEMON				UMETA(DisplayName = "Demon"),
	UNDEAD				UMETA(DisplayName = "Undead"),
	HUMAN_FEMALE		UMETA(DisplayName = "Female Human")
};