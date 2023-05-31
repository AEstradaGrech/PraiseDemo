// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BotDecision.h"
#include "../../Enums/AI/EMachineState.h"
#include "BooleanDecision.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UBooleanDecision : public UBotDecision
{
	GENERATED_BODY()
public:
	UBooleanDecision();
	virtual EBotDecision DecisionID() const override { return EBotDecision::BOOLEAN; }
	FORCEINLINE EMachineState GetOutputState() const { return OutputState; }
	FORCEINLINE bool IsTransitionDecision() const { return bIsTransitionDecision; }
	FORCEINLINE bool GetCallbackResult() const { return bDecisionCallbackResult; }
	FORCEINLINE TFunction<bool()> CheckResult() const { return CheckFunction; }
	FORCEINLINE TFunction<void(bool)> TriggerCallback() const { return DecisionCallback; }

	virtual bool HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision  = nullptr) override;
	void SetupDecision(TFunction<bool()> Condition, TFunction<void(bool)> Callback, bool bCallbackResult, bool bIsTransitionDecision = false, EMachineState OutputState = EMachineState::IDLE);
	void SetupDecision(TFunction<bool()> Condition, EMachineState OutputState);

	
protected:
	EMachineState OutputState;
	bool bIsTransitionDecision = false;
	bool bDecisionCallbackResult;

	TFunction<bool()> CheckFunction;
	TFunction<void(bool)> DecisionCallback;
	

	/*
		USO:
		- En un MachineState se añade una BooleanDecision
		- BooleanDecision lleva resultados que se quieren pasar como salida en caso 
		  de que se cumpla CheckFunction (OutputState, bIsTransition, valor que se quiere pasar al Callback Result (true / false. CallbRes != HasDecide.Res)
		- El callback de la decision se configura asi:
		
			---- State comprueba CheckFunction, configura OutputDecision y ejecuta el Callback + Result de Output
			BooleanDecision.bResult = true / false;
			BooleanDecision->DecisionCallback = [MachineState](const bool bValue) {  MachineState::Method(bValue) };
		 
		 y se ejecuta asi:

			 if(HasDecided(OutputRef)){									----- Si se cumple CheckFunction o cualquier otra funcion (si no es BooleanDecision)
				if(OutputRef.IsTransition){								----- Si la decision es de transicion / cambio de estado
					SwitchAIState(Decision.OutputState);				----- Se cambia de estado y StateMachine ejecuta OnExit() / OnEnter()
				}
				else {													----- Si es una decision que no implica necesariamente un cambio de State (ShouldRun() o ShouldAutodestroy()
					if(Decision->IsA<UBooleanDecision>())				----- solo se pueden configurar con BooleanDecisions + Lambdas (si no, es una Decision 'con nombre propio' / especifica (Idle, Patrol, Attack...)
						OutputRef.DecisionCallback(OutputRef.Result);	----- Se ejecuta el Callback del OutputRef, que se ha configurado en HasDecided() con Callback+Result preconfigurado en Decision
				}
			 }


				bool UBooleanDecision::HasDecided(FDecisionOutput& DecisionOutput)
				{
					if (!CheckFunction) {
						DecisionOutput.OutputState = OutputState;
						DecisionOutput.bIsTransitionDecision = false;
						DecisionOutput.DecisionCallback = DecisionCallback(DecisionOutput); // Callback de configurado en Decision, en State.AddStateDecisions()
						DecisionOutput.bCallbackResult = bDecisionCallbackResult;			// resultado que se quiere pasar. ej: ShouldRun(true / false)
						return true;
					}

					return false;
				}

	
	*/

};
