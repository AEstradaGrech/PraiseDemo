// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMachineState.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../../AI/BotDecisions/RunawayDecision.h"
#include "../../AI/BotDecisions/LocoDecision.h"
#include "../../AI/BotDecisions/PatrolDecision.h"
#include "../../AI/BotDecisions/IdleDecision.h"
#include "../../AI/BotDecisions/ChaseDecision.h"
#include "../../AI/BotDecisions/SelectTargetDecision.h"
#include "../../AI/BotDecisions/EngageCombatDecision.h"
#include "../../AI/BotDecisions/SnitchDecision.h"
//#include ""

UBaseMachineState::UBaseMachineState() 
{
	bDidInit = false;
	LastTickSeconds = 0.f;
	bUsePathFinding = false;
	bShouldResetLoop = false;
	bCheckBlockedPath = false;
	
}

bool UBaseMachineState::OnEnterState(ABaseBotController* NewBotController)
{
	
	if (HasValidReferences(NewBotController) && !Cast<ABaseBotCharacter>(NewBotController->GetPawn())->IsDead()) {
		BotController = NewBotController;
		StateMachine = BotController->GetBrainAs<UStateMachineBrainComponent>();
		Bot = Cast<ABaseBotCharacter>(BotController->GetPawn());
		AddStateDecisions();
		PathFindingDistance = Bot->GetPathFindingDistance();
		TimeInCurrentState = 0;
		OnEnterStateTimeStamp = StateMachine->GetTimeSinceGameStart();
		LastTickSeconds = OnEnterStateTimeStamp;
		StateMachine->SetTimeBlocked(0.f);
		bDidInit = true;
		return true;
	}

	else return false;
	
	return true;
}

void UBaseMachineState::OnExitState()
{
	TotalTimeInCurrentState += TimeInCurrentState;

	StateMachine->GetBotBB()->SetValueAsBool(BB_CURRENT_PATH_IS_PARTIAL, false);
}

void UBaseMachineState::RunState()
{	
	if (bIsTimeLimited) {
		if (LastTickSeconds != 0.f) {
			TimeInCurrentState += StateMachine->GetTimeSinceGameStart() - LastTickSeconds;		
		}
	}

	LastTickSeconds = StateMachine->GetTimeSinceGameStart();
}



void UBaseMachineState::AddDefaultBotDecisions()
{
	if (!StateMachine) return;

	TArray<UBotDecision*> Final;
	TArray<UBotDecision*> Remaining;
	for (int i = 0; i < StateDecisions.Num(); i++)
	{
		if (StateDecisions[i]->DecisionID() == EBotDecision::AUTODESTROY)
			Final.Add(StateDecisions[i]);

		else Remaining.Add(StateDecisions[i]);
	}


	for (int i = 0; i < StateMachine->GetDefaultBotDecisions().Num(); i++) {
		if (!StateMachine->GetDefaultBotDecisions()[i]->HasValidBrain()) {
			StateMachine->GetDefaultBotDecisions()[i]->SetBotBrain(StateMachine);
		}
		Final.Add(StateMachine->GetDefaultBotDecisions()[i]);
	}

	Final.Append(Remaining);
	StateDecisions = Final;
}

bool UBaseMachineState::ShouldUnblockPath()
{
	if (!bCheckBlockedPath) return false;

	if(StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_VALID_LOCATION) && !StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_REACHED_TARGET) && !StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_BEEN_DAMAGED))
		if ((Bot->GetVelocity().Size() <= 0.0f) && LastTickSeconds > 0)
			StateMachine->IncreaseTimeBlocked(StateMachine->GetTimeSinceGameStart() - LastTickSeconds);

	if (StateMachine->GetTimeBlocked() > StateMachine->GetMaxTimeBlocked())
	{
		BotController->StopMovement();
		StateMachine->SetTimeBlocked(0.f);
		return true;
	}

	return false;
}


bool UBaseMachineState::HasValidReferences(ABaseBotController* Controller)
{
	return Controller->GetBotBrain() && 
		   Controller->HasBrainOfType<UStateMachineBrainComponent>() && 
		   Controller->GetPawn() && 
		   Controller->GetPawn()->IsA<ABaseBotCharacter>() &&
		   Cast<ABaseBotCharacter>(Controller->GetPawn())->GetMsgsFactory() &&
		   Controller->GetBotBrain()->GetBotBB() && 
		   Controller->GetBrainAs<UStateMachineBrainComponent>()->GetStatesFactory();
}

