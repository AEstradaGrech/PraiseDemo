// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../../Enums/CombatSystem/EArmedPoses.h"
#include "../../Enums/CombatSystem/EAttackType.h"
#include "../../Enums/CombatSystem/EWeaponSlot.h"
#include "../../Enums/CommonUtility/ECharVector.h"
#include "../../Inventory/CharWeaponSlot.h"
#include "Combatible.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatible : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PRAISE_API ICombatible
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual float GetHealth() const = 0;
	virtual float GetStamina() const = 0;
	virtual bool CanAttack() const = 0;
	virtual bool IsAttacking() const = 0;
	virtual bool IsEquiping() const = 0;				 
	virtual bool IsBlocking() const = 0;
	virtual bool IsParring() const = 0;
	virtual bool IsAiming() const = 0;
	virtual bool IsTargeting() const = 0;
	virtual bool IsRolling() const = 0;
	virtual bool IsEvading() const = 0;
	virtual bool IsJumping() const = 0;					 
	virtual bool IsRunning() const = 0;
	virtual bool IsCrouching() const = 0;
	virtual bool IsWeaponEquiped() const = 0;		 
	virtual bool HasWeapon() const = 0;					
	virtual EArmedPoses GetCurrentPose() const = 0;
	virtual EAttackType GetCurrentAttackType() const = 0;
	virtual TMap<EWeaponSlot, UCharWeaponSlot*> GetCharCurrentWeapons() const = 0;

	virtual bool IsTargetInsideFOV(AActor* Target, bool bCheckCol, float Angle = 0.f) = 0;
	virtual bool IsInCombatRange(AActor* Opponent, float MaxDistanceOffset) const = 0;
	virtual bool IsInMeleeRange(AActor* Opponent, float MaxDistanceOffset = 0.f) const = 0;
	virtual float GetMeleeDistanceOffset() const = 0;
	virtual bool IsBeingDamaged() const = 0;
	virtual ECharVector GetTargetLocationVector(AActor* Target) const = 0;
};
