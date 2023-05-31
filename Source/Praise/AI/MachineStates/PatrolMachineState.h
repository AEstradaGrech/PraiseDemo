// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMachineState.h"
#include "../CommonUtility/Navigation/WaypointHandler.h"
#include "../CommonUtility/Navigation/AIWaypoint.h"
#include "PatrolMachineState.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UPatrolMachineState : public UBaseMachineState
{
	GENERATED_BODY()
	
public:
	UPatrolMachineState();
	virtual bool OnEnterState(ABaseBotController* NewBotController) override;
	virtual void OnExitState() override;
	virtual void RunState() override;

	virtual bool ShouldUnblockPath() override;
	virtual bool ShouldRun() override;
private:

	virtual void AddStateDecisions() override;
	virtual bool ShouldBackToIdle() override;
	void SetNewRoute(class ANodeWaypoint* OriginNode, TArray<ANodeWaypoint*> Nodes);
	void SetNewAvailableRoute();
	bool TrySetNewRoute(class ANodeWaypoint* OriginNode, TArray<ANodeWaypoint*> KnownLocations, class UAIRoute*& OutRoute);
	void OnTargetReached();
	bool HasCurrentWaypoint();
	float GetWaitingTime(bool bExactTime = false);
	AAIWaypoint* GetCurrentWP();
	void SetCurrentWP(AAIWaypoint* PathPoint);
	void SetSubZonePatrol(class ANodeWaypoint* SubZoneEntry);
	UFUNCTION()
		void SetNextPathPoint();
	UFUNCTION()
		void WandeArroundWP();
	UFUNCTION()
		void ResumeSubZonePatrol();

	float StoppingDistance = 150.0f;
	float WaypointWaitingTime = 0.f;
	FTimerHandle WaypointWaitingTimer;
	FTimerHandle WanderArroundTimer;
	FTimerHandle ResumeSubZonePatrolTimer;
	int MaxUnblockPathTriesPerWP;
	int UnblockPathTries;
	UNavigationSystemV1* NavMesh;
	UWaypointHandler* WaypointHandler;
	TArray<class ANodeWaypoint*> AvailableNodes;
	UPROPERTY()
	class UAIRoute* PatrolRoute;
	class ANodeWaypoint* GetAvailableNode(bool bRandomNode = true);
	bool TrySetAvailableNodes();

	bool bIsSubZonePatrol = false;
	float SubZonePatrolSecs;
	float SubZoneEnterTimestamp;
	float MinSubZonePatrolSecs;
	float MaxSubZonePatrolSecs;
	bool bIsWanderingArroundWP = false;
	bool ShouldEnterSubZone();

};
