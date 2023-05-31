#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "../../../Enums/CombatSystem/EArmedPoses.h"
#include "../../../Enums/Animation/EHolsterPosition.h"
#include "FWeaponDTStruct.generated.h"

USTRUCT()
struct PRAISE_API FWeaponDTStruct : public FTableRowBase
{
	GENERATED_BODY();
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		int32 ModifierType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		uint8 WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		uint8 WeaponClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString WeaponModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		EArmedPoses ArmedPose;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		bool IsTwoHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		int32 Lvl;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float BaseXP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float CritDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float CritChance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float Weight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float AttackRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float WeaponState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float StateReductionConst;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float Durability;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float ChanceToBreak;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		bool IsConfigBased;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		bool AppliesOnConfig;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float MeleeDistanceOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		bool CanEquipLeftHandOnly;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TArray<int32> Modifiers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TArray<int32> WeaponAbilities;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TArray<int32> WeaponSkins;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TArray<int32> WeaponFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TArray<EHolsterPosition> HolsterPositions;
};