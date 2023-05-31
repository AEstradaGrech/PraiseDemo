// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvQueryCheck.h"
#include "EnvQueryVisibilityCheck.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UEnvQueryVisibilityCheck : public UEnvQueryCheck
{
	GENERATED_BODY()
	
public:
	UEnvQueryVisibilityCheck();

	FORCEINLINE bool IsObjectQuery() const { return bIsObjectQuery; }
	FORCEINLINE bool IsFromQuerierEyesLoc() const { return bFromQuerierEyesLoc; }
	
	FORCEINLINE void SetIsObjectQuery(bool bValue) { bIsObjectQuery = bValue; }
	FORCEINLINE void SetPointHeightOffset(FVector Offset) { PointHeightOffset = Offset; }
	FORCEINLINE void SetQuerierHeightOffset(FVector Offset) { QuerierHeightOffset = Offset; }
	FORCEINLINE void SetIsFromQuerierEyesLoc(bool bValue) { bFromQuerierEyesLoc = bValue; }
	FORCEINLINE void SetPreferNonVisible(bool bValue) { bPreferNonVisible = bValue; }
	FORCEINLINE void SetCheckedActor(AActor* Actor) { CheckedActor = Actor; }

	virtual bool CheckLocation(FVector PointLocation) override;

private:
	bool bIsObjectQuery;
	bool bPreferNonVisible;
	bool bFromQuerierEyesLoc;
	FVector PointHeightOffset;
	FVector QuerierHeightOffset;

	AActor* CheckedActor;

	
};
