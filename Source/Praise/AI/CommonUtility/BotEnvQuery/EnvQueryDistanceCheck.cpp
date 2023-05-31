// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryDistanceCheck.h"
#include "../../../Structs/CommonUtility/FUtilities.h"

UEnvQueryDistanceCheck::UEnvQueryDistanceCheck() : Super()
{
	QueriedLocation = FVector::ZeroVector;
}

bool UEnvQueryDistanceCheck::CheckLocation(FVector PointLocation)
{
	if (!Querier) return false;

	if (!FUtilities::IsValidVector(QueriedLocation))
		QueriedLocation = Querier->GetActorLocation();

	if (PointCheckResults.Contains(PointLocation)) return false;

	bLowestValueFirst = bPreferClosestLocs;

	FVector QuerierToPoint = PointLocation - QueriedLocation;

	PointCheckResults.Add(PointLocation, QuerierToPoint.Size());

	PointCheckResults.ValueSort([this](const float A, const float B) { return bLowestValueFirst ? A < B : A > B; });

	return true;
}
