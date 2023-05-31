// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NavigationSystem.h"
#include "EnvQueryCheck.h"
#include "../../../Enums/AI/EEnvQueryCheck.h"
#include "../../../Enums/AI/EEnvQueryMode.h"
#include "../../../Structs/AI/FEnvQueryConfig.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "BotEnvQuerier.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UBotEnvQuerier : public UObject
{
	GENERATED_BODY()
	
public:
	

	UBotEnvQuerier();

	FORCEINLINE bool IsValid() const { return ValidLocations.Num() > 0; }
	FORCEINLINE TArray<FVector> GetQueriedPoints() const { return QueryPoints; }
	FORCEINLINE TArray<FNavLocation> GetValidLocations() const { return ValidLocations; }
	
	FORCEINLINE float GetQueryRadius() const { return QueryRadius; }
	FORCEINLINE float GetQueryMaxAngle() const { return QueryMaxAngle; }
	FORCEINLINE float GetAngleStep() const { return AngleStep; }
	FORCEINLINE float GetQueryPointsDistance() const { return QueryPointsDistance; }
	FORCEINLINE float GetQueryPointOffset() const { return QueryPointOffset; }
	FORCEINLINE float GetQueryPointsTolerance() const { return PointsProximityTolerance; }
	FORCEINLINE bool HasProximityCheck() const { return bHasProximityCheck; }

	FORCEINLINE void SetQueryRadius(float Value) { QueryRadius = Value; }
	FORCEINLINE void SetQueryMaxAngle(float Value) { QueryMaxAngle = Value; }
	FORCEINLINE void SetQueryAngleStep(float Value) { AngleStep = Value; }
	FORCEINLINE void SetQueryPointsDistance(float Value) { QueryPointsDistance = Value; }
	FORCEINLINE void SetQueryPointOffset(float Value) { QueryPointOffset = Value; }
	FORCEINLINE void SetQueryPointsTolerance(float Value) { PointsProximityTolerance = Value; }
	FORCEINLINE void SetHasProximityCheck(bool bValue) { bHasProximityCheck = bValue; }
	FORCEINLINE void SetIsArroundQuerier(bool bValue) { bIsArroundQuerier = bValue; }
	FORCEINLINE void SetQuerierLocation(FVector Loc) { QuerierLocation = Loc; }
	FORCEINLINE void SetCylinderRingsPairs(int Value) { CylinderRingPairs = Value; }
	FORCEINLINE void SetCylinderRingsHeight(float Value) { CylinderRingsHeight = Value; }

	typedef UEnvQueryCheck* (UBotEnvQuerier::* pCreateQueryCheckFn)();

	UEnvQueryCheck* GetQueryCheck(EEnvQueryCheck Check);
	void Setup(class ABaseBotCharacter* QuerierOwner);
	void SetDefaults();
	void ValidateConfig(FEnvQueryConfig& Config);
	void SetQueryParams(FEnvQueryConfig& Config);
	void ClearParams();

	bool TryQuery(FEnvQueryConfig QueryConfig);
	

	TArray<FVector> GetBestResults(int MaxResults = 1);

	/* DEV ONLY */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bDrawPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bDrawBestResults;
	
private:

	bool CanQuery() const;
	bool TryQuery(EEnvQueryMode Mode);
	bool QueryEnv(FVector Fwd, float HeightOffset = 0.f, EAxis::Type RotAxis = EAxis::Z);
	bool SphereQueryEnv(FVector Fwd);
	bool CylinderQueryEnv(FVector Fwd);
	bool IsValidLocation(FVector Loc);
	void CheckValidLocations();
	void CheckValidLocationConditions();
	void AddCheckResultWeights(TMap<FVector, float> CheckResults, bool bLowestValueFirst);
	TArray<FNavLocation> GetDistanceAdjustedPoints();
	void CheckAxisLocationsTolerance(TArray<FNavLocation>& AxisLocations, TArray<FNavLocation>& FinalLocations);
	void GetFilteredAxisLocations(TArray<float>& SortedKeys, TMap<float, FNavLocation>& SortedLocs, TArray<FNavLocation>& ResultLocs);
	FVector GetRotationAxis(EAxis::Type RotnAxis = EAxis::Z);

	class ABaseBotCharacter* Bot;
	class AActor* Querier;
	EAxis::Type RotationAxis;
	int CylinderRingPairs;
	float CylinderRingsHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float QueryRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float QueryMaxAngle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float AngleStep;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float QueryPointsDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float QueryPointOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float PointsProximityTolerance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bHasProximityCheck;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bIsArroundQuerier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bOmitQuerierLocation;
	UPROPERTY(EditDefaultsOnly, Category = Generator)
		FEnvTraceData ProjectionData;

	

	FVector QuerierLocation;
	UNavigationSystemV1* NavMesh;
	TArray<FVector> QueryPoints;
	TMap<float, TArray<FNavLocation>> LocationsByAxis;
	TArray<FNavLocation> ValidLocations;
	TArray<TFunction<bool(FVector)>> QueryConditions; 
	UPROPERTY()
	TArray<UEnvQueryCheck*> QueryPointChecks; 
	UPROPERTY()
	TMap<FVector, float> WeightedLocs;
	TMap<EEnvQueryCheck, pCreateQueryCheckFn> CreateQueryChecksMap;

	void RegisterQueryChecks();
	void ProjectAndFilterNavPoints(TArray<FNavLocation>& Points);
	ANavigationData* FindNavigationDataForQuery();
	void RunNavProjection(const ANavigationData& NavData, const UObject& CompOwner, const FEnvTraceData& TraceData, TArray<FNavLocation>& Points, bool bDiscardUnprojected = true);

	template<typename T>
	UEnvQueryCheck* CreateQueryCheck() { return NewObject<T>(); }

	template<typename T>
	void RegisterQueryCheck(EEnvQueryCheck Check) { if (!CreateQueryChecksMap.Contains(Check)) CreateQueryChecksMap.Add(Check, &UBotEnvQuerier::CreateQueryCheck<T>); };


};
