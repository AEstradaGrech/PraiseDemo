// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponsDBComponent.h"
#include "../../../Structs/CombatSystem/Weapons/FFiregunDTStruct.h"
#include "Engine/DataTable.h"
#include "WeaponsDBComponent.h"
#include "../../../Praise.h"
#include "../../../Structs/CommonUtility/FLogger.h"
#include "../../../Structs/CommonUtility/FUtilities.h"
#include <exception>

// Sets default values for this component's properties
UWeaponsDBComponent::UWeaponsDBComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	LoadDataTables();
	// ...
}


// Called when the game starts
void UWeaponsDBComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

FWeaponDTStruct* UWeaponsDBComponent::GetTemplateForWeaponType(EWeaponType Type)
{
	FWeaponDTStruct* Template = nullptr;
	static const FString ContextString = FString(" :: GETTING TEMPLATE FOR WEAPON OF TYPE :: ") + FUtilities::EnumToString(*FString("EWeaponType"), (int)Type);
	try
	{
		switch (Type)
		{
		case(EWeaponType::UNARMED):
			Template = ShieldsDT->FindRow<FWeaponDTStruct>(FName("0"), ContextString, true);
			break;
		case(EWeaponType::SHARP):
			Template = SharpWeaponsDT->FindRow<FMeleeWeaponDTStruct>(FName("0"), ContextString, true);
			break;
		case(EWeaponType::BLUNT):
			Template = BluntWeaponsDT->FindRow<FMeleeWeaponDTStruct>(FName("0"), ContextString, true);
			break;
		case(EWeaponType::POINTED):
			Template = PointedWeaponsDT->FindRow<FMeleeWeaponDTStruct>(FName("0"), ContextString, true);
			break;
		case(EWeaponType::FIREARM):
			Template = FiregunsDT->FindRow<FFiregunDTStruct>(FName("0"), ContextString, true);
			break;
		case(EWeaponType::SHIELD):
			Template = ShieldsDT->FindRow<FMeleeWeaponDTStruct>(FName("0"), ContextString, true);
			break;
		case(EWeaponType::NONE):
		default:
			break;
		}
	}
	catch (std::exception e)
	{
		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION MSG --> ") + e.what());
	}
	

	return Template;
}

FWeaponDTStruct* UWeaponsDBComponent::GetDefaultFistWeapon()
{
	static const FString ContextString = FString(" :: GETTING ALL DATA FOR FIREGUN :: ");
	try {
		FWeaponDTStruct* DataRow = FistWeaponsDT->FindRow<FWeaponDTStruct>(FName("0"), ContextString, true);

		return (DataRow && DataRow->WeaponModel == "Default") ? DataRow : nullptr;
	}
	catch (std::exception e) {
		FLogger::LogTrace(__FUNCTION__ + FString(e.what()));
	}
	return nullptr;
}

TArray<FWeaponDTStruct*> UWeaponsDBComponent::GetAll_FirearmDT() const
{
	
	static const FString ContextString = FString(" :: GETTING ALL DATA FOR FIREGUN :: ");
	TArray<FWeaponDTStruct*> Results;
	TArray<FFiregunDTStruct*> Rows;
	try {

		if (!FiregunsDT) return TArray<FWeaponDTStruct*>();

		FiregunsDT->GetAllRows<FFiregunDTStruct>(ContextString, Rows);

		for (FFiregunDTStruct* row : Rows)
			Results.Add(row);

	}
	catch (std::exception e) {

		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION GETTING ALL DATA FOR FIREGUN DT ") + FString(" :: EXCEPTION MSG :: ") + FString(e.what()));

		return Results;
	}

	return Results;
}

TArray<FWeaponDTStruct*> UWeaponsDBComponent::GetAll_FistsDT() const
{
	
	static const FString ContextString = FString(" :: GETTING ALL DATA FOR FIREGUN :: ");
	TArray<FWeaponDTStruct*> Results;
	TArray<FWeaponDTStruct*> Rows;
	try {
		if (!FistWeaponsDT) return TArray<FWeaponDTStruct*>();

		FistWeaponsDT->GetAllRows<FWeaponDTStruct>(ContextString, Rows);

		for (FWeaponDTStruct* row : Rows)
			Results.Add(row);
	}
	catch (std::exception e) {

		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION GETTING ALL DATA FOR FIREGUN DT ") + FString(" :: EXCEPTION MSG :: ") + FString(e.what()));

		return Results;
	}

	return Results;
}

