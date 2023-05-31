// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BotDecision.h"
#include "SnitchDecision.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API USnitchDecision : public UBotDecision
{
	GENERATED_BODY()
public:
	USnitchDecision();
	virtual EBotDecision DecisionID() const override { return EBotDecision::SNITCH; }
	virtual bool HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision) override;

	void SetSnitchLocation(const bool bValue);

private:
	bool CanSnitch();
};
