// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../Structs/CombatSystem/Weapons/FWeaponDTStruct.h"
#include "WeaponConfig.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UWeaponConfig : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		float BaseXP;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		float XPToLevelUp;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		float Damage;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		float CritDamage;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		float CritChance;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		float Weight;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		float AttackRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		float WeaponState;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		float Durability;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		float StateReductionConst;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		float ChanceToBreak;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		float MeleeDistanceOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly )
		FString Description;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		TArray<FString> CurrentMods;

	virtual void UpdateConfig(FWeaponDTStruct* NewStats);
};
