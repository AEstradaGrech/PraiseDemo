// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BotDecision.h"
#include "../../Enums/CombatSystem/ETargetType.h"
#include "SeekDecision.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API USeekDecision : public UBotDecision
{
	GENERATED_BODY()
	
public:
	USeekDecision();
	FORCEINLINE float GetSearchRadius() { return SearchRadius; }
	FORCEINLINE ETargetType GetTargetType() { return TargetType; }
	FORCEINLINE bool IsSeekingAnyTarget() { return bIsSeekingAnyTarget; }
	FORCEINLINE bool IsSeekingLivingTarget() { return bIsSeekingLivingTargets; }
	FORCEINLINE bool IsLimitedDistance() { return bIsLimitedDistance; }
	FORCEINLINE bool ShouldDoFactionCheck() const { return bShouldDoFactionCheck; }

	FORCEINLINE void SetSearchRadius(float Value) { SearchRadius = Value; }
	FORCEINLINE void SetTargetType(ETargetType Type) { TargetType = Type; }
	FORCEINLINE void SetIsSeekingAnyTarget(bool bValue) { bIsSeekingAnyTarget = bValue; if (bValue) bIsSeekingLivingTargets = false; }
	FORCEINLINE void SetIsSeekingLivingTargets(bool bValue) { bIsSeekingLivingTargets = bValue; if (bValue) bIsSeekingAnyTarget = false; }
	FORCEINLINE void SetIsLimitedDistance(bool bValue) { bIsLimitedDistance = bValue; }
	FORCEINLINE void SetShouldSeekClosestTarget(bool bValue) { bShouldSeekClosestTarget = bValue; }
	FORCEINLINE void SetShouldDoFactionCheck(bool bValue) { bShouldDoFactionCheck = bValue; }
	FORCEINLINE void SetTargetAsPersonalEnemy(bool bValue) { bSetTargetAsPersonalEnemy = bValue; }

	virtual EBotDecision DecisionID() const override { return EBotDecision::SEEK; }
	virtual bool HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision) override;

	
private:
	float UpdateTargetSeconds;
	float LastUpdateTimestamp;
	bool bSetTargetAsPersonalEnemy;
	void UpdateTargets();
	bool bIsLimitedDistance;
	float SearchRadius;
	bool bIsSeekingAnyTarget;
	bool bIsSeekingLivingTargets;
	bool bShouldSeekClosestTarget;
	bool bShouldDoFactionCheck;
	bool ShouldUpdateTargets();
	ETargetType TargetType;
	UPROPERTY()
	TArray<AActor*> CurrentTargets;
	TArray<AActor*> GetTargets();
	bool IsTargetInRange(AActor* Target);
	void ProcessTarget(ITargeteable* Target, ETargetType MatchType, TArray<AActor*>& FilteredTargets);
};