TArray<FWeaponDTStruct*> UWeaponsDBComponent::GetAll_PointedWeaponsDT() const
{
	
	static const FString ContextString = FString(" :: GETTING ALL DATA FOR FIREGUN :: ");
	TArray<FWeaponDTStruct*> Results;
	TArray<FMeleeWeaponDTStruct*> Rows;
	try {

		if (!PointedWeaponsDT) return TArray<FWeaponDTStruct*>();

		PointedWeaponsDT->GetAllRows<FMeleeWeaponDTStruct>(ContextString, Rows);

		for (FMeleeWeaponDTStruct* row : Rows)
			Results.Add(row);

	}
	catch (std::exception e) {

		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION GETTING ALL DATA FOR FIREGUN DT ") + FString(" :: EXCEPTION MSG :: ") + FString(e.what()));

		return Results;
	}

	return Results;
}

TArray<FWeaponDTStruct*> UWeaponsDBComponent::GetAll_BluntWeaponsDT() const
{
	
	static const FString ContextString = FString(" :: GETTING ALL DATA FOR BLUNT WEAPONS :: ");
	TArray<FWeaponDTStruct*> Results;
	TArray<FWeaponDTStruct*> Rows;
	try {

		if (!BluntWeaponsDT) return TArray<FWeaponDTStruct*>();

		BluntWeaponsDT->GetAllRows<FWeaponDTStruct>(ContextString, Rows);

		for (FWeaponDTStruct* row : Rows)
			Results.Add(row);

	}
	catch (std::exception e) {

		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION GETTING ALL DATA FOR BLUNT WPNS DT ") + FString(" :: EXCEPTION MSG :: ") + FString(e.what()));

		return Results;
	}

	return Results;
	
}

TArray<FWeaponDTStruct*> UWeaponsDBComponent::GetAll_SharpWeaponsDT() const
{
	static const FString ContextString = FString(" :: GETTING ALL DATA FOR SHARP WEAPONS :: ");
	TArray<FWeaponDTStruct*> Results;
	TArray<FWeaponDTStruct*> Rows;
	try {

		if (!SharpWeaponsDT) return TArray<FWeaponDTStruct*>();

		SharpWeaponsDT->GetAllRows<FWeaponDTStruct>(ContextString, Rows);

		for (FWeaponDTStruct* row : Rows)
			Results.Add(row);

	}
	catch (std::exception e) {

		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION GETTING ALL DATA FOR SHARP WPNS DT ") + FString(" :: EXCEPTION MSG :: ") + FString(e.what()));

		return Results;
	}

	return Results;

}

TArray<FWeaponDTStruct*> UWeaponsDBComponent::GetAll_ShieldsDT() const
{
	static const FString ContextString = FString(" :: GETTING ALL DATA FOR SHIELDS :: ");
	TArray<FWeaponDTStruct*> Results;
	TArray<FWeaponDTStruct*> Rows;
	try {

		if (!ShieldsDT) return TArray<FWeaponDTStruct*>();

		ShieldsDT->GetAllRows<FWeaponDTStruct>(ContextString, Rows);

		for (FWeaponDTStruct* row : Rows)
			Results.Add(row);

	}
	catch (std::exception e) {

		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION GETTING ALL DATA FOR SHIELDS DT ") + FString(" :: EXCEPTION MSG :: ") + FString(e.what()));

		return Results;
	}

	return Results;
}

TArray<FWeaponDTStruct*> UWeaponsDBComponent::GetWeaponsDTByType(EWeaponType Type)
{
	

	TArray<FWeaponDTStruct*> Results = TArray<FWeaponDTStruct*>();
	
	if (Type == EWeaponType::UNARMED) return Results;

	switch (Type)
	{
		case(EWeaponType::FIREARM):
			return GetAll_FirearmDT();
		case(EWeaponType::UNARMED):
		default:
			break;
	}

	return Results;
}

