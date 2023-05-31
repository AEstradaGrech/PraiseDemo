// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponConfig.h"

void UWeaponConfig::UpdateConfig(FWeaponDTStruct* NewStats)
{
	BaseXP = NewStats->BaseXP;
	Damage = NewStats->Damage;
	Weight = NewStats->Weight;
	AttackRate = NewStats->AttackRate;
	WeaponState = NewStats->WeaponState;
	Durability = NewStats->Durability;
	CritDamage = NewStats->CritDamage;
	CritChance = NewStats->CritChance;
	ChanceToBreak = NewStats->ChanceToBreak;
	MeleeDistanceOffset = NewStats->MeleeDistanceOffset;
	StateReductionConst = NewStats->StateReductionConst;
}