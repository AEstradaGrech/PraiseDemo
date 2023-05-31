// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BotDecision.h"
#include "SelectTargetDecision.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API USelectTargetDecision : public UBotDecision
{
	GENERATED_BODY()
public:
	USelectTargetDecision();
	virtual EBotDecision DecisionID() const override { return EBotDecision::SELECT_TARGET; }
	virtual bool HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision) override;

private:
	AActor* GetBlackboardTarget(FName Key);
	bool HasBlackboardValues() const;

	bool SetPriorityTargetAndReturn(FDecisionOutput& DecisionOutput, AActor* Target);
	bool SetPriorityLocationAndReturn(FDecisionOutput& DecisionOutput, FVector Location);
};
