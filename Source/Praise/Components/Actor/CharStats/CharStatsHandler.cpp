// Cuidado!!

#include "CharStatsHandler.h"
#include "../../../Structs/CommonUtility/FLogger.h"
#include "../../../Structs/CommonUtility/FUtilities.h"
#include "../../../Praise.h"

UCharStatsHandler::UCharStatsHandler()
{
	Level = 1;
}

void UCharStatsHandler::Init(FCharStats* Stats)
{
	Level = FMath::Clamp<int>(Stats->Level, 1, MAX_CHAR_LVL_VALUE);
	Class = Stats->Class;
	Strenght = FMath::Clamp<int>(Stats->Strenght, MIN_STATS_VALUE, MAX_STATS_VALUE);
	Constitution = FMath::Clamp<int>(Stats->Constitution, MIN_STATS_VALUE, MAX_STATS_VALUE);
	Dextrity = FMath::Clamp<int>(Stats->Dextrity, MIN_STATS_VALUE, MAX_STATS_VALUE);
	Intelligence = FMath::Clamp<int>(Stats->Intelligence, MIN_STATS_VALUE, MAX_STATS_VALUE);
	Luck = FMath::Clamp<int>(Stats->Luck, MIN_STATS_VALUE, MAX_STATS_VALUE);

	SetMaxHealth();
	SetMaxStamina();

	bDidInit = true;
}

void UCharStatsHandler::DebugStuff()
{
	FLogger::LogTrace(__FUNCTION__);

	FLogger::LogTrace(FString(" --> STRENGHT		: ") + FString::FromInt(Strenght));
	FLogger::LogTrace(FString(" --> CONSTITUTION	: ") + FString::FromInt(Constitution));
	FLogger::LogTrace(FString(" --> DEXTRITY		: ") + FString::FromInt(Dextrity));
	FLogger::LogTrace(FString(" --> INTELLIGENCE	: ") + FString::FromInt(Intelligence));
	FLogger::LogTrace(FString(" --> LUCK			:") + FString::FromInt(Luck));

	
	FLogger::LogTrace(FString(" --> CLASS ENUM ID	:") + FString::FromInt((int)Class) + FString(" :: CLASS --> ") + FUtilities::EnumToString(TEXT("ECharClass"), (int)Class));
	FLogger::LogTrace(FString(" --> FACTION ENUM ID	:") + FString::FromInt((int)Faction) + FString(" :: FACTION --> ") + FUtilities::EnumToString(TEXT("ECharFaction"), (int)Faction));
	FLogger::LogTrace(FString(" --> MAX HEALTH		:") + FString::FromInt(MaxHealth));
	FLogger::LogTrace(FString(" --> MAX STAMINA		:") + FString::FromInt(MaxStamina));
	FLogger::LogTrace(FString(" --> MAX MANA		:") + FString::FromInt(MaxMana));
	FLogger::LogTrace(FString(" --> MAX ADRENALINE	:") + FString::FromInt(MaxAdrenaline));
}

void UCharStatsHandler::SetMaxHealth()
{
	MaxHealth = DEFAULT_CHAR_HEALTH;

	float LvlBonus = MaxHealth * 10 * Level / 100;
	float ConstBonus = MaxHealth * Constitution * 15 / 100;
	float StrengthBonus = MaxHealth * Strenght * 10 / 100;

	MaxHealth += LvlBonus + (ConstBonus + Strenght / 2);
}

void UCharStatsHandler::SetMaxStamina()
{
	MaxStamina = DEFAULT_CHAR_STAMINA;

	float LvlBonus = MaxStamina * 10 * Level / 100;
	float ConstBonus = MaxStamina * Constitution * 10 / 100;
	float StrengthBonus = MaxStamina * Strenght * 20 / 100;

	MaxStamina += LvlBonus + (ConstBonus + Strenght / 2);
}

void UCharStatsHandler::SetMaxMana()
{
	MaxMana = DEFAULT_CHAR_MANA;

	float LvlBonus = MaxMana * 15 * Level / 100;
	float IntBonus = MaxMana * Intelligence * 20 / 100;

	MaxMana += LvlBonus + IntBonus;
}

void UCharStatsHandler::SetMaxAdrenaline()
{
	MaxAdrenaline = DEFAULT_CHAR_ADRENALINE;

	float LvlBonus = MaxAdrenaline * 10 * Level / 100;

	float StrengthBonus = MaxStamina * Strenght * 15 / 100;
	float DextrityBonus = MaxAdrenaline * 10 * Dextrity / 100;
	MaxAdrenaline += LvlBonus + (DextrityBonus + StrengthBonus / 2);
}
