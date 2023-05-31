#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "ETargetType.generated.h"

UENUM(BlueprintType)
enum class ETargetType : uint8
{
	PLAYER		UMETA(DisplayName="Player Character"),
	NPC			UMETA(DiplayName="Non-Playable Character"),
	ITEM		UMETA(DisplayName="Collectable Item"),
	VIP			UMETA(DisplayName="Very Important Place")
};
