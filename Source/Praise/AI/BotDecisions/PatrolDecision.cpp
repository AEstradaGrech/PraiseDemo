// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolDecision.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"

UPatrolDecision::UPatrolDecision() : Super()
{

}

bool UPatrolDecision::HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision)
{
	Super::HasDecided(DecisionOutput, PreviousDecision);

	if (CanPatrol()) {
		DecisionOutput.OutputState = EMachineState::PATROL;
		DecisionOutput.bIsTransitionDecision = true;
		return IsChained() ? ChainResult(DecisionOutput) : true;
	}

	return false;
}

bool UPatrolDecision::CanPatrol()
{
	return BotBrain->GetBotBB()->GetValueAsBool(BB_CAN_PATROL) && BotBrain->GetKnownLocations().Num() > 0 && !BotBrain->HasKnownEnemies() && !HasBlackboardTargets(); 
}

bool UPatrolDecision::HasBlackboardTargets()
{
	return BotBrain->GetBotBB()->GetValueAsBool(BB_HAS_BEEN_DAMAGED) ||
			BotBrain->HasValidBBTarget(BB_LAST_DAMAGER) || 
			BotBrain->HasValidBBTarget(BB_LAST_SEEN_TARGET) ||
		   BotBrain->HasValidBBTarget(BB_PRIORITY_TARGET);
}
