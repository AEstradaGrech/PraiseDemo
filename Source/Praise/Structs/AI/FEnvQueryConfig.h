#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "../../Enums/AI/EEnvQueryCheck.h"
#include "../../Enums/AI/EEnvQueryMode.h"
#include "../../AI/CommonUtility/BotEnvQuery/EnvQueryCheck.h"
#include "FEnvQueryConfig.generated.h"

USTRUCT()
struct PRAISE_API FEnvQueryConfig
{
	GENERATED_BODY()

		FEnvQueryConfig() : 
			Querier(nullptr), QueryMode(EEnvQueryMode::PLANE), MaxQueryAngle(360), AngleStep(45), QueryRadius(1000), QueryPointsDistance(100), PointsProximityTolerance(50), bPreferClosestTargets(true), RotationAxis(EAxis::Z) {};

		FEnvQueryConfig(AActor* Origin, EEnvQueryMode Mode, float MaxAngle, float Step, float Radius, float PointsDistance, float PointOffset, float ProximityTolerance, bool bPreferClosest) :
			Querier(Origin), QueryMode(Mode), MaxQueryAngle(MaxAngle), AngleStep(Step), QueryRadius(Radius), QueryPointsDistance(PointsDistance), QueryPointOffset(PointOffset), PointsProximityTolerance(ProximityTolerance), bPreferClosestTargets(bPreferClosest) {};

		AActor* Querier;
		EEnvQueryMode QueryMode = EEnvQueryMode::PLANE;
		float MaxQueryAngle = 360;
		float AngleStep = 0;
		float QueryRadius = 0;
		float QueryPointsDistance = 0;
		float QueryPointOffset = 0;
		float PointsProximityTolerance = 0;
		bool bPreferClosestTargets = true;
		EAxis::Type RotationAxis = EAxis::Z;
		int CylinderRingPairs = 1;
		int CylinderRingsHeight = 150.f;
		bool bOmitQuerierLocation = true;

		TArray<UEnvQueryCheck*> QueryChecks; 
		TArray<TFunction<bool(FVector)>> QueryConditions;

};