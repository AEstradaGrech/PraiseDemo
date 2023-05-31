// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "../../Enums/AI/EBrainType.h"
#include "BaseBotController.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API ABaseBotController : public AAIController
{
	GENERATED_BODY()
	
public:
	ABaseBotController();

	FORCEINLINE class UBotBrainComponent* GetBotBrain() const { return BotBrain; }

	template<class T>
	void OverrideBrainComponent();
	template<class T>
	bool HasBrainOfType() const;
	template<class T>
	T* GetBrainAs() const;
	virtual void OnPossess(APawn* InPawn) override;
	bool TryReachLocation(TArray<FVector>& Locations, FVector& SelectedLocation, float StoppingDistance = 50.f, bool bWorstLocFirst = false);
	void SetBotBrain(EBrainType BrainType);
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = BotBrain)
		class UBotBrainComponent* BotBrain;
};

template<class T>
inline bool ABaseBotController::HasBrainOfType() const
{
	return BotBrain ? BotBrain->IsA<T>() : false;
}

template<class T>
inline T* ABaseBotController::GetBrainAs() const
{
	return BotBrain ? Cast<T>(BotBrain) : nullptr;
}

template<class T>
inline void ABaseBotController::OverrideBrainComponent()
{
	T* NewBrain = NewObject<T>(this);

	if (!NewBrain) return;

	NewBrain->RegisterComponent();
	NewBrain->InitializeComponent();

	BotBrain = NewBrain;
}