void UBaseMachineState::AddStateDecisions()
{

}

bool UBaseMachineState::ShouldAutodestroy()
{
	
	if (bIsTimeLimited && TimeInCurrentState >= MaxTimeInCurrentState)
		return true;

	return !(StateMachine && Bot && BotController && StateMachine->GetBotBB());
}

UBotDecision* UBaseMachineState::TryAddStateDecision(EBotDecision Decision)
{
	UBotDecision* NewDecision = nullptr;
	UBooleanDecision* BooleanDecision = nullptr;
	switch (Decision) {
		case(EBotDecision::AUTODESTROY):
			BooleanDecision = Cast<UBooleanDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::BOOLEAN, this));

			if (!BooleanDecision) return nullptr;

			if (!BooleanDecision->HasValidBrain()) return nullptr;

			BooleanDecision->SetupDecision([this]() { return ShouldAutodestroy(); }, [this](const bool bValue) { StateMachine->BeginAutodestroy(); }, true);
			
			NewDecision = BooleanDecision;
			break;
		case(EBotDecision::STAY_IN_IDLE):
			BooleanDecision = Cast<UBooleanDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::BOOLEAN, this));

			if (BooleanDecision && BooleanDecision->HasValidBrain()) {
				BooleanDecision->SetupDecision([this]() { return ShouldBackToIdle(); }, [this](const bool bValue) { BackToIdle(); }, true, true, EMachineState::IDLE);
				StateDecisions.Add(BooleanDecision);
			}
			break;
		case(EBotDecision::START_PATROL):
			NewDecision = Cast<UPatrolDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::START_PATROL, this));
			break;
		case(EBotDecision::RUN_AWAY):
			NewDecision = Cast<URunawayDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::RUN_AWAY, this));
			break;
		case(EBotDecision::CHASE_TARGET):
			NewDecision = Cast<UChaseDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::CHASE_TARGET, this));
			break;
		case(EBotDecision::SELECT_TARGET):
			NewDecision = Cast<USelectTargetDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::SELECT_TARGET, this));
			break;
		/*case(EBotDecision::ATTACK):
			NewDecision = Cast<UAttackDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::ATTACK, this));
			break;*/
		case(EBotDecision::ENGAGE_COMBAT):
			NewDecision = Cast<UEngageCombatDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::ENGAGE_COMBAT, this));
			break;
		case(EBotDecision::SNITCH):
			NewDecision = Cast<USnitchDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::SNITCH, this));
			break;
		default:
			return NewDecision;
	}

	if (NewDecision && NewDecision->HasValidBrain())
		StateDecisions.Add(NewDecision);

	return NewDecision;
}

UBotDecision* UBaseMachineState::TryGetStateDecision(EBotDecision Decision)
{
	for (int i = 0; i < StateDecisions.Num(); i++)
		if (StateDecisions[i]->DecisionID() == Decision)
			return StateDecisions[i];
		
	return nullptr;
}

void UBaseMachineState::TryChainDecision(EBotDecision ChainTo, UBotDecision* Chained)
{
	UBotDecision* Previous = TryGetStateDecision(ChainTo);

	if (Previous && Chained->HasValidBrain())
		Previous->ChainDecision(Chained);
}

UBotDecision* UBaseMachineState::TryAddStateDecision(EBotDecision ChainTo, EBotDecision Chained, bool bReturnChained)
{
	UBotDecision* Previous = StateMachine->GetStatesFactory()->GetStateDecision(ChainTo, this);

	if (Previous && Previous->HasValidBrain())
	{
		UBotDecision* ChainedDecision = StateMachine->GetStatesFactory()->GetStateDecision(Chained, this);

		if (ChainedDecision && ChainedDecision->HasValidBrain())
		{
			Previous->ChainDecision(ChainedDecision);
			StateDecisions.Add(Previous);
			return bReturnChained ? ChainedDecision : Previous;
		}
	}

	return nullptr;
}


void UBaseMachineState::UpdateState(EMachineState NewState)
{
	if (!StateMachine) {
		OnStopBrain.ExecuteIfBound();
		return;
	}

	StateMachine->SwitchAIState(NewState);
}


void UBaseMachineState::BackToIdle()
{
	if (!StateMachine) {
		OnStopBrain.ExecuteIfBound();
		return;
	}

	StateMachine->SwitchAIState(EMachineState::IDLE);
}

float UBaseMachineState::DeltaTime()
{
	return StateMachine->GetTimeSinceGameStart() - LastTickSeconds;
}


