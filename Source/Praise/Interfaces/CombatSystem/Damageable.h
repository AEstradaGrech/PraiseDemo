// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../../Enums/CombatSystem/EAttackType.h"
#include "../../Enums/CombatSystem/EWeaponSlot.h"

#include "../../Weapons/Weapon.h"
#include "Damageable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PRAISE_API IDamageable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void GetDamage(float Damage, class AActor* Damager, EAttackType AttackType = EAttackType::PRIMARY_ATTACK) = 0;
	virtual void GetWeaponDamage(float Damage, EAttackType AttackType, EWeaponSlot AtackerHand, class ABasePraiseCharacter* Damager, AWeapon* DamagerWeapon) = 0;
	virtual void EnableStatsBar(bool bEnable) = 0;
	virtual bool HasBeenRecentlyDamaged() = 0;
	virtual bool IsDead() const = 0;
};
