#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EHolsterPosition.generated.h"

UENUM(BlueprintType)
enum class EHolsterPosition : uint8
{
	UNARMED				UMETA(DisplayName = "Unarmed"),
	SHOULDER_R			UMETA(DisplayName = "Right Shoulder"),
	LOW_BACK_L			UMETA(DisplayName = "Low Back L"),
	LOW_BACK_R			UMETA(DisplayName = "Low Back R"),
	THIGH_R				UMETA(DisplayName = "Thigh R"), 
	CHEST_L				UMETA(DisplayName = "Chest L"),
	LOW_CHEST_L			UMETA(DisplayName = "Low Chest L"),
	HOLSTER_R			UMETA(DisplayName = "Right Hand Holster"),
	THIGH_L				UMETA(DisplayName = "Thigh L"), 
	HOLSTER_L			UMETA(DisplayName = "Left Hand Holster")
};
