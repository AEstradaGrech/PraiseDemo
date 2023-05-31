#include "PlayerStatsComponent.h"
#include "../../../Characters/Player/PraisePlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "../../../Praise.h"

UPlayerStatsComponent::UPlayerStatsComponent() : Super()
{
	SetIsReplicatedByDefault(true);
	
	SightRange = 3000.f;
}

float UPlayerStatsComponent::GetWalkSpeed() const
{
	return Super::GetWalkSpeed();
}

float UPlayerStatsComponent::GetArmedSpeedMult() const
{
	return FMath::Clamp(Super::GetArmedSpeedMult() /* + WeaponConstraints*/, 0.f, 1.f);
}


void UPlayerStatsComponent::ManageXP(float Value)
{
	UPlayerStatsHandler* Stats = GetStatsObject<UPlayerStatsHandler>();

	if (!Stats) return;

	float XP = Stats->XP;

	XP += Value;

	if (XP >= Stats->XPToLvlUp) {
		LevelUp(XP, Stats->XPToLvlUp);
	}
}

void UPlayerStatsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerStatsComponent, Adrenaline);
	DOREPLIFETIME(UPlayerStatsComponent, Mana);
	DOREPLIFETIME(UPlayerStatsComponent, Stamina);

}

void UPlayerStatsComponent::DebugStuff()
{
	FLogger::LogTrace(__FUNCTION__);

	if (CharacterStats)
		CharacterStats->DebugStuff();

	else FLogger::LogTrace(__FUNCTION__ + FString(" :: NO CHAR STATS OBJECT..."));
}

void UPlayerStatsComponent::UpdateMainStatValue(ECharStats Stat, float Value)
{
	if (!CharacterStats) return;

	UPlayerStatsHandler* Handler = GetStatsObject<UPlayerStatsHandler>();

	float ClampedValue = FMath::Clamp<int>(Value, 0, MAX_STATS_VALUE);

	switch (Stat)
	{
	case(ECharStats::STRENGHT):
		Handler->Strenght = ClampedValue;
		break;
	case(ECharStats::CONSTITUTION):
		Handler->Constitution = ClampedValue;
		break;
	case(ECharStats::INTELLIGENCE):
		Handler->Intelligence = ClampedValue;
		break;
	case(ECharStats::DEXTRITY):
		Handler->Dextrity = ClampedValue;
		break;
	case(ECharStats::LUCK):
		Handler->Luck = ClampedValue;
		break;
	case(ECharStats::WISDOM):
		Handler->Wisdom = ClampedValue;
		break;
	case(ECharStats::CHARISMA):
		Handler->Charisma = ClampedValue;
		break;
	default:
		break;
	}
}

void UPlayerStatsComponent::SetDefaults()
{
	Super::SetDefaults();

	SightRange = 3000.f;
}

void UPlayerStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = GetMaxHealth();
	Stamina = GetMaxStamina();
}

float UPlayerStatsComponent::GetMainStatValue(ECharStats Stat)
{
	if (!CharacterStats) return 0.f;

	UPlayerStatsHandler* Struct = GetStatsObject<UPlayerStatsHandler>();

	switch (Stat)
	{
		case(ECharStats::STRENGHT):
		case(ECharStats::CONSTITUTION):
		case(ECharStats::INTELLIGENCE):
		case(ECharStats::DEXTRITY):
		case(ECharStats::LUCK):
			return Super::GetMainStatValue(Stat);
		case(ECharStats::WISDOM):
			return Struct ? Struct->Wisdom : 0.f;
		case(ECharStats::CHARISMA):
			return Struct ? Struct->Charisma : 0.f;
		default:
			return 0.f;
	}

	return 0.f;
}

void UPlayerStatsComponent::InitializePlayerStatsObject(FPlayerStats StatsStruct)
{
	UPlayerStatsHandler* Handler = NewObject<UPlayerStatsHandler>(this);

	Handler->Init(StatsStruct);

	CharacterStats = Handler;
}

void UPlayerStatsComponent::LevelUp(float TotalXP, float CurrentLvlMaxXP)
{
	ULevelUp* NewLvl = NewObject<ULevelUp>(this);

	float RemainingXP = -CurrentLvlMaxXP + TotalXP;

	NewLvl->SetCurrentLvl(GetStatsObject<UPlayerStatsHandler>()->Level);
	NewLvl->RemainingXP = RemainingXP;
	NewLvl = GetStatsObject<UPlayerStatsHandler>()->SetLevelUp(NewLvl);

	LevelUps.Add(NewLvl);
}

