// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMachineState.h"
#include "CombatMachineState.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UCombatMachineState : public UBaseMachineState
{
	GENERATED_BODY()
	
public:
	UCombatMachineState();
	virtual bool OnEnterState(ABaseBotController* BotController) override;
	virtual void OnExitState() override;
	virtual void RunState() override;

	virtual bool ShouldRun() override;
	virtual bool ShouldEquip() override;
	virtual bool ShouldAttack() override;
	virtual bool ShouldBlock() override;
	virtual bool ShouldEvade() override;
	virtual bool ShouldTarget() override;
	bool ShouldBackToIdle() override; 
	bool CanAttackTarget();
	ITargeteable* GetTargetAsTargeteable() const;
	ICombatible* GetTargetAsCombatible() const;
private:
	virtual void AddStateDecisions();
	AActor* CurrentTarget;

	float UpdateEnemyLocationSeconds;
	float SecondsToUpdateLocations;
	TMap<ECharVector, TArray<AActor*>> NearbyEnemies;
	TArray<AActor*> NearbyAllies;
	void LocateFightingAllies();
	void LocateEnemies();
	void UpdateEnemyStatus();
	ECharVector GetMostDangerousVector();
	TArray<ECharVector> GetSafeDirections();
	ECharVector SafestEvadeDirection;

	bool ShouldRequestBackup();
	void SendBackupRequest();
	float BackupReqRange = 2500.f;
	float BackupReqSeconds;
	float LastBackupReqTimestamp;
};
