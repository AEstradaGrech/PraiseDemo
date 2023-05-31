// Fill out your copyright notice in the Description page of Project Settings.


#include "ArmorConfig.h"

void UArmorConfig::UpdateConfig(FArmorsDTStruct* Config)
{
	BaseXP = Config->BaseXP;
	Absorption = Config->Absorption;
	Weight = Config->Weight;
	ArmorState = Config->ArmorState;
	Durability = Config->Durability;
	ChanceToBreak = Config->ChanceToBreak;
	StateReductionConst = Config->StateReductionConst;
	RemainingDmgReductionMult = Config->RemainingDmgReductionMult;
}
