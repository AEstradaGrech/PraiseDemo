#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EBotClass.generated.h"

UENUM(BlueprintType)
enum class EBotClass : uint8 {
	CITIZEN				UMETA(DisplayName = "Citizen Bot"),
	FIGHTER				UMETA(DisplayName = "Fighter Bot"),
	UNDEAD				UMETA(DisplayName = "Undead")
};
