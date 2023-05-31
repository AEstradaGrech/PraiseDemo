// Fill out your copyright notice in the Description page of Project Settings.


#include "Fist.h"

AFist::AFist() : Super()
{
	WeaponType = EWeaponType::UNARMED;
	WeaponModel = "Default";
	
}

void AFist::BeginPlay()
{
}

void AFist::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	bDidInit = TryInitStats();
}
