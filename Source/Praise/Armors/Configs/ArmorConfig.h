// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../Structs/CombatSystem/Armors/FArmorsDTStruct.h"
#include "ArmorConfig.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UArmorConfig : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		float BaseXP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		float Absorption;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		float Weight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		float ArmorState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		float StateReductionConst;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		float RemainingDmgReductionMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		float Durability;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		float ChanceToBreak;

	virtual void UpdateConfig(FArmorsDTStruct* Config);
};
