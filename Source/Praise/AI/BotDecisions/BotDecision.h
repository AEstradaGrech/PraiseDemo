// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../Structs/AI/FDecisionOutput.h"
#include "../../Enums/AI/EBotDecision.h"
#include "../../Components/Actor/AI/BotBrainComponent.h"
#include "BotDecision.generated.h"

/**
 * Se pueden encadenar. La anterior configura el comportamiento de la siguiente
 * Se pueden instanciar ya encadenadas en OnEnter pero tambien se pueden encadenar desde una Decision para casos especificos
 * Tambien pueden emitir un resultado tipo TFunction<void(bool)> para configurar algo antes de la siguiente (y desconocida) Decision del Loop
 */
UCLASS()
class PRAISE_API UBotDecision : public UObject
{
	GENERATED_BODY()
	
public:
	UBotDecision();
	virtual EBotDecision DecisionID() const { return EBotDecision::STAY_IN_IDLE; }
	FORCEINLINE void ChainDecision(UBotDecision* NextDecision) { DependantDecision =  IsChained() ? DependantDecision : NextDecision; }
	FORCEINLINE void OverrideChainedDecision(UBotDecision* NextDecision) { DependantDecision = NextDecision; }
	FORCEINLINE bool IsChained() { return DependantDecision != nullptr; }
	FORCEINLINE bool HasPreviousChain() { return PrevDecision != nullptr; }
	virtual bool HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision = nullptr);
	FORCEINLINE bool HasValidBrain() const { return BotBrain != nullptr; }
	FORCEINLINE void SetBotBrain(UBotBrainComponent* Brain) { BotBrain = Brain; }
	bool ChainResult(FDecisionOutput& DecisionOutput);
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		UBotBrainComponent* BotBrain;

	UPROPERTY()
		UBotDecision* DependantDecision;
	UPROPERTY()
		UBotDecision* PrevDecision;
	
	bool bIsChained;
};
