#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "ECharFaction.generated.h"

UENUM(BlueprintType)
enum class ECharFaction : uint8
{
	NONE				UMETA(DisplayName = "None"),		
	PROTECTOR			UMETA(DisplayName = "Protector"),	
	PUBLIC_ENEMY		UMETA(DisplayName = "Public Enemy"),
	CREATURE			UMETA(DisplayName = "Creature"),	
	// Faith related factions
	CREED				UMETA(DisplayName = "Credo"),
	CZORT				UMETA(DisplayName = "Czort"),
	PAGAN				UMETA(DisplayName = "Pagano"),
	BAAL				UMETA(DisplayName = "Baal"),
	// Other factions
	BOTVILLE			UMETA(DisplayName = "BotVille")
};