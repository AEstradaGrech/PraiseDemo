// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BotDecision.h"
#include "../../Interfaces/CombatSystem/Targeteable.h"
#include "RunawayDecision.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API URunawayDecision : public UBotDecision
{
	GENERATED_BODY()
	
public:
	URunawayDecision();
	virtual EBotDecision DecisionID() const override { return EBotDecision::RUN_AWAY; }
	virtual bool HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision) override;

private:
	bool HasBlackboardTargets();
protected:

	ITargeteable* GetCurrentThreat();
	ITargeteable* GetCurrentBBThreat();
};