TArray<FWeaponDTStruct*> UWeaponsDBComponent::GetWeaponsDTByClass(EWeaponClass Class)
{
	TArray<FWeaponDTStruct*> Results;

	TArray<FWeaponDTStruct*> Firearms = TArray<FWeaponDTStruct*>();

	switch (Class)
	{
		case EWeaponClass::UNARMED:
			//return All?
			break;
		case EWeaponClass::HANDGUN:
		case EWeaponClass::SHOTGUN:
		case EWeaponClass::RIFLE:
		
			Firearms = GetWeaponsDTByType(EWeaponType::FIREARM);
		
			for (int i = 0; i < Firearms.Num(); i++)
				if (Firearms[i]->WeaponClass == (uint8)Class)
					Results.Add(Firearms[i]);
			break;
		default:
			break;
	}

	return Results;
}

TArray<FWeaponDTStruct*> UWeaponsDBComponent::GetWeaponsConfigDTByClass(EWeaponClass Class)
{
	TArray<FWeaponDTStruct*> Results = TArray<FWeaponDTStruct*>();
	TArray<FFiregunDTStruct*> Weapons = TArray<FFiregunDTStruct*>();
	
	return Results;
}

TArray<FWeaponDTStruct*> UWeaponsDBComponent::GetWeaponsConfigDTByModel(EWeaponClass Class, FString WeaponModel)
{
	

	TArray<FWeaponDTStruct*> Results = TArray<FWeaponDTStruct*>();
	TArray<FFiregunDTStruct*> Weapons = TArray<FFiregunDTStruct*>();
	

	return Results;
}

FWeaponDTStruct* UWeaponsDBComponent::GetWeaponsConfigDTByClassAndId(EWeaponClass Class, int32 ConfigId) const
{
	FString ContextString = "";
	FWeaponDTStruct* Result = nullptr;
	try {
		switch (Class) {
			case(EWeaponClass::UNARMED):
				if (!FistWeaponConfigsDT) return nullptr;
				ContextString = FString(" :: GETTING DATA FOR FIST WEAPON WITH CONFIG ID :: ") + FString::FromInt((uint8)ConfigId);
				Result = FistWeaponConfigsDT->FindRow<FWeaponDTStruct>(*FString::FromInt(ConfigId), ContextString, true);
				break;
			case(EWeaponClass::SWORD):
				break;
			case(EWeaponClass::AXE):
				break;
			case(EWeaponClass::HAMMER):
				break;
			case(EWeaponClass::PIKE):
				break;
			case(EWeaponClass::HANDGUN):
				break;
			case(EWeaponClass::SHOTGUN):
				break;
			case(EWeaponClass::RIFLE):
				if (!FiregunConfigsDT) return nullptr;
				ContextString = FString(" :: GETTING DATA FOR FIREGUN WITH CONFIG ID :: ") + FString::FromInt((uint8)ConfigId);
				Result = FiregunConfigsDT->FindRow<FFiregunDTStruct>(*FString::FromInt(ConfigId), ContextString, true);
				break;
		}

		return Result;
	}
	catch (std::exception e) {

		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION GETTING DATA FOR FIREGUN WITH ID :: ") + FString::FromInt((uint8)Class) + FString(" :: EXCEPTION MSG :: ") + FString(e.what()));

		return nullptr;
	}
	
}

