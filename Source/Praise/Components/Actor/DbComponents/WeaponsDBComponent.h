// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "../../../Structs/CombatSystem/Weapons/FFiregunDTStruct.h"
#include "../../../Structs/CombatSystem/Weapons/FWeaponArtDTStruct.h"
#include "../../../Structs/CombatSystem/Weapons/FWeaponAbilitiesDTStruct.h"
#include "../../../Structs/CombatSystem/Weapons/FWeaponFxDTStruct.h"
#include "../../../Structs/CombatSystem/Weapons/FMeleeWeaponDTStruct.h"
#include "../../../Enums/CombatSystem/EWeaponType.h"
#include "../../../Enums/CombatSystem/EWeaponClass.h"
#include "WeaponsDBComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRAISE_API UWeaponsDBComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponsDBComponent();

	//POINTED MELEE WEAPONS
	FORCEINLINE class UDataTable* GetPointedWeaponsDT() const { return PointedWeaponsDT; }
	FORCEINLINE class UDataTable* GetPointedWeaponConfigsDT() const { return PointedWeaponConfigsDT; }
	FORCEINLINE class UDataTable* GetPointedWeaponModifiers() const { return PointedWeaponModifiersDT; }
	// FIREARMS
	FORCEINLINE class UDataTable* GetFiregunsDT() const { return FiregunsDT; }
	FORCEINLINE class UDataTable* GetFiregunConfigsDT() const { return FiregunConfigsDT; }
	FORCEINLINE class UDataTable* GetFiregunModifiers() const { return FiregunModifiersDT; }

	FORCEINLINE class UDataTable* GetUnarmedDT() const { return FistWeaponsDT; }


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	FWeaponDTStruct* GetTemplateForWeaponType(EWeaponType Type);
	//UNARMED
	FWeaponDTStruct* GetDefaultFistWeapon();
	//FIREARMS
	TArray<FWeaponDTStruct*> GetAll_FirearmDT() const;
	TArray<FWeaponDTStruct*> GetAll_FistsDT() const;
	TArray<FWeaponDTStruct*> GetAll_PointedWeaponsDT() const;
	TArray<FWeaponDTStruct*> GetAll_BluntWeaponsDT() const;
	TArray<FWeaponDTStruct*> GetAll_SharpWeaponsDT() const;
	TArray<FWeaponDTStruct*> GetAll_ShieldsDT() const;

	TArray<FWeaponDTStruct*> GetWeaponsDTByType(EWeaponType Type);
	TArray<FWeaponDTStruct*> GetWeaponsDTByClass(EWeaponClass Class);
	TArray<FWeaponDTStruct*> GetWeaponsConfigDTByType(EWeaponType Type) const;
	TArray<FWeaponDTStruct*> GetWeaponsConfigDTByClass(EWeaponClass Class = EWeaponClass::UNARMED);

	TArray<FWeaponDTStruct*> GetWeaponsConfigDTByModel(EWeaponClass Class, FString WeaponModel);

	FWeaponDTStruct* GetWeaponsConfigDTByClassAndId(EWeaponClass Class, int32 ConfigId) const;
	FWeaponDTStruct* GetWeaponModsByClassAndId(EWeaponClass Class, int32 ModId) const;
	FWeaponDTStruct* GetWeaponByClassAndModel(EWeaponClass Class, FString WeaponModel) const;
	FWeaponDTStruct* GetWeaponByTypeAndModel(EWeaponType Type, FString WeaponModel) const;
	//COMMON
	FWeaponArtDTStruct* GetWeaponArtDataById(int32 WeaponArtID);
	FWeaponAbilitiesDTStruct* GetWeaponAbilityDataById(int32 AbilityID);
	FWeaponFxDTStruct* GetWeaponFxDataById(int32 FxDataID);

private:
	void LoadDataTables();
	FWeaponDTStruct* FindResultByModel(TArray<FWeaponDTStruct*> Rows, FString WeaponModel) const;
	//UNARMED
	class UDataTable* FistWeaponsDT;
	class UDataTable* FistWeaponConfigsDT;
	class UDataTable* FistWeaponModifiersDT;
	//POINTED WEAPONS
	class UDataTable* PointedWeaponsDT;
	class UDataTable* PointedWeaponConfigsDT;
	class UDataTable* PointedWeaponModifiersDT;
	//BLUNT WEAPONS
	class UDataTable* BluntWeaponsDT;
	class UDataTable* BluntWeaponConfigsDT;
	class UDataTable* BluntWeaponModifiersDT;
	//SHARP WEAPONS
	class UDataTable* SharpWeaponsDT;
	class UDataTable* SharpWeaponConfigsDT;
	class UDataTable* SharpWeaponModifiersDT;
	//SHIELD
	class UDataTable* ShieldsDT;
	class UDataTable* ShieldsConfigsDT;
	class UDataTable* ShieldsModifiersDT;
	//FIREARM
	class UDataTable* FiregunsDT;
	class UDataTable* FiregunConfigsDT;
	class UDataTable* FiregunModifiersDT;
	class UDataTable* WeaponArtDT;
	class UDataTable* WeaponAbilitiesDT;
	class UDataTable* WeaponFxDT;
};
