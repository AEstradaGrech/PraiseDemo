// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../Enums/AI/EMachineState.h"
#include "../../Enums/AI/EBotDecision.h"
#include "../../Enums/AI/EWanderMode.h"
#include "../../Enums/AI/EAI_Msg.h"
#include "../../Enums/AI/EAI_MsgChannel.h"
#include "../../Structs/CommonUtility/FLogger.h"
#include "../../Characters/AI/BaseBotController.h"
#include "../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../AI/CommonUtility/Navigation/AIWaypoint.h"
#include "../BotDecisions/BotDecision.h"
#include "../BotDecisions/BooleanDecision.h"
#include "../../Structs/AI/FDecisionOutput.h"
#include "../../Interfaces/AI/Evaluable.h"
#include "../../Praise.h"
#include "../../Structs/CommonUtility/FUtilities.h"
#include "../../Structs/AI/FEnvQueryConfig.h"
#include "../CommonUtility/BotEnvQuery/EnvQueryDistanceCheck.h"
#include "../CommonUtility/BotEnvQuery/EnvQueryVisibilityCheck.h"
#include "../CommonUtility/BotEnvQuery/EnvQueryTargetInRadiusCheck.h"
#include "../../Enums/AI/EEnvQueryMode.h"
#include "NavigationSystem.h"
#include "../../Enums/Characters/ECharAction.h"
#include "BaseMachineState.generated.h"

/**
 * 
 */
DECLARE_DELEGATE(FOnStopBrainSignature)
UCLASS()
class PRAISE_API UBaseMachineState : public UObject, public IEvaluable
{
	GENERATED_BODY()
	
public:
	UBaseMachineState();


	FORCEINLINE class UStateMachineBrainComponent* GetStateMachine() const { return StateMachine; }
	FORCEINLINE ABaseBotCharacter* GetBot() const { return Bot; }
	FORCEINLINE TArray<UBotDecision*> GetStateDecisions() const { return StateDecisions; }
	FORCEINLINE bool DidInit() const { return bDidInit; }
	FORCEINLINE bool IsTimeLimited() const { return bIsTimeLimited; }
	virtual bool OnEnterState(ABaseBotController* BotController);
	virtual void OnExitState();
	virtual void RunState();
	void AddDefaultBotDecisions();
	FOnStopBrainSignature OnStopBrain;
	virtual bool ShouldBackToIdle() override { return bShouldResetLoop; }  
	// ---------- IEvaluable -----------
	virtual bool ShouldRun() override { return false; };
	virtual bool ShouldEquip() override { return false; };
	virtual bool ShouldAttack() override { return false; };
	virtual bool ShouldBlock() override { return false; };
	virtual bool ShouldTarget() override { return false; }
	virtual bool ShouldEvade() override { return false; }
	virtual bool ShouldUnblockPath() override;

protected:

	virtual bool HasValidReferences(ABaseBotController* BotController);
	virtual void AddStateDecisions();
	virtual bool ShouldAutodestroy();	  

	UBotDecision* TryAddStateDecision(EBotDecision Decision);
	UBotDecision* TryAddStateDecision(EBotDecision ChainTo, EBotDecision Chained, bool bReturnChained = true);
	UBotDecision* TryGetStateDecision(EBotDecision Decision);
	void TryChainDecision(EBotDecision ChainTo, UBotDecision* Chained);
	
	void UpdateState(EMachineState NewState);
	bool bDidInit;
	bool bShouldResetLoop;
	bool bCheckBlockedPath;
	
	void BackToIdle();
	class UStateMachineBrainComponent* StateMachine;
	ABaseBotController* BotController;
	ABaseBotCharacter* Bot;
	
	UPROPERTY()
	FDecisionOutput DecisionOutput;
	UPROPERTY()
	TArray<UBotDecision*> StateDecisions;
	UNavigationSystemV1* NavMesh;
	float TimeInCurrentState;
	float TotalTimeInCurrentState;
	float MaxTimeInCurrentState;
	float OnEnterStateTimeStamp;
	float PathFindingDistance;
	bool bUsePathFinding;
	bool bIsTimeLimited;
	float LastTickSeconds;
	float DeltaTime();

};
