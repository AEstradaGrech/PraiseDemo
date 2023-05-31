// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BotBrainComponent.h"
#include "../../../Enums/AI/EMachineState.h"
#include "../../../Components/GameMode/MachineStatesFactoryComponent.h"
#include "../../../Interfaces/AI/Evaluable.h"
#include "StateMachineBrainComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnMachineStateUpdateSignature);
UCLASS()
class PRAISE_API UStateMachineBrainComponent : public UBotBrainComponent
{
	GENERATED_BODY()

public:
	UStateMachineBrainComponent();

	virtual void ResetLoop() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SwitchAIState(EMachineState NewState);
	virtual void SetBrainEnabled(bool bEnable) override;
	void BeginAutodestroy();
	FORCEINLINE class UBaseMachineState* GetCurrentMachineState() const { return CurrentMachineState; }
	FORCEINLINE EMachineState GetCurrentState() const { return CurrentState; }
	FORCEINLINE UMachineStatesFactoryComponent* GetStatesFactory() const { return StatesFactory; }
	TArray<UBotDecision*> GetDefaultBotDecisions() const;

	FORCEINLINE void SetStatesFactory(UMachineStatesFactoryComponent* Factory) { StatesFactory = Factory; }

	virtual bool InitBrain(ABaseBotController* OwnerController, ABaseBotCharacter* BrainOwner) override;
	virtual void SetStateMachineEnabled(bool bEnabled);
	virtual void SetStayInIdle(bool Value) override;
	
	virtual void ClearMachine();

	FOnMachineStateUpdateSignature OnStateUpdate;
	
protected:
	// Called when the game starts
	virtual void SetDefaults() override;
	virtual void TickRunState(float DeltaTime);
	TArray<UBotDecision*> GetCharDefaultDecisions();
	class UBaseMachineState* GetFactoryState(EMachineState NewState);
	UFUNCTION()
		void OnPostWorldCreatedCallback(UWorld* World);
	UFUNCTION()
		virtual void ChangeState();

	UPROPERTY(EditAnywhere, BlueprintReadonly)
		TArray<EBotDecision> DefaultBotDecisions;

	void ForceChangeState(EMachineState NewState);
	void UpdateState(class UBaseMachineState* NewState);
	void CheckForUpdates();

	UPROPERTY(VisibleAnywhere, Category = AI)
		UMachineStatesFactoryComponent* StatesFactory;
	UPROPERTY()
	TArray<UBotDecision*> CachedDefaultDecisions;
	UPROPERTY()
	TArray<UBooleanDecision*> LocomotionDecisions;
	void HandleLocoDecisions(IEvaluable* EvaluableState);
	void HandleStateDecisions(UBaseMachineState* State);
	bool EvaluateCurrentState(IEvaluable* State, EBotDecision Decision);
	virtual void SetupDefaultDecisions();

	UPROPERTY()
	class UBaseMachineState* CurrentMachineState;

	UPROPERTY(VisibleAnywhere, Category = AI)
		FName CurrentStateName;
	UPROPERTY(VisibleAnywhere, Category = AI)
		EMachineState CurrentState;
	UPROPERTY(EditAnywhere, Category = AI)
		float StateUpdateInterval;
	
	float StateUpdateCounter;
	FTimerHandle StateUpdateTimerHandle;

private:
	void AddLocoDecison(EBotDecision DecisionType);

};
