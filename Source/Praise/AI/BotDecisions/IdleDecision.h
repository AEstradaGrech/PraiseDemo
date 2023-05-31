// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BotDecision.h"
#include "IdleDecision.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UIdleDecision : public UBotDecision
{
	GENERATED_BODY()
	
public:
	UIdleDecision();

	virtual EBotDecision DecisionID() const override { return EBotDecision::STAY_IN_IDLE; }
	virtual bool HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision) override;
	
private:
	bool HasBlackboardTargets();
};
