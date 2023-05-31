#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "ECombatSFX.generated.h"

UENUM(BlueprintType)
enum class ECombatSFX : uint8
{
	WEAPON_EQUIP		UMETA(DisplayName="Weapon Equip"),
	WEAPON_UNEQUIP		UMETA(DisplayName="Weapon Unequip"),
	WEAPON_DROP			UMETA(DisplayName="Drop Weapon"),
	WEAPON_PICK			UMETA(DisplayName="Pick Weapon"),
	WEAPON_BLOCK		UMETA(DisplayName ="Weapon Block"),
	WEAPON_PARRY		UMETA(DisplayName="Weapon Parry"),
	WEAPON_HIT			UMETA(DisplayName="Weapon Hit"), 
	WEAPON_ATTACK		UMETA(DisplayName="Weapon Attack"), 
	WEAPON_BROKEN		UMETA(DisplayName="Weapon Broken"),
	NONE = 255		    UMETA(DisplayName = "None")
	
};

