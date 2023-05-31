// Fill out your copyright notice in the Description page of Project Settings.


#include "LocoDecision.h"

ULocoDecision::ULocoDecision() : Super() 
{
	DecisionType = EBotDecision::LOCOMOTION;
}

bool ULocoDecision::HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision)
{
	Super::HasDecided(DecisionOutput, PreviousDecision);

	if (CheckFunction()) {
		DecisionOutput.OutputState = OutputState;
		DecisionOutput.bIsTransitionDecision = bIsTransitionDecision;
		DecisionOutput.bCallbackResult = true;
		DecisionOutput.DecisionCallback = DecisionCallback;
		return true;
	}
	else {
		DecisionOutput.OutputState = OutputState;
		DecisionOutput.bIsTransitionDecision = bIsTransitionDecision;
		DecisionOutput.bCallbackResult = false;
		DecisionOutput.DecisionCallback = DecisionCallback;
		return true;
	}

	return false;
}
