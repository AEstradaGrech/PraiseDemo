// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Enums/AI/EMachineState.h"
#include "../../Enums/AI/EBotDecision.h"
#include "../../AI/MachineStates/BaseMachineState.h"
#include "../../AI/BotDecisions/BotDecision.h"
#include "MachineStatesFactoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRAISE_API UMachineStatesFactoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMachineStatesFactoryComponent();

	FORCEINLINE bool HasRegisteredStates() const { return CreateStateFunctionsMap.Num() > 0; }
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	typedef UBaseMachineState* (UMachineStatesFactoryComponent::* pCreateStateFn)();
	typedef UBotDecision* (UMachineStatesFactoryComponent::* pCreateBotDecisionFn)(UBaseMachineState* OwnerState);

	template<typename T>
	FORCEINLINE void Register(EMachineState State) { CreateStateFunctionsMap.Add(State, &UMachineStatesFactoryComponent::CreateState<T>); }
	template<typename T>
	FORCEINLINE void RegisterDecision(EBotDecision Decision) { CreateDecisionFunctionsMap.Add(Decision, &UMachineStatesFactoryComponent::CreateBotDecision<T>);}
	
	UBaseMachineState* GetMachineState(EMachineState State);
	UBotDecision* GetStateDecision(EBotDecision Decision);
	UBotDecision* GetStateDecision(EBotDecision Decision, UBaseMachineState* OwnerState);

	void EnsureStatesRegistration();
	

private:
	void RegisterStates();
	void RegisterDecisions();

	template<typename T>
	FORCEINLINE UBaseMachineState* CreateState() { return NewObject<T>(); }
	template<typename T>
	FORCEINLINE UBotDecision* CreateBotDecision(UBaseMachineState* OwnerState) { return OwnerState ? NewObject<T>(OwnerState) : NewObject<T>(); }
	template<typename T>
	FORCEINLINE UBotDecision* CreateBotDecision() { return NewObject<T>(); }

	TMap<EMachineState,pCreateStateFn> CreateStateFunctionsMap;

	TMap<EBotDecision, pCreateBotDecisionFn> CreateDecisionFunctionsMap;

	UFUNCTION()
		void OnPostWorldCreationCallback(UWorld* World);

};
