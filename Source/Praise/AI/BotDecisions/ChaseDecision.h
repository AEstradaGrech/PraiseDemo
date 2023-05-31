// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BotDecision.h"
#include "../../Enums/AI/EChaseMode.h"
#include "ChaseDecision.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UChaseDecision : public UBotDecision
{
	GENERATED_BODY()
public:
	UChaseDecision();

	virtual EBotDecision DecisionID() const override { return EBotDecision::CHASE_TARGET; }
	virtual bool HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision) override;

private:
	bool HandleDefaultCase(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision);
	bool HandleCombatCase(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision);
	bool HandleChaseCase(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision);

	bool CanChase(bool bCheckTarget);
};
