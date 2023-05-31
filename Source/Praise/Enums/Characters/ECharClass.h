#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "ECharClass.generated.h"

UENUM(BlueprintType)
enum class ECharClass : uint8
{
	NONE				UMETA(DisplayName = "None"),
	CREATURE_HUNTER		UMETA(DisplayName = "Creature Hunter"),
	ACOLYT				UMETA(DisplayName = "Acolyt")
};