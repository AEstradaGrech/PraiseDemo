// Fill out your copyright notice in the Description page of Project Settings.


#include "BotDecision.h"

UBotDecision::UBotDecision() 
{

}

bool UBotDecision::HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision)
{
	if (PrevDecision)
		PrevDecision = PreviousDecision;
	
	return false;
}

bool UBotDecision::ChainResult(FDecisionOutput& DecisionOutput)
{
	if (!IsChained()) return DecisionOutput.bIsTransitionDecision; 
	
	return DependantDecision->HasDecided(DecisionOutput, this);
}
