// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvQueryCheck.h"
#include "EnvQueryDistanceCheck.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UEnvQueryDistanceCheck : public UEnvQueryCheck
{
	GENERATED_BODY()
	
public:
	UEnvQueryDistanceCheck();

	FORCEINLINE void SetPreferClosestLocs(bool bValue) { bPreferClosestLocs = bValue; }
	FORCEINLINE void SetQueriedLocation(FVector Loc) { QueriedLocation = Loc; }

	virtual bool CheckLocation(FVector PointLocation) override;
private:

	bool bPreferClosestLocs;
	FVector QueriedLocation;

};