FWeaponDTStruct* UWeaponsDBComponent::GetWeaponModsByClassAndId(EWeaponClass Class, int32 ModId) const
{
	try 
	{
		FString ContextString = "";
		FWeaponDTStruct* Result = nullptr;
		TArray<FWeaponDTStruct*> Rows;
		switch (Class) {
		case(EWeaponClass::UNARMED):
			if (!FistWeaponModifiersDT) return nullptr;
			ContextString = FString(" :: GETTING MODS DATA FOR UNARMED WITH MOD ID :: ") + FString::FromInt((uint8)ModId);
			Result = FistWeaponModifiersDT->FindRow<FWeaponDTStruct>(*FString::FromInt(ModId), ContextString, true);
			return Result ? Result : nullptr;
		case(EWeaponClass::SWORD):
			break;
		case(EWeaponClass::AXE):
			break;
		case(EWeaponClass::HAMMER):
			break;
		case(EWeaponClass::PIKE):
			break;
		case(EWeaponClass::HANDGUN):
			break;
		case(EWeaponClass::SHOTGUN):
			break;
		case(EWeaponClass::RIFLE):
			if (!FiregunModifiersDT) return nullptr;
			ContextString = FString(" :: GETTING MODS DATA FOR RIFLE WITH MOD ID :: ") + FString::FromInt((uint8)ModId);
			Result = FiregunModifiersDT->FindRow<FFiregunDTStruct>(*FString::FromInt(ModId), ContextString, true);
			break;
		}

		return Result;
	}
	catch (std::exception e) 
	{
		FLogger::LogTrace(__FUNCTION__ + FString(" :: CATCH EXCEPTION :: ") + FString(e.what()));
		return nullptr;
	}
}

FWeaponDTStruct* UWeaponsDBComponent::GetWeaponByClassAndModel(EWeaponClass Class, FString WeaponModel) const
{
	FWeaponDTStruct* Result = nullptr;
	TArray<FWeaponDTStruct*> Rows;
	switch (Class) 
	{
		case(EWeaponClass::UNARMED):
			Rows = GetAll_FistsDT();
			break;
		case(EWeaponClass::SWORD):
			break;
		case(EWeaponClass::AXE):
			break;
		case(EWeaponClass::HAMMER):
			break;
		case(EWeaponClass::PIKE):
			break;
		case(EWeaponClass::HANDGUN):
		case(EWeaponClass::SHOTGUN):
		case(EWeaponClass::RIFLE):
			Rows = GetAll_FirearmDT();
			break;
	}

	return FindResultByModel(Rows, WeaponModel);;
}

FWeaponDTStruct* UWeaponsDBComponent::GetWeaponByTypeAndModel(EWeaponType Type, FString WeaponModel) const
{
	FWeaponDTStruct* Result = nullptr;
	TArray<FWeaponDTStruct*> Rows;
	switch (Type) 
	{
		case(EWeaponType::UNARMED):
			Rows = GetAll_FistsDT();
			break;
		case(EWeaponType::FIREARM):
			Rows = GetAll_FirearmDT();
			break;
		case(EWeaponType::BLUNT):
			Rows = GetAll_BluntWeaponsDT();
			break;
		case(EWeaponType::SHARP):
			Rows = GetAll_SharpWeaponsDT();
			break;
		case(EWeaponType::POINTED):
			Rows = GetAll_PointedWeaponsDT();
			break;
		case(EWeaponType::SHIELD):
			Rows = GetAll_ShieldsDT();
		default:
			break;
	}

	return FindResultByModel(Rows, WeaponModel);;
}

FWeaponArtDTStruct* UWeaponsDBComponent::GetWeaponArtDataById(int32 WeaponArtID)
{
	static const FString ContextString = FString(" :: GETTING WEAPON ART DATA WITH ID :: ") + FString::FromInt(WeaponArtID);
	FWeaponArtDTStruct* GunArtData = nullptr;

	try 
	{
		GunArtData = WeaponArtDT->FindRow<FWeaponArtDTStruct>(*FString::FromInt(WeaponArtID), ContextString, true);
	}
	catch (std::exception e) 
	{
		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION GETTING WEAPON ART DATA  WITH ID :: ") + FString::FromInt(WeaponArtID) + FString(" :: EXCEPTION MSG :: ") + FString(e.what()));

		return GunArtData;
	}

	return GunArtData;
}

