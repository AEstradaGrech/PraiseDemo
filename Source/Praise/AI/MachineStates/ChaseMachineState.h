// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMachineState.h"
#include "../../Enums/AI/EChaseMode.h"
#include "ChaseMachineState.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UChaseMachineState : public UBaseMachineState
{
	GENERATED_BODY()

	UChaseMachineState();
public:

	virtual bool OnEnterState(ABaseBotController* BotController) override;
	virtual void OnExitState() override;
	virtual void RunState() override;

	virtual bool ShouldRun() override;
	virtual bool ShouldEquip() override;
	virtual bool ShouldTarget() override;
	bool ShouldBackToIdle() override;

private:
	virtual void AddStateDecisions() override;

	float StoppingDistance;
	float TriggerDistance;
	float MaxChasingTime;
	bool bCanMoveToLocation;

	FVector CurrentTargetLocation;
	FVector ToTarget;
	AActor* GetChasedTarget();
	FVector GetTargetLocation();
	float GetRandomizedStoppingDistance();

	float DistanceToTarget;
	float DistanceToLocation;
	float TimeBlocked;
	float MaxTimeBlocked;
	bool bCanReachTarget;
	FVector CurrentNavLoc;
	EChaseMode ChaseMode;
	bool ShouldUpdateLocation();
	void UpdateTargetLocation(bool bAbortIfPartial = true);
	void HandleChaseModeMovement();
	bool CanReachTarget(int Attempts = 1);
	int CanReachAttempts;
	float UpdatePathTimestamp;
	float UpdatePathSeconds;

	UFUNCTION()
		bool CheckQueryPointMaxDistance(FVector Point, float MaxDistance);
};
