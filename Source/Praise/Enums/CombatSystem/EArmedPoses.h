#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EArmedPoses.generated.h"

UENUM(BlueprintType)
enum class EArmedPoses : uint8
{
	UNARMED				UMETA(DisplayName = "Unarmed"),
	ONE_HAND			UMETA(DisplayName = "One Hand"),
	TWO_HAND_SWORD		UMETA(DisplayName = "Two Hand Sword"),
	TWO_HAND_AXE		UMETA(DisplayName = "One Hand Axe"),
	TWO_HAND_HAMMER		UMETA(DisplayName = "Two Hand Hammer"),
	HARPOON				UMETA(DisplayName = "Harpoon"),
	TWO_HAND_PIKE		UMETA(DisplayName = "Two Hand Pike"),
	RIFLE				UMETA(DisplayName = "Rifle"),	
	DUALS				UMETA(DisplayName = "Duals")
};
