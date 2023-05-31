// Fill out your copyright notice in the Description page of Project Settings.


#include "BooleanDecision.h"

UBooleanDecision::UBooleanDecision() : Super()
{

}

bool UBooleanDecision::HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision)
{
	Super::HasDecided(DecisionOutput, PreviousDecision);

	if (CheckFunction()) {
		DecisionOutput.OutputState = OutputState;
		DecisionOutput.bIsTransitionDecision = bIsTransitionDecision;
		DecisionOutput.bCallbackResult = bDecisionCallbackResult;
		DecisionOutput.DecisionCallback = DecisionCallback;
		return IsChained() ? ChainResult(DecisionOutput) : true;
	}

	return false;
}

void UBooleanDecision::SetupDecision(TFunction<bool()> Condition, TFunction<void(bool)> Callback, bool bCallbackResult, bool bIsTransition, EMachineState Output)
{
	CheckFunction = Condition;
	DecisionCallback = Callback;
	bDecisionCallbackResult = bCallbackResult;
	bIsTransitionDecision = bIsTransition;
	OutputState = Output;
}

void UBooleanDecision::SetupDecision(TFunction<bool()> Condition, EMachineState Output)
{
	CheckFunction = Condition;
	OutputState = Output;
	bIsTransitionDecision = true;
	
}
