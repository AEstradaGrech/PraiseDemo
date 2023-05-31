// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BooleanDecision.h"
#include "LocoDecision.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API ULocoDecision : public UBooleanDecision
{
	GENERATED_BODY()
public:
	ULocoDecision();
	FORCEINLINE void SetDecisionType(EBotDecision Decision) { DecisionType = Decision; }
	virtual EBotDecision DecisionID() const override { return DecisionType; }
	virtual bool HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision) override;

private:
	EBotDecision DecisionType;
};
