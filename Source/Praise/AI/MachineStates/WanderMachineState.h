// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMachineState.h"
#include "NavigationSystem.h"
#include "WanderMachineState.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UWanderMachineState : public UBaseMachineState
{
	GENERATED_BODY()
	
public:
	UWanderMachineState();

	virtual bool OnEnterState(ABaseBotController* NewBotController) override;
	virtual void OnExitState() override;
	virtual void RunState() override;
private:
	void SetNewRandomLocation();
	void OnTargetReached();
	FVector GetTargetLocation(bool bCurrentTarget = true);
	bool HasReachedTarget();
	float MaxRadius;
	UNavigationSystemV1* NavMesh;
	float StoppingDistance = 150.f;
	float LocationWaitingTime = 0.f;
	bool bIsFacingTarget;


	int VisitedLocationsIdx;
	int LocationsToEndPatrol;
	
	float CurrentPathSize;
	EWanderMode WanderMode;
	
	virtual void AddStateDecisions() override;
	virtual bool ShouldBackToIdle() override;

	FTimerHandle OnTargetReachedWaitingTimer;
	UFUNCTION()
		void MoveToNextLocation();
	float GetWaitingTime(bool bExactTime = false);

};
