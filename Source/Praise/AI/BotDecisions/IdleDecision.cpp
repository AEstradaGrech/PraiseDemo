// Fill out your copyright notice in the Description page of Project Settings.


#include "IdleDecision.h"

UIdleDecision::UIdleDecision() : Super() 
{

}

bool UIdleDecision::HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision)
{
	Super::HasDecided(DecisionOutput, PreviousDecision);

	if (!BotBrain) {
		DecisionOutput.OutputState = EMachineState::IDLE;
		DecisionOutput.bIsTransitionDecision = true;
		return true;
	}

	if ((!BotBrain->GetBotBB()->GetValueAsBool(BB_CAN_PATROL) && !HasBlackboardTargets()) || BotBrain->ShouldStayInIdle()) {
		DecisionOutput.OutputState = EMachineState::IDLE;
		DecisionOutput.bIsTransitionDecision = true;
		return true;
	}


	return false;
}

bool UIdleDecision::HasBlackboardTargets()
{
	return BotBrain->GetBotBB()->GetValueAsBool(BB_HAS_BEEN_DAMAGED) ||
		BotBrain->HasValidBBTarget(BB_CURRENT_WP) ||
		BotBrain->HasValidBBTarget(BB_LAST_DAMAGER) ||
		BotBrain->HasValidBBTarget(BB_PRIORITY_TARGET) ||
		BotBrain->HasValidBBTarget(BB_MAX_AGGRO_TARG);
}
