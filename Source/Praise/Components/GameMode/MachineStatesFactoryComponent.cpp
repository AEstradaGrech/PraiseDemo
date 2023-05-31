// Fill out your copyright notice in the Description page of Project Settings.


#include "MachineStatesFactoryComponent.h"
#include "../../AI/MachineStates/IdleMachineState.h"
#include "../../AI/MachineStates/PatrolMachineState.h"
#include "../../AI/MachineStates/TravelMachineState.h"
#include "../../AI/MachineStates/WanderMachineState.h"
#include "../../AI/MachineStates/RunawayMachineState.h"
#include "../../AI/MachineStates/ChaseMachineState.h"
#include "../../AI/MachineStates/CombatMachineState.h"
#include "../../AI/MachineStates/SnitchMachineState.h"

// -- Bot Decisions
#include "../../AI/BotDecisions/IdleDecision.h"
#include "../../AI/BotDecisions/BooleanDecision.h"
#include "../../AI/BotDecisions/PatrolDecision.h"
#include "../../AI/BotDecisions/RunawayDecision.h"
#include "../../AI/BotDecisions/ChaseDecision.h"
#include "../../AI/BotDecisions/LocoDecision.h"
#include "../../AI/BotDecisions/SeekDecision.h"
#include "../../AI/BotDecisions/SelectTargetDecision.h"
#include "../../AI/BotDecisions/EngageCombatDecision.h"
#include "../../AI/BotDecisions/SnitchDecision.h"

//-----
#include "../../Structs/CommonUtility/FLogger.h"
// Sets default values for this component's properties
UMachineStatesFactoryComponent::UMachineStatesFactoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	FWorldDelegates::OnPostWorldCreation.AddUObject(this, &UMachineStatesFactoryComponent::OnPostWorldCreationCallback);

	
	// ...
}


// Called when the game starts
void UMachineStatesFactoryComponent::BeginPlay()
{
	Super::BeginPlay();

	EnsureStatesRegistration();
	// ...
}


UBaseMachineState* UMachineStatesFactoryComponent::GetMachineState(EMachineState State)
{
	return CreateStateFunctionsMap.Contains(State)? (this->*CreateStateFunctionsMap[State])() : nullptr;
}

UBotDecision* UMachineStatesFactoryComponent::GetStateDecision(EBotDecision Decision)
{
	return CreateDecisionFunctionsMap.Contains(Decision) ? (this->*CreateDecisionFunctionsMap[Decision])(nullptr) : nullptr;
}

UBotDecision* UMachineStatesFactoryComponent::GetStateDecision(EBotDecision Decision, UBaseMachineState* OwnerState)
{
	if (!OwnerState->GetBot() || !OwnerState->GetBot()->GetBrain()) return nullptr;

	UBotDecision* NewDecision = CreateDecisionFunctionsMap.Contains(Decision) ? (this->*CreateDecisionFunctionsMap[Decision])(OwnerState) : nullptr;

	if (!NewDecision) return nullptr;

	NewDecision->SetBotBrain(OwnerState->GetBot()->GetBrain());

	return NewDecision;
}

void UMachineStatesFactoryComponent::EnsureStatesRegistration()
{
	if (CreateStateFunctionsMap.Num() == 0)
		RegisterStates();

	if (CreateDecisionFunctionsMap.Num() == 0)
		RegisterDecisions();
}

void UMachineStatesFactoryComponent::OnPostWorldCreationCallback(UWorld* World)
{
	
}

void UMachineStatesFactoryComponent::RegisterStates()
{
	Register<UIdleMachineState>(EMachineState::IDLE);
	Register<UPatrolMachineState>(EMachineState::PATROL);
	Register<UTravelMachineState>(EMachineState::TRAVEL);
	Register<UWanderMachineState>(EMachineState::WANDER);
	Register<URunawayMachineState>(EMachineState::RUNAWAY);
	Register<UChaseMachineState>(EMachineState::CHASE);
	Register<UCombatMachineState>(EMachineState::COMBAT);
	Register<USnitchMachineState>(EMachineState::SNITCH);
}

void UMachineStatesFactoryComponent::RegisterDecisions()
{
	RegisterDecision<UIdleDecision>(EBotDecision::STAY_IN_IDLE);
	RegisterDecision<UBooleanDecision>(EBotDecision::BOOLEAN);
	RegisterDecision<UPatrolDecision>(EBotDecision::START_PATROL);
	RegisterDecision<URunawayDecision>(EBotDecision::RUN_AWAY);
	RegisterDecision<UChaseDecision>(EBotDecision::CHASE_TARGET);
	RegisterDecision<ULocoDecision>(EBotDecision::LOCOMOTION);
	RegisterDecision<USeekDecision>(EBotDecision::SEEK);
	RegisterDecision<USelectTargetDecision>(EBotDecision::SELECT_TARGET);
	RegisterDecision<UEngageCombatDecision>(EBotDecision::ENGAGE_COMBAT);
	RegisterDecision<USnitchDecision>(EBotDecision::SNITCH);
}

