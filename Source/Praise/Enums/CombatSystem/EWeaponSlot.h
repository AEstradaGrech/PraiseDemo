#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EWeaponSlot.generated.h"

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	PRIMARY_WEAPON			UMETA(DisplayName="Primary Weapon"),
	SECONDARY_WEAPON		UMETA(DisplayName="Secondary Weapon"),
	EXTRA_SLOT_1			UMETA(DisplayName="Extra Slot 1"),
	EXTRA_SLOT_2			UMETA(DisplayName="Extra Slot 2"),
	INVALID					UMETA(DisplayName="None / Invalid")
};