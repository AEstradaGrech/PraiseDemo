#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EGameMenu.generated.h"

UENUM(BlueprintType)
enum class EGameMenu : uint8
{
	MAIN				UMETA(DisplayName = "Main Menu"),
	INGAME				UMETA(DisplayName = "InGame Menu"),
	ENDGAME				UMETA(DisplayName = "End Game Menu")
	
};