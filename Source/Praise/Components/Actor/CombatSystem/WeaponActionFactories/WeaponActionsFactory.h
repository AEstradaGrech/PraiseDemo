// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "../../../../Weapons/Weapon.h"
#include "../../../../Characters/BasePraiseCharacter.h"
/**
 * 
 */
class PRAISE_API WeaponActionsFactory
{
public:
	WeaponActionsFactory();
	~WeaponActionsFactory();

	virtual ABasePraiseCharacter* GetFactoryOwner() const = 0;

	void UpdateFactoryType();
private:
	
	ABasePraiseCharacter* FactoryOwner;
};
