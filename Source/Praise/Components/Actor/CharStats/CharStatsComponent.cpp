// Fill out your copyright notice in the Description page of Project Settings.


#include "CharStatsComponent.h"
#include "../../../Praise.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "../../../Characters/BasePraiseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UCharStatsComponent::UCharStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetDefaults();
	// ...
}

void UCharStatsComponent::SetDefaults()
{
	bIsDead = Health > 0;
	Health = DEFAULT_CHAR_HEALTH;
	Stamina = DEFAULT_CHAR_STAMINA;
	CharFOV = 90.f;
	bIsDead = false;
	DefaultWalkSpeed = 450.f;
	SprintSpeedMult = 1.5f;
	ArmedSpeedMult = 0.9f;
	WalkSpeedMult = 1.f;
	MaxWalkSpeedMult = 2.f;
	WalkSpeed = DefaultWalkSpeed;
	MinWalkSpeed = 200.f;
	MaxWalkSpeed = DefaultWalkSpeed * MaxWalkSpeedMult;
	Health = CharacterStats ? CharacterStats->GetMaxHealth() : DEFAULT_CHAR_HEALTH;
}


void UCharStatsComponent::UpdateStamina(float Value)
{
	float NewValue = Stamina + Value;

	Stamina = FMath::Clamp<float>(NewValue, 0, GetMaxHealth());
}

float UCharStatsComponent::GetResistance()
{
	if (!CharacterStats) return 1.f;
	float A = (CharacterStats->Strenght + CharacterStats->Constitution) / 2;
	float B = (HEALTH_RESISTANCE_FACTOR * CharacterStats->Constitution + (FMath::Square(0.25f + GetHealthRatio())));
	float C = (STAM_RESISTANCE_FACTOR * CharacterStats->Strenght + (FMath::Square(0.25f + GetStaminaRatio())));

	return A + B + C;
}

float UCharStatsComponent::GetMaxResistance()
{
	if (!CharacterStats) return 1.f;

	return (CharacterStats->Strenght + CharacterStats->Constitution) / 2 +
		(HEALTH_RESISTANCE_FACTOR * CharacterStats->Constitution + FMath::Sqrt(1.25f)) +
		(STAM_RESISTANCE_FACTOR * CharacterStats->Strenght + FMath::Sqrt(1.25f));
}

float UCharStatsComponent::GetResistanceRatio()
{
	if (!CharacterStats) return 1.f;

	return GetResistance() / GetMaxResistance();
}

float UCharStatsComponent::GetHealthRatio() const
{
	if (!CharacterStats) return 0.f;

	return CharacterStats->GetMaxHealth() > 0 ? Health / CharacterStats->GetMaxHealth() : 0.0f;
}

float UCharStatsComponent::GetStaminaRatio() const
{
	if (!CharacterStats) return 0.f;

	return CharacterStats->GetMaxStamina() > 0 ? Stamina / CharacterStats->GetMaxStamina() : 0.0f;
}

float UCharStatsComponent::GetWalkSpeed() const
{
	ABasePraiseCharacter* Owner = GetStatsOwner<ABasePraiseCharacter>();

	return DefaultWalkSpeed * (Owner->IsWeaponEquiped() ? ArmedSpeedMult : 1) * (Owner->IsRunning() ? MaxWalkSpeedMult : 1) * WalkSpeedMult;
}

void UCharStatsComponent::SetWalkSpeed(float Value)
{
	WalkSpeed = FMath::Clamp(Value, GetMinWalkSpeed(), GetMaxWalkSpeed());
}

void UCharStatsComponent::IncreaseSpeed(float MultRatio)
{
	WalkSpeedMult *= MultRatio;
}

void UCharStatsComponent::RestoreDefaultSpeed()
{
	WalkSpeedMult = 1.f;
	WalkSpeed = DefaultWalkSpeed;
}

void UCharStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);
}

void UCharStatsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCharStatsComponent, Health);
	DOREPLIFETIME(UCharStatsComponent, Stamina);
	DOREPLIFETIME(UCharStatsComponent, bIsDead);
	DOREPLIFETIME(UCharStatsComponent, WalkSpeed);
	DOREPLIFETIME(UCharStatsComponent, CharFOV);
	DOREPLIFETIME(UCharStatsComponent, WalkSpeedMult);
	DOREPLIFETIME(UCharStatsComponent, MaxWalkSpeedMult);
}

// Called every frame
void UCharStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	// ...
}



