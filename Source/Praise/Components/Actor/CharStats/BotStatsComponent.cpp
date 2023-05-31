// Fill out your copyright notice in the Description page of Project Settings.


#include "BotStatsComponent.h"
#include "../../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../TargetingComponent.h"
#include "../../../Structs/CommonUtility/FLogger.h"


UBotStatsComponent::UBotStatsComponent() : Super() 
{
	SetDefaults();
}


void UBotStatsComponent::InitializeStatsObject(FCharStats* StatsStruct)
{
	FBotStats* Stats = (FBotStats*)StatsStruct;

	if (!Stats) return;

	UPlayerStatsHandler* StatsObject = NewObject<UPlayerStatsHandler>(this);

	StatsObject->Init(Stats);

	ArmedSpeedMult = Stats->ArmedSpeedMult;
	CharFOV = Stats->DefaultFOV;
	DefaultWalkSpeed = Stats->DefaultMovementSpeed;
	WalkSpeedMult = Stats->WalkSpeedMult;
	MaxWalkSpeedMult = Stats->MaxWalkSpeedMult;
	SightRange = Stats->DefaultSightRange;
	ArmedSpeedMult = Stats->ArmedSpeedMult;
	MaxPerceivedTargets = Stats->MaxPerceivedTargets;
	TargetRecallSeconds = Stats->TargetRecallSeconds;
	MaxPerceivedAllies = Stats->MaxPerceivedAllies;
	AllyRecallSeconds = Stats->AllyRecallSeconds;
	MaxPerceivedEnemies = Stats->MaxPerceivedEnemies;
	EnemyRecallSeconds = Stats->EnemyRecallSeconds;
	LastKnownEnemyRecallSeconds = Stats->LastKnownEnemyRecallSeconds;
	AttackAwaitSecs = Stats->AttackAwaitSecs;
	EvadeAwaitSecs = Stats->EvadeAwaitSecs;
	AlertedStateSecs = Stats->AlertedStateSecs;
	bSprintStamEnabled = Stats->bStamLimitedSprint;
	WalkSpeed = DefaultWalkSpeed;
	MaxWalkSpeed = WalkSpeed * MaxWalkSpeedMult;
	MinWalkSpeed = 100.f;
	AccelerationLerpSpeed = Stats->AccelerationLerpSpeed;

	CharacterStats = StatsObject;

	if (GetStatsOwner<ABaseBotCharacter>() && GetStatsOwner<ABaseBotCharacter>()->GetTargetingComponent())
		GetStatsOwner<ABaseBotCharacter>()->GetTargetingComponent()->SetTraceRange(SightRange);

}

void UBotStatsComponent::SetDefaults()
{
	Super::SetDefaults();

	DefaultWalkSpeed = 200.f;
	MaxWalkSpeedMult = 3.f;
	MinWalkSpeed = DefaultWalkSpeed;
	WalkSpeed = DefaultWalkSpeed;
	SightRange = 3000.f;

	// --- Target Mgmt
	MaxPerceivedTargets = 5;
	TargetRecallSeconds = 3.f;
	// --- Ally Mgmt
	MaxPerceivedAllies = 5;
	AllyRecallSeconds = 5.f;
	// --- Enemy Mgmt
	MaxPerceivedEnemies = 50;
	EnemyRecallSeconds = 5.f;
	LastKnownEnemyRecallSeconds = 30.f;
	AttackAwaitSecs = 2.f;
	EvadeAwaitSecs = 3.f;
}

void UBotStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = GetMaxHealth();
	Stamina = GetMaxStamina();
}