FWeaponAbilitiesDTStruct* UWeaponsDBComponent::GetWeaponAbilityDataById(int32 AbilityID)
{
	static const FString ContextString = FString(" :: GETTING WEAPON ABILITY DATA WITH ID :: ") + FString::FromInt(AbilityID);
	FWeaponAbilitiesDTStruct* AbilityData = nullptr;
	try 
	{
		AbilityData = WeaponAbilitiesDT->FindRow<FWeaponAbilitiesDTStruct>(*FString::FromInt(AbilityID), ContextString, true);
	}
	catch (std::exception e) 
	{
		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION GETTING WEAPON ABILITY DATA  WITH ID :: ") + FString::FromInt(AbilityID) + FString(" :: EXCEPTION MSG :: ") + FString(e.what()));

		return AbilityData;
	}

	return AbilityData;
}

FWeaponFxDTStruct* UWeaponsDBComponent::GetWeaponFxDataById(int32 FxDataID)
{
	static const FString ContextString = FString(" :: GETTING WEAPON FX DATA WITH ID :: ") + FString::FromInt(FxDataID);
	FWeaponFxDTStruct* FxData = nullptr;
	try 
	{
		FxData = WeaponFxDT->FindRow<FWeaponFxDTStruct>(*FString::FromInt(FxDataID), ContextString, true);
	}
	catch (std::exception e) 
	{
		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION GETTING WEAPON FX DATA  WITH ID :: ") + FString::FromInt(FxDataID) + FString(" :: EXCEPTION MSG :: ") + FString(e.what()));

		return FxData;
	}

	return FxData;
}

