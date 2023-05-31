// Fill out your copyright notice in the Description page of Project Settings.


#include "FirearmConfig.h"

void UFirearmConfig::UpdateConfig(FWeaponDTStruct* NewStats)
{
	Super::UpdateConfig(NewStats);

	FFiregunDTStruct* NewGunStats = (FFiregunDTStruct*)NewStats;

	Range = NewGunStats->Range;
	ReloadTime = NewGunStats->ReloadTime;
	MaxAmmo = NewGunStats->MaxAmmo;
	MaxMagAmmo = NewGunStats->MaxMagAmmo;
	SpreadCorrectionPercentage = NewGunStats->SpreadCorrectionPercentage;
	HorizontalSpreadDegs = NewGunStats->HorizontalSpreadDegs;
	VerticalSpreadDegs = NewGunStats->VerticalSpreadDegs;
}