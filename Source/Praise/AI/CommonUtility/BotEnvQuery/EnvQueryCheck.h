// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EnvQueryCheck.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UEnvQueryCheck : public UObject
{
	GENERATED_BODY()
	
public:
	UEnvQueryCheck();

	FORCEINLINE bool IsExclusiveCheck() const { return bIsExclusiveCheck; }
	FORCEINLINE bool IsLowestValueFirst() const { return bLowestValueFirst; }
	FORCEINLINE TMap<FVector, float> GetCheckResults() const { return PointCheckResults; }
	FORCEINLINE AActor* GetQuerier() const { return Querier; }

	FORCEINLINE void SetIsExclusiveCheck(bool bValue) { bIsExclusiveCheck = bValue; }
	FORCEINLINE void SetIsLowestValueFirst(bool bValue) { bLowestValueFirst = bValue; }
	FORCEINLINE void SetQuerier(AActor* NewQuerier) { Querier = NewQuerier; }

	virtual bool CheckLocation(FVector PointLocation);

	/* DEVONLY*/
	bool bDebug;
protected:
	bool bIsExclusiveCheck;
	bool bLowestValueFirst;
	AActor* Querier;
	TMap<FVector, float> PointCheckResults; 

};
