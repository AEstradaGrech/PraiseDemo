// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatsHandler.h"
#include "../../../Structs/CommonUtility/FLogger.h"
#include "../../../Praise.h"
#include "LevelUp.h"
#include "Engine/DataTable.h"

UPlayerStatsHandler::UPlayerStatsHandler() : Super()
{
}


void UPlayerStatsHandler::Init(FPlayerStats Stats)
{
	
	Level = FMath::Clamp<int>(Stats.Level, 1, MAX_CHAR_LVL_VALUE);
	Class =Stats.Class;
	Strenght = FMath::Clamp<int>(Stats.Strenght, MIN_STATS_VALUE, MAX_STATS_VALUE);
	Constitution = FMath::Clamp<int>(Stats.Constitution, MIN_STATS_VALUE, MAX_STATS_VALUE);
	Dextrity = FMath::Clamp<int>(Stats.Dextrity, MIN_STATS_VALUE, MAX_STATS_VALUE);
	Intelligence = FMath::Clamp<int>(Stats.Intelligence, MIN_STATS_VALUE, MAX_STATS_VALUE);
	Luck = FMath::Clamp<int>(Stats.Luck, MIN_STATS_VALUE, MAX_STATS_VALUE);
	Charisma = Stats.Charisma;
	Wisdom = Stats.Wisdom;

	XP = Stats.CurrentXP;

	SetMaxHealth();
	SetMaxStamina();

	bDidInit = true;
}

void UPlayerStatsHandler::Init(FCharStats* Stats)
{
	Super::Init(Stats);

	FPlayerStats* PlayerStats = (FPlayerStats*)Stats;
	
	if (!PlayerStats) return;

	Charisma = PlayerStats->Charisma;
	Wisdom = PlayerStats->Wisdom;
	XP = PlayerStats->CurrentXP;
}

ULevelUp* UPlayerStatsHandler::SetLevelUp(ULevelUp* LevelUp)
{
	return LevelUp;
}

void UPlayerStatsHandler::ApplyLvlUp(ULevelUp* LevelUp)
{
	FLogger::LogTrace(__FUNCTION__ + FString(" :: APPLYING LEVEL UP :: TO LEVEL --> ") + FString::FromInt(LevelUp->ToLevel()));

	Level = LevelUp->ToLevel();
	XP = LevelUp->RemainingXP;
	XPToLvlUp = GetXPToNextLvl(Level);
	TotalSkillPoints += LevelUp->SkillPoints;
	TotalAttributePoints += LevelUp->AttributePoints;

	MaxHealth = LevelUp->GetMaxHealth();
	MaxStamina = LevelUp->GetMaxStamina();
	MaxMana = LevelUp->GetMaxMana();
	MaxAdrenaline = LevelUp->GetMaxAdrenaline();
	Restness = LevelUp->GetMaxRestness();
}

float UPlayerStatsHandler::GetXPToNextLvl(int lvl)
{
	return FIRST_LVL_UP_XP * FMath::Pow(lvl, 2) - FIRST_LVL_UP_XP * lvl;
}

void UPlayerStatsHandler::BuildProgressionChart()
{
	FLogger::LogTrace(__FUNCTION__);

}


