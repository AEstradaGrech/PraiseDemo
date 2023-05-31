// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvQueryCheck.h"
#include "../../../Enums/CombatSystem/ETargetType.h"
#include "EnvQueryTargetInRadiusCheck.generated.h"


UCLASS()
class PRAISE_API UEnvQueryTargetInRadiusCheck : public UEnvQueryCheck
{
	GENERATED_BODY()
	
public:
	UEnvQueryTargetInRadiusCheck();

	FORCEINLINE void SetTargetType(ETargetType Type) { TargetType = Type; }
	FORCEINLINE void SetCheckPlayersAndNPCs(bool bValue) { bCheckPlayersAndNPCs = bValue; }
	FORCEINLINE void SetPointSweepRadius(float Radius) { SweepRadius = Radius; }
	FORCEINLINE void SetCheckAllies(bool bValue) { bCheckAllies = bValue; }
	FORCEINLINE void SetIncludeNeutrals(bool bValue) { bIncludeNeutrals = bValue; }
	FORCEINLINE void SetPreferEmptyLocations(bool bValue) { bPreferEmptyLocs = bValue; }
	FORCEINLINE void SetCheckTargetsVisibility(bool bValue) { bCheckTargetsVisibility = bValue; }
	FORCEINLINE void SetPointHeightOffset(FVector Offset) { PointHeightOffset = Offset; }
	FORCEINLINE void SetTargetHeightOffset(FVector Offset) { TargetHeightOffset = Offset; }
	FORCEINLINE void SetVisibilityFromTargetEyesLoc(bool bValue) { bVisibilityFromEyesLoc = bValue; }
private:
	float SweepRadius;
	ETargetType TargetType;
	bool bCheckPlayersAndNPCs;
	bool bCheckAllies;
	bool bIncludeNeutrals;
	bool bPreferEmptyLocs;
	AActor* CheckedActor;
	bool bCheckTargetsVisibility;
	bool bVisibilityFromEyesLoc;
	FVector PointHeightOffset;
	FVector TargetHeightOffset;

	virtual bool CheckLocation(FVector PointLocation) override;
	bool TargetCanSeePoint(AActor* Target, const FVector& PointLocation);
	bool IsValidSweepTarget(AActor* HittenActor);
};
