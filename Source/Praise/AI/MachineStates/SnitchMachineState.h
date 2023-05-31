// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMachineState.h"
#include "SnitchMachineState.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API USnitchMachineState : public UBaseMachineState
{
	GENERATED_BODY()
	
public:
	USnitchMachineState();

	virtual bool OnEnterState(ABaseBotController* BotController) override;
	virtual void OnExitState() override;
	virtual void RunState() override;

	virtual bool ShouldRun() override;
	virtual bool ShouldBackToIdle() override;

private:
	virtual void AddStateDecisions() override;
	bool ShouldUpdateTarget();
	bool ShouldWander();
	bool ShouldRequestBackup();
	bool ShouldSnitch();
	bool ShouldUpdatePath();

	void UpdateTarget();
	void GetRandomLocation();
	void SendBackupRequest();
	void SendTargetSeenMsg();
	void UpdatePath();

	ITargeteable* GetPriorityTarget();
	bool IsValidSnitchingTarget(ITargeteable* Target);
	ITargeteable* TryGetSnitchingTarget();

	int MaxGetTargetTries;
	int CurrentGetTargetTries;

	bool bHasReachedLocation;
	bool bHasValidLocation;
	bool bHasSnitched;
	float TargetCheckSeconds;
	float LastTargetCheckTimestamp;
	float MinSnitchTime;
	float BackupReqRange = 15000.f;
	float BackupReqSeconds;
	float LastBackupReqTimestamp;
	float SnitchingDistance;

	FVector LastEnemyKnownLoc;
};
