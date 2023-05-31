#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "ECharAnimState.generated.h"

UENUM(BlueprintType)
enum class ECharAnimState : uint8
{
	IDLE				UMETA(DisplayName = "Idle"),
	ATTACKING			UMETA(DisplayName = "Attacking"),
	BLOCKING			UMETA(DisplayName = "Blocking"),
	EQUIPING			UMETA(DisplayName = "Equiping"),
	JUMPING				UMETA(DisplayName = "Jumping"),
	ROLLING				UMETA(DisplayName = "Rolling"),
	EVADING				UMETA(DisplayName = "Evading"),
	PARRING				UMETA(DisplayName = "Parring"),
	PICKING_ITEM		UMETA(DisplayName = "Picking Item"),
	AIMING				UMETA(DisplayName = "Aiming"),
	RELOADING			UMETA(DisplayName = "Reloading"),
	ROAR				UMETA(DisplayName = "Roar")
};
