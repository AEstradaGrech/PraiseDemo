// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMachineState.h"
#include "NavigationSystem.h"
#include "RunawayMachineState.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API URunawayMachineState : public UBaseMachineState
{
	GENERATED_BODY()
	
public:
	URunawayMachineState();

	virtual bool OnEnterState(ABaseBotController* BotController) override;
	virtual void OnExitState() override;
	virtual void RunState() override;

	virtual bool ShouldRun() override;
	virtual bool ShouldBackToIdle() override; 
private:
	virtual void AddStateDecisions() override;

	FORCEINLINE float GetSafeDistance() { return SearchDistance - (SearchDistance * 30 / 100); }

	float SafeDistance;
	float SearchDistance = 3000.f;
	float StoppingDistance = 150.f;
	float MinRunawayTime = 30.f;
	float MaxRunawayRange = 7500;
	float UpdatePathSeconds = 3.f;
	float LastPathUpdateTimestamp = 0;
	float FindCoverSpotSeconds = 5.f;
	float LastFindCoverSpotTimestamp = 0.f;
	bool bCanHide;
	bool bIsMovingToCoverSpot;
	bool bIsHiding;

	bool TryFindCoverSpot();

	AActor* GetAvoidedActor();
	bool HasActorToAvoid();
	FVector GetAvoidedLocation();
	bool TrySetTargetLocation(bool bMoveToSpawnLoc = false);
	bool TrySetPriorityLocation();
	bool TargetCanSeePoint(AActor* Target, const FVector& PointLocation);
	bool ShouldMoveToSpawnLoc();
	bool ShouldUpdatePath();
	bool ShouldFindCoverSpot();
	bool IsSafeCoverSpot();
	
	void UpdatePath(bool bMoveToSpawnLoc = false);
	bool HasReachedTarget();
	bool HasBlackboardValues();
	FVector GetRandomOpposedLocation(FVector LocationToAvoid);
	bool bHasReachedTarget = false;
	bool bIsMovingToTarget = false;
	bool bIsMovingToSpawnLoc = false;
	FVector TargetLocation = FVector::ZeroVector;

	float BackupReqRange = 15000.f;
	float BackupReqSeconds;
	float LastBackupReqTimestamp;

	bool ShouldRequestBackup();
	void SendBackupRequest();
};
