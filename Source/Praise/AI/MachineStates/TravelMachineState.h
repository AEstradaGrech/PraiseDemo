// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMachineState.h"
#include "../CommonUtility/Navigation/WaypointHandler.h"
#include "../CommonUtility/Navigation/AIWaypoint.h"
#include "TravelMachineState.generated.h"

/**
 *
 */
UCLASS()
class PRAISE_API UTravelMachineState : public UBaseMachineState
{
	GENERATED_BODY()

public:
	UTravelMachineState();
	virtual bool OnEnterState(ABaseBotController* NewBotController) override;
	virtual void OnExitState() override;
	virtual void RunState() override;
	virtual bool ShouldUnblockPath() override;
	virtual bool ShouldRun() override;

private:
	virtual void AddStateDecisions() override;
	virtual bool ShouldBackToIdle() override;
	void SetNewRoute(class ANodeWaypoint* OriginNode, class ANodeWaypoint* DestinationNode, TArray<ANodeWaypoint*> Nodes);
	bool TrySetNewRoute(class ANodeWaypoint* OriginNode, class ANodeWaypoint* DestinationNode, TArray<ANodeWaypoint*> KnownLocations, class UAIRoute*& OutRoute);
	void OnTargetReached();
	bool HasCurrentWaypoint();
	AAIWaypoint* GetCurrentWP();
	void SetCurrentWP(AAIWaypoint* PathPoint);

	bool TryGetClosestNodeToTarget(AActor* Target, class ANodeWaypoint*& OutNode, bool bOnlyCommonNodes = true);
	bool TryGetClosestAvailableNodeToTarget(AActor* Target, class ANodeWaypoint*& OutNode);
	UFUNCTION()
		void SetNextPathPoint();

	UPROPERTY()
		class UAIRoute* TravelRoute;

	float StoppingDistance = 150.0f;
	UNavigationSystemV1* NavMesh;
	UWaypointHandler* WaypointHandler;
	TArray<class ANodeWaypoint*> AvailableNodes;
	
	bool TrySetAvailableNodes();

	UPROPERTY()
		class ANodeWaypoint* RouteOrigin;
	UPROPERTY()
		class ANodeWaypoint* RouteDestination;

	int UnblockPathTries;
};
