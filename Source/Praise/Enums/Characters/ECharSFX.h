#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "ECharSFX.generated.h"

UENUM(BlueprintType)
enum class ECharSFX : uint8
{
	CHAR_ATTACK			UMETA(DisplayName = "Character Attack"),
	CHAR_BLOCK			UMETA(DisplayName = "Character Block"),
	CHAR_DAMAGED		UMETA(DisplayName = "Character Damaged"),
	CHAR_DEAD			UMETA(DisplayName = "Character Dead"),
	CHAR_EVADE			UMETA(DisplayName = "Character Evade"),
	CHAR_TAUNT			UMETA(DisplayName = "Character Taunt"),
	CHAR_BACKUP_REQ		UMETA(DisplayName = "Character Backup Req"),
	NONE = 255		    UMETA(DisplayName = "None")

};
