#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EWeaponFX.generated.h"

UENUM(BlueprintType)
enum class EWeaponFX  : uint8
{
	MuzzleFX = 0,
	DefaultImpactFX = 1,
	TargetImpactFX = 2,
	ShootBeamFX = 3,
	BulletShellFX = 4
};
