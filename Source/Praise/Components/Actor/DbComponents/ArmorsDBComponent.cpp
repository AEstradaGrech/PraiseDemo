// Fill out your copyright notice in the Description page of Project Settings.


#include "ArmorsDBComponent.h"
#include "ArmorsDBComponent.h"
#include "../../../Structs/CommonUtility/FUtilities.h"
#include "../../../Structs/CommonUtility/FLogger.h"
#include <exception>

// Sets default values for this component's properties
UArmorsDBComponent::UArmorsDBComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	static const ConstructorHelpers::FObjectFinder<UDataTable> ArmorsDataTable(TEXT("DataTable'/Game/Core/DataTables/CombatSystem/Armors/DT_Armors.DT_Armors'"));

	if (ArmorsDataTable.Succeeded())
		ArmorsDT = ArmorsDataTable.Object;

	ConstructorHelpers::FObjectFinder<UDataTable> ConfigsDataTable(TEXT("DataTable'/Game/Core/DataTables/CombatSystem/Armors/DT_ArmorConfigs.DT_ArmorConfigs'"));

	if (ConfigsDataTable.Succeeded())
		ArmorConfigsDT = ConfigsDataTable.Object;

	ConstructorHelpers::FObjectFinder<UDataTable> ModsDataTable(TEXT("DataTable'/Game/Core/DataTables/CombatSystem/Armors/DT_ArmorMods.DT_ArmorMods'"));

	if (ModsDataTable.Succeeded())
		ArmorModsDT = ModsDataTable.Object;

	static const ConstructorHelpers::FObjectFinder<UDataTable> ArmorArtDataTable(TEXT("DataTable'/Game/Core/DataTables/CombatSystem/Armors/DT_ArmorArt.DT_ArmorArt'"));

	if (ArmorArtDataTable.Succeeded())
		ArmorArtDT = ArmorArtDataTable.Object;
}

// Called when the game starts
void UArmorsDBComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


FArmorsDTStruct* UArmorsDBComponent::GetArmorByTypeAndModel(EArmorType Type, FString Model)
{
	if (!ArmorsDT) return nullptr;

	static const FString ContextString = FString(" :: Getting Armor Data :: TYPE --> ") + FUtilities::EnumToString(*FString("EArmorType"), (int)Type) + FString(" :: MODEL --> ") + Model;

	try
	{
		TArray<FArmorsDTStruct*> Rows;
		ArmorsDT->GetAllRows(ContextString, Rows);

		for (FArmorsDTStruct* Row : Rows)
			if ((EArmorType)Row->ArmorType == Type && Row->ArmorModel == Model)
				return Row;
	}
	catch (std::exception e)
	{
		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION --> ") + e.what());
	}

	return nullptr;
}

TArray<FArmorsDTStruct*> UArmorsDBComponent::GetArmorConfigsByTypeAndModel(EArmorType Type, FString Model)
{
	if (!ArmorConfigsDT) return TArray<FArmorsDTStruct*>();

	static const FString ContextString = FString(" :: Getting Configs for Armor Type --> ") + FUtilities::EnumToString(*FString("EArmorType"), (int)Type) + FString(" :: MODEL --> ") + Model;

	TArray<FArmorsDTStruct*> Results;
	try
	{
		TArray<FArmorsDTStruct*> Rows;
		ArmorConfigsDT->GetAllRows(ContextString, Rows);

		for (FArmorsDTStruct* Row : Rows)
			if ((EArmorType)Row->ArmorType == Type && Row->ArmorModel == Model)
				Results.Add(Row);
	}
	catch (std::exception e)
	{
		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION --> ") + e.what());
	}

	return Results;
}

TArray<FArmorsDTStruct*> UArmorsDBComponent::GetArmorModsByTypeAndModel(EArmorType Type, FString Model)
{
	if (!ArmorModsDT) return TArray<FArmorsDTStruct*>();

	static const FString ContextString = FString(" :: Getting Mods for Armor Type --> ") + FUtilities::EnumToString(*FString("EArmorType"), (int)Type) + FString(" :: MODEL --> ") + Model;

	TArray<FArmorsDTStruct*> Results;
	try
	{
		TArray<FArmorsDTStruct*> Rows;
		ArmorModsDT->GetAllRows(ContextString, Rows);

		for (FArmorsDTStruct* Row : Rows)
			if ((EArmorType)Row->ArmorType == Type && Row->ArmorModel == Model)
				Results.Add(Row);
	}
	catch (std::exception e)
	{
		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION --> ") + e.what());
	}

	return Results;
}

FArmorsDTStruct* UArmorsDBComponent::GetArmorConfigByID(int ConfigID)
{
	if (!ArmorConfigsDT) return nullptr;

	static const FString ContextString = FString(" :: Getting Config with ID --> ") + FString::FromInt(ConfigID);
	try
	{
		return ArmorConfigsDT->FindRow<FArmorsDTStruct>(*FString::FromInt(ConfigID), ContextString, true);
	}
	catch (std::exception e)
	{
		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION --> ") + e.what());
	}

	return nullptr;
}

FArmorsDTStruct* UArmorsDBComponent::GetArmorModByID(int ModID)
{
	if (!ArmorModsDT) return nullptr;

	static const FString ContextString = FString(" :: Getting Mod with ID --> ") + FString::FromInt(ModID);
	try
	{
		return ArmorModsDT->FindRow<FArmorsDTStruct>(*FString::FromInt(ModID), ContextString, true);
	}
	catch (std::exception e)
	{
		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION --> ") + e.what());
	}

	return nullptr;
}

FArmorArtDTStruct* UArmorsDBComponent::GetArmorArtByID(int ArtDataID)
{
	static const FString ContextString = FString(" :: Getting Art with ID --> ") + FString::FromInt(ArtDataID);

	try
	{
		return ArmorArtDT->FindRow<FArmorArtDTStruct>(*FString::FromInt(ArtDataID), ContextString, true);
	}
	catch (std::exception e)
	{
		FLogger::LogTrace(__FUNCTION__ + FString(" :: EXCEPTION --> ") + e.what());
	}

	return nullptr;
}
