// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../../Weapons/Weapon.h"

/**
 * 
 */
class PRAISE_API WeaponAction
{
public:
	WeaponAction();
	~WeaponAction();

	virtual AWeapon* GetActionWeapon() const = 0;

private:

	AWeapon* ActionWeapon;

	bool bHasIK;
	bool bCanMove;
	
};
