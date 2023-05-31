#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "FArmorsDTStruct.generated.h"

USTRUCT()
struct PRAISE_API FArmorsDTStruct : public FTableRowBase
{
	GENERATED_BODY();
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		int32 ModifierType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		uint8 ArmorType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		FString ArmorModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		bool IsFullSet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		int32 Lvl;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		bool IsConfigBased;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		bool AppliesOnConfig;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
	TArray<int32> ArmorModifiers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
	TArray<int32> ArmorAbilities;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
	int32 ArmorSkinsID;
};