void UCharStatsComponent::UpdateStatValue(ECharStats Stat, float Value)
{
	switch (Stat)
	{
		case(ECharStats::STRENGHT):
		case(ECharStats::CONSTITUTION):
		case(ECharStats::INTELLIGENCE):
		case(ECharStats::DEXTRITY):
		case(ECharStats::LUCK):
			UpdateMainStatValue(Stat, Value);
			break;
		case(ECharStats::SPEED):
			WalkSpeedMult = FMath::Clamp(Value, 0.f, 5.f); 
			Cast<ABasePraiseCharacter>(GetOwner())->UpdateWalkSpeed(ECurveType::LINEAR);
			break;
		case(ECharStats::MAX_SPEED):
			MaxWalkSpeedMult = FMath::Clamp(Value, 2.f, 5.f);
			break;
		case(ECharStats::FOV):
			CharFOV = FMath::Clamp(Value, 0.f, 90.f);
			break;
	}
}

float UCharStatsComponent::GetMainStatValue(ECharStats Stat)
{
	switch (Stat)
	{
		case(ECharStats::LEVEL):
			return CharacterStats->Level;
		case(ECharStats::STRENGHT):
			return CharacterStats->Strenght;
		case(ECharStats::CONSTITUTION):
			return CharacterStats->Constitution;
		case(ECharStats::INTELLIGENCE):
			return CharacterStats->Intelligence;
		case(ECharStats::DEXTRITY):
			return CharacterStats->Dextrity;
		case(ECharStats::LUCK):
			return CharacterStats->Luck;
		default:
			return 0.f;
	}
}

float UCharStatsComponent::GetStatValue(ECharStats Stat)
{
	switch (Stat)
	{
	case(ECharStats::STRENGHT):
		return CharacterStats->Strenght;
	case(ECharStats::CONSTITUTION):
		return CharacterStats->Constitution;
	case(ECharStats::INTELLIGENCE):
		return CharacterStats->Intelligence;
	case(ECharStats::DEXTRITY):
		return CharacterStats->Dextrity;
	case(ECharStats::LUCK):
		return CharacterStats->Luck;
	case(ECharStats::WISDOM):
		return GetMainStatValue(Stat);
	case(ECharStats::SPEED):
		return GetSpeedMult();
	case(ECharStats::MAX_SPEED):
		return GetMaxWalkSpeed();
	case(ECharStats::FOV):
		return GetFieldOfView();
	default:
		return 0.f;
	}
}

void UCharStatsComponent::ManageHealth(float Value, AActor* Damager)
{
	Health += Value;

	if (Health > CharacterStats->GetMaxHealth())
		Health = CharacterStats->GetMaxHealth();

	if (Health <= 0) 
	{
		bIsDead = true;
		OnSetDead.ExecuteIfBound(Damager);
	}
}

void UCharStatsComponent::UpdateHealth(float Value)
{
	float health = FMath::Clamp<float>(Health + Value, 0.f, CharacterStats->GetMaxHealth());

	Health = health;
	
	if (Health <= 0)
		bIsDead = true;
}

float UCharStatsComponent::GetPhysicalDamageReduction(float DamagePoints)
{
	return  DamagePoints * ((float)CharacterStats->Constitution / (float)MAX_ATTRIBUTE_POINTS);
}

void UCharStatsComponent::UpdateMainStatValue(ECharStats Stat, float Value)
{
	if (!CharacterStats) return;

	float ClampedValue = FMath::Clamp<int>(Value, 0, MAX_STATS_VALUE);

	switch (Stat)
	{
	case(ECharStats::STRENGHT):
		CharacterStats->Strenght = ClampedValue;
		break;
	case(ECharStats::CONSTITUTION):
		CharacterStats->Constitution = ClampedValue;
		break;
	case(ECharStats::INTELLIGENCE):
		CharacterStats->Intelligence = ClampedValue;
		break;
	case(ECharStats::DEXTRITY):
		CharacterStats->Dextrity = ClampedValue;
		break;
	case(ECharStats::LUCK):
		CharacterStats->Luck = ClampedValue;
		break;
	default:
		break;
	}
}


void UCharStatsComponent::InitializeStatsObject(FCharStats* StatsStruct)
{
	if (!CharacterStats)
		CharacterStats = NewObject<UCharStatsHandler>(this);

	if (CharacterStats) 
	{
		CharacterStats->Init(StatsStruct);
		Health = CharacterStats->GetMaxHealth();
		Stamina = CharacterStats->GetMaxStamina();
	}

	else FLogger::LogTrace(__FUNCTION__ + FString(" :: FAILED TO LOAD STATS STRUCT..."));
}

