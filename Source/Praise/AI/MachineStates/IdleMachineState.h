// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMachineState.h"
#include "IdleMachineState.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UIdleMachineState : public UBaseMachineState
{
	GENERATED_BODY()
	
public:
	UIdleMachineState();
	virtual bool OnEnterState(ABaseBotController* NewBotController) override;
	virtual void OnExitState() override;
	virtual void RunState() override;

private:
	virtual bool ShouldAutodestroy() override;
	virtual void AddStateDecisions() override;

	bool ShouldWander();
	bool HasBlackboardTargets();
	bool CanRunDefaultBehaviour();

};
