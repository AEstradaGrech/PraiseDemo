// Fill out your copyright notice in the Description page of Project Settings.


#include "IdleMachineState.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../../AI/BotDecisions/RunawayDecision.h"
#include "../../AI/BotDecisions/PatrolDecision.h"
//#include "../../AI/BotDecisions/"

UIdleMachineState::UIdleMachineState()
{
	bIsTimeLimited = true;

	MaxTimeInCurrentState = BB_GLOBAL_MAX_IDLE_VAL;
}

bool UIdleMachineState::OnEnterState(ABaseBotController* NewBotController)
{
	if (!Super::OnEnterState(NewBotController)) return false; 

	StateMachine->SetupBB();
	bIsTimeLimited = true;
	TimeInCurrentState = 0;
	LastTickSeconds = 0.f;

	BotController->StopMovement();
	
	AddDefaultBotDecisions();

	MaxTimeInCurrentState = bIsTimeLimited ? StateMachine->GetBotBB()->GetValueAsFloat(BB_GLOBAL_MAX_IDLE) : 0.f;

	bDidInit = true;

	return bDidInit;
}

void UIdleMachineState::OnExitState()
{
	Super::OnExitState();
}

void UIdleMachineState::RunState()
{
	if (!StateMachine) {
		OnStopBrain.ExecuteIfBound();
		return;
	}

	Super::RunState();		
}

bool UIdleMachineState::ShouldAutodestroy()
{
	if (Super::ShouldAutodestroy()) return true;

	if (StateDecisions.Num() > 0) {
		if (StateDecisions.Num() == 1 ) {
			return true;
		}
	}
	if (bIsTimeLimited && TimeInCurrentState >= MaxTimeInCurrentState)
	{
		return true;
	}

	return !StateMachine->GetBotBB()->GetValueAsBool(BB_CAN_WANDER) && !StateMachine->GetBotBB()->GetValueAsBool(BB_CAN_PATROL) && TimeInCurrentState >= MaxTimeInCurrentState;
}

void UIdleMachineState::AddStateDecisions()
{
	TryAddStateDecision(EBotDecision::AUTODESTROY);
	TryAddStateDecision(EBotDecision::RUN_AWAY);
	
	UBooleanDecision* WanderDecision = Cast<UBooleanDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::BOOLEAN, this));
	
	if (WanderDecision && WanderDecision->HasValidBrain()) {
		WanderDecision->SetupDecision([this]() { return ShouldWander(); }, EMachineState::WANDER);
		StateDecisions.Add(WanderDecision);
	}
}

bool UIdleMachineState::ShouldWander()
{
	return !StateMachine->GetBotBB()->GetValueAsBool(BB_CAN_PATROL) && StateMachine->GetBotBB()->GetValueAsBool(BB_CAN_WANDER);
}

bool UIdleMachineState::HasBlackboardTargets()
{
	return StateMachine->GetBotBB()->GetValueAsObject(BB_MAX_AGGRO_TARG) &&
		   StateMachine->GetPriorityTarget() &&
		   StateMachine->GetBotBB()->GetValueAsObject(BB_HAS_BEEN_DAMAGED);
}

bool UIdleMachineState::CanRunDefaultBehaviour()
{
	bool bCanPatrol = StateMachine->GetBotBB()->GetValueAsBool(BB_CAN_PATROL);
	bool bCanWander = StateMachine->GetBotBB()->GetValueAsBool(BB_CAN_WANDER);

	return bCanPatrol && bCanWander;
}