void UWeaponsDBComponent::LoadDataTables()
{
	static const ConstructorHelpers::FObjectFinder<UDataTable>FistWeaponsDTFile(TEXT("DataTable'/Game/Core/DataTables/CombatSystem/Weapons/Fists/DT_FistWeapons.DT_FistWeapons'"));
	static const ConstructorHelpers::FObjectFinder<UDataTable>FistConfigsDTFile(TEXT("DataTable'/Game/Core/DataTables/CombatSystem/Weapons/Fists/DT_FistWeaponConfigs.DT_FistWeaponConfigs'"));

	static const ConstructorHelpers::FObjectFinder<UDataTable>PointedWeaponsDTFile(TEXT("DataTable'/Game/Core/DataTables/CombatSystem/Weapons/MeleeWeapons/DT_PointedWeapons.DT_PointedWeapons'"));
	static const ConstructorHelpers::FObjectFinder<UDataTable>BluntWeaponsDTFile(TEXT("DataTable'/Game/Core/DataTables/CombatSystem/Weapons/MeleeWeapons/DT_BluntWeapons.DT_BluntWeapons'"));
	static const ConstructorHelpers::FObjectFinder<UDataTable>SharpWeaponsDTFile(TEXT("DataTable'/Game/Core/DataTables/CombatSystem/Weapons/MeleeWeapons/DT_SharpWeapons.DT_SharpWeapons'"));
	static const ConstructorHelpers::FObjectFinder<UDataTable>ShieldsDTFile(TEXT("DataTable'/Game/Core/DataTables/CombatSystem/Weapons/MeleeWeapons/DT_Shields.DT_Shields'"));

	static const ConstructorHelpers::FObjectFinder<UDataTable>FiregunsDTFile(TEXT("DataTable'/Game/Core/DataTables/CombatSystem/Weapons/Firearms/DT_Fireguns.DT_Fireguns'"));
	static const ConstructorHelpers::FObjectFinder<UDataTable>FiregunConfigsDTFile(TEXT("DataTable'/Game/Core/DataTables/CombatSystem/Weapons/Firearms/DT_FiregunConfigs.DT_FiregunConfigs'"));
	static const ConstructorHelpers::FObjectFinder<UDataTable>FiregunModsDTFile(TEXT("DataTable'/Game/Core/DataTables/CombatSystem/Weapons/Firearms/DT_FiregunMods.DT_FiregunMods'"));

	static const ConstructorHelpers::FObjectFinder<UDataTable>WeaponArtDTFile(TEXT("DataTable'/Game/Core/DataTables/CombatSystem/Weapons/DT_WeaponsArt.DT_WeaponsArt'"));
	static const ConstructorHelpers::FObjectFinder<UDataTable>WeaponAbilitiesDTFile(TEXT("DataTable'/Game/Core/DataTables/CombatSystem/Weapons/DT_WeaponAbilities.DT_WeaponAbilities'"));
	static const ConstructorHelpers::FObjectFinder<UDataTable>WeaponFxDTFile(TEXT("DataTable'/Game/Core/DataTables/CombatSystem/Weapons/DT_WeaponFx.DT_WeaponFX'"));


	if (FistWeaponsDTFile.Succeeded())
		FistWeaponsDT = FistWeaponsDTFile.Object;

	else FLogger::LogTrace(__FUNCTION__ + FString(" :: DATA TABLE NOT FOUND :: FIST_WEAPONS_DT "));

	if (FistConfigsDTFile.Succeeded())
		FistWeaponConfigsDT = FistConfigsDTFile.Object;

	else FLogger::LogTrace(__FUNCTION__ + FString(" :: DATA TABLE NOT FOUND :: FIST_WEAPON_CONFIGS_DT "));

	if (PointedWeaponsDTFile.Succeeded())
		PointedWeaponsDT = PointedWeaponsDTFile.Object;

	else FLogger::LogTrace(__FUNCTION__ + FString(" :: DATA TABLE NOT FOUND :: POINTED_WEAPONS_DT "));

	if (BluntWeaponsDTFile.Succeeded())
		BluntWeaponsDT = BluntWeaponsDTFile.Object;

	else FLogger::LogTrace(__FUNCTION__ + FString(" :: DATA TABLE NOT FOUND :: BLUNT_WEAPONS_DT "));

	if (SharpWeaponsDTFile.Succeeded())
		SharpWeaponsDT = SharpWeaponsDTFile.Object;

	else FLogger::LogTrace(__FUNCTION__ + FString(" :: DATA TABLE NOT FOUND :: SHARP_WEAPONS_DT "));

	if (ShieldsDTFile.Succeeded())
		ShieldsDT = ShieldsDTFile.Object;

	else FLogger::LogTrace(__FUNCTION__ + FString(" :: DATA TABLE NOT FOUND :: SHIELDS_DT "));

	if (FiregunsDTFile.Succeeded())
		FiregunsDT = FiregunsDTFile.Object;

	else FLogger::LogTrace(__FUNCTION__ + FString(" :: DATA TABLE NOT FOUND :: FIREGUNS_DT "));

	if (FiregunModsDTFile.Succeeded())
		FiregunModifiersDT = FiregunModsDTFile.Object;

	else FLogger::LogTrace(__FUNCTION__ + FString(" :: DATA TABLE NOT FOUND :: FIREGUN_MODS_DT "));

	if (FiregunConfigsDTFile.Succeeded())
		FiregunConfigsDT = FiregunConfigsDTFile.Object;

	else FLogger::LogTrace(__FUNCTION__ + FString(" :: DATA TABLE NOT FOUND :: FIREGUN_CONFIGS_DT "));

	if (WeaponArtDTFile.Succeeded())
		WeaponArtDT = WeaponArtDTFile.Object;

	else FLogger::LogTrace(__FUNCTION__ + FString(" :: DATA TABLE NOT FOUND :: WEAPON_ART_DT "));

	if (WeaponAbilitiesDTFile.Succeeded())
		WeaponAbilitiesDT = WeaponAbilitiesDTFile.Object;

	else FLogger::LogTrace(__FUNCTION__ + FString(" :: DATA TABLE NOT FOUND :: WEAPON_ABILITIES_DT "));

	if (WeaponFxDTFile.Succeeded())
		WeaponFxDT = WeaponFxDTFile.Object;

	else FLogger::LogTrace(__FUNCTION__ + FString(" :: DATA TABLE NOT FOUND :: WEAPON_FX_DT "));
}

FWeaponDTStruct* UWeaponsDBComponent::FindResultByModel(TArray<FWeaponDTStruct*> Rows, FString WeaponModel) const
{
	FWeaponDTStruct* Result = nullptr;

	for (FWeaponDTStruct* Row : Rows)
		if (Row->WeaponModel == WeaponModel)
			Result = Row;
	
	return Result;
}

