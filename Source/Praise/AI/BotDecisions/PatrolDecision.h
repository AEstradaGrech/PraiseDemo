// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BotDecision.h"
#include "PatrolDecision.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UPatrolDecision : public UBotDecision
{
	GENERATED_BODY()
	
public:
	UPatrolDecision();

	virtual EBotDecision DecisionID() const override { return EBotDecision::START_PATROL; }
	virtual bool HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision) override;

private:
	bool CanPatrol();
	bool HasBlackboardTargets();
};
