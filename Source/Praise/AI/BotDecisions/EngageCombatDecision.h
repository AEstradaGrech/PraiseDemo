// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BotDecision.h"
#include "../../Interfaces/CombatSystem/Combatible.h"
#include "EngageCombatDecision.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UEngageCombatDecision : public UBotDecision
{
	GENERATED_BODY()

public:
	UEngageCombatDecision();
	virtual EBotDecision DecisionID() const override { return EBotDecision::ENGAGE_COMBAT; }
	virtual bool HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision) override;
	void SetOnlyInCombatRange(bool bValue) { bOnlyInCombatRange = bValue; }
private:
	AActor* GetPriorityTarget();
	ICombatible* GetTargetAsCombatible();
	bool IsValidTarget();
	bool bOnlyInCombatRange;

};
