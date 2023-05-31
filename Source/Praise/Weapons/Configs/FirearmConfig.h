// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponConfig.h"
#include "../../Structs/CombatSystem/Weapons/FFiregunDTStruct.h"
#include "FirearmConfig.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UFirearmConfig : public UWeaponConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Configuration)
		float Range;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Configuration)
		int MaxAmmo;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Configuration)
		int MaxMagAmmo;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Configuration)
		float SpreadCorrectionPercentage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Configuration)
		float ReloadTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Configuration)
		float HorizontalSpreadDegs = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Configuration)
		float VerticalSpreadDegs = 0.f;

	virtual void UpdateConfig(FWeaponDTStruct* NewStats);
};
