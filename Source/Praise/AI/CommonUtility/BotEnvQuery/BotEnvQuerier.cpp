// Fill out your copyright notice in the Description page of Project Settings.


#include "BotEnvQuerier.h"
#include "GameFramework/Actor.h"
#include "../../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "EnvQueryDistanceCheck.h"
#include "EnvQueryVisibilityCheck.h"
#include "../../../Structs/CommonUtility/FLogger.h"
#include "Components/CapsuleComponent.h"



UBotEnvQuerier::UBotEnvQuerier()
{
	QueryRadius = 1000;
	QueryMaxAngle = 360;
	AngleStep = 45;
	QueryPointsDistance = 100;
	QueryPointOffset = 25.f;

	ProjectionData.TraceMode = EEnvQueryTrace::Navigation;
	ProjectionData.bCanProjectDown = true;
	ProjectionData.bCanDisableTrace = true;
	ProjectionData.ExtentX = 0.0f;
	bHasProximityCheck = true;
	bOmitQuerierLocation = true;

	bDrawPoints = false;
	bDrawBestResults = false;
}

UEnvQueryCheck* UBotEnvQuerier::GetQueryCheck(EEnvQueryCheck Check)
{
	return CreateQueryChecksMap.Contains(Check) ? (this->*CreateQueryChecksMap[Check])() : nullptr;
}

void UBotEnvQuerier::Setup(ABaseBotCharacter* QuerierOwner)
{
	Bot = QuerierOwner;

	SetDefaults();

	RegisterQueryChecks();

	if (!ensure(Bot->GetWorld() != nullptr)) return;

	UNavigationSystemV1* Mesh = UNavigationSystemV1::GetCurrent(Bot->GetWorld());

	if (Mesh)
		NavMesh = Mesh;
}

void UBotEnvQuerier::SetDefaults()
{
	QueryRadius = 1000;
	QueryMaxAngle = 360;
	AngleStep = 45;
	QueryPointsDistance = 150.f;
	QueryPointOffset = 1.f;
	PointsProximityTolerance = 50.f;
	bHasProximityCheck = true;
	RotationAxis = EAxis::Z;
	CylinderRingPairs = 1;
	CylinderRingsHeight = 100.f;
	bOmitQuerierLocation = true;

	if(Bot)
		Querier = Bot;
}

void UBotEnvQuerier::ValidateConfig(FEnvQueryConfig& Config)
{
	if (Config.MaxQueryAngle == 0 || Config.MaxQueryAngle > 360)
		Config.MaxQueryAngle = QueryMaxAngle;						

	if (Config.AngleStep == 0 || Config.AngleStep > Config.MaxQueryAngle)
		Config.AngleStep = AngleStep;

	if (!Config.Querier)
		Config.Querier = Bot;

	if (Config.QueryRadius <= 0)
		Config.QueryRadius = QueryRadius;

	if (Config.QueryPointsDistance <= 50.f)
		Config.QueryPointsDistance = QueryPointsDistance;

	if (Config.QueryPointOffset < 25.f)
		Config.QueryPointOffset = QueryPointOffset;

	if (Config.QueryPointOffset > Config.QueryPointsDistance)
		Config.QueryPointOffset = Config.QueryPointsDistance / 2;

	if (Config.PointsProximityTolerance <= 0)
		Config.PointsProximityTolerance = 50.f;

	CylinderRingPairs = Config.CylinderRingPairs;
	CylinderRingsHeight = Config.CylinderRingsHeight;

	if (Config.QueryMode == EEnvQueryMode::CYLINDRIC)
	{
		if (CylinderRingPairs <= 0)
			CylinderRingPairs = 1;

		if (CylinderRingsHeight <= 0)
			CylinderRingsHeight = 100.f;
	}
}

void UBotEnvQuerier::SetQueryParams(FEnvQueryConfig& Config)
{
	QueryMaxAngle = Config.MaxQueryAngle;
	AngleStep = Config.AngleStep;
	QueryRadius = Config.QueryRadius;
	QueryPointsDistance = Config.QueryPointsDistance;
	QueryPointOffset = Config.QueryPointOffset;
	PointsProximityTolerance = Config.PointsProximityTolerance;
	RotationAxis = Config.RotationAxis;
	bOmitQuerierLocation = Config.bOmitQuerierLocation;
	
	Querier = Config.Querier;
	QueryPointChecks = Config.QueryChecks;
	QueryConditions = Config.QueryConditions;
	QuerierLocation = !bIsArroundQuerier && FUtilities::IsValidVector(QuerierLocation) ? QuerierLocation : Querier->GetActorLocation();

	for (UEnvQueryCheck* Check : QueryPointChecks)
	{
		if (!Check->GetQuerier())
			Check->SetQuerier(Querier);
		
		if (Check->IsExclusiveCheck())
		{
			QueryConditions.Add([Check](FVector Point) { return Check->CheckLocation(Point); });
		}
	}

	if (bOmitQuerierLocation)
	{
		QueryConditions.Add([this](FVector Loc) { return (Loc - Querier->GetActorLocation()).Size() > 100.f; });
	}
}

void UBotEnvQuerier::ClearParams()
{
	QueryPointChecks.Empty();
	QueryConditions.Empty();
	ValidLocations.Empty();
	WeightedLocs.Empty();
	LocationsByAxis.Empty();

	SetDefaults();
}

bool UBotEnvQuerier::TryQuery(FEnvQueryConfig QueryConfig)
{
	ClearParams();
	
	ValidateConfig(QueryConfig);

	SetQueryParams(QueryConfig);

	if (QueryPointChecks.Num() <= 0 && QueryConditions.Num() <= 0) return false; 

	return TryQuery(QueryConfig.QueryMode);
}

bool UBotEnvQuerier::TryQuery(EEnvQueryMode Mode)
{
	if (!NavMesh)
	{
		UNavigationSystemV1* Mesh = UNavigationSystemV1::GetCurrent(GetWorld());

		if (!Mesh) return false;

		NavMesh = Mesh;
	}
	
	if (!CanQuery()) return false;

	switch (Mode)
	{
		case(EEnvQueryMode::SPHERIC):
			SphereQueryEnv(Querier->GetActorForwardVector());
			break;
		case(EEnvQueryMode::CYLINDRIC):
			CylinderQueryEnv(Querier->GetActorForwardVector());
			break;
		case(EEnvQueryMode::PLANE):
		default:
			bHasProximityCheck = false;
			QueryEnv(Querier->GetActorForwardVector(), RotationAxis);
			break;
	}

	if (!IsValid()) return false;
	
	ProjectAndFilterNavPoints(ValidLocations);

	if (bHasProximityCheck)
		ValidLocations = GetDistanceAdjustedPoints();

	CheckValidLocationConditions();

	if(bDrawPoints)
		for (int i = 0; i < ValidLocations.Num(); i++)
			DrawDebugSphere(Bot->GetWorld(), ValidLocations[i].Location, 50.f, 8, FColor::Red, false, 5.f);

	CheckValidLocations();

	bIsArroundQuerier = false;
	QuerierLocation = FVector::ZeroVector;
	CylinderRingPairs = 1;
	CylinderRingsHeight = 100.f;

	return WeightedLocs.Num() > 0; 
}



TArray<FVector> UBotEnvQuerier::GetBestResults(int MaxResults)
{
	if (MaxResults <= 0)
		MaxResults = 1;

	TArray<FVector> Results;
	
	if (WeightedLocs.Num() <= 0) return Results;

	TArray<FVector> Keys;
	WeightedLocs.GetKeys(Keys);
	for (int i = 0; i < Keys.Num(); i++)
	{
		if (Results.Num() >= MaxResults) break;

		FColor Col = FColor::Green;

		if (i == 1)
			Col = FColor::Cyan;
		if (i == 2)
			Col = FColor::Orange;

		if(bDrawBestResults)
			DrawDebugSphere(Bot->GetWorld(), Keys[i], 100.f, 8, Col, false, 5.f);

		Results.Add(Keys[i]);
	}

	return Results;
}

bool UBotEnvQuerier::CanQuery() const
{
	return Querier && QueryRadius > 0 && (QueryMaxAngle <= 360 && QueryMaxAngle > 0) && AngleStep > 0 && QueryPointsDistance >= 50.f;
}

bool UBotEnvQuerier::QueryEnv(FVector Fwd, float HeightOffset, EAxis::Type RotAxis)
{
	FVector Origin = (bIsArroundQuerier ? Querier->GetActorLocation() : QuerierLocation) + FVector(0, 0, HeightOffset);

	if (AngleStep == 0) return false;

	float Degs = QueryMaxAngle / 2;

	for (float Angle = -Degs; Angle < Degs; Angle += AngleStep)
	{
		TArray<FNavLocation> AxisLocs;

		FVector Vector = Fwd.RotateAngleAxis(Angle, GetRotationAxis(RotationAxis));

		int ScaledRadius = FMath::RoundToInt(QueryRadius / QueryPointsDistance);

		for (int Point = 0; Point < ScaledRadius; Point++)
		{
			FVector PointLoc = Origin + (Vector.GetSafeNormal() * Point * QueryPointsDistance);
			
			FColor SphereCol = FColor::Yellow;

			FNavLocation Loc(PointLoc);
			
			ValidLocations.Add(Loc);

			if(bDrawPoints)
				DrawDebugSphere(Querier->GetWorld(), PointLoc, 50.f, 8, SphereCol, false, 5.f);
			
			if (bHasProximityCheck)
				AxisLocs.Add(Loc);
		}

		if (bHasProximityCheck)
		{
			if (LocationsByAxis.Contains(Angle))
				LocationsByAxis[Angle].Append(AxisLocs);

			else LocationsByAxis.Add(Angle, AxisLocs);
		}
	}

	return ValidLocations.Num() > 0;
}

bool UBotEnvQuerier::SphereQueryEnv(FVector Fwd)
{
	float Degs = FMath::RoundToFloat(QueryMaxAngle / 2);
	FVector Vector;

	for (float Angle = -Degs; Angle < Degs; Angle += AngleStep)
	{
		Vector = Querier->GetActorForwardVector().RotateAngleAxis(Angle, Querier->GetActorRightVector());
		
		QueryEnv(Vector, EAxis::Z);		
	}

	return ValidLocations.Num() > 0;
}

bool UBotEnvQuerier::CylinderQueryEnv(FVector Fwd)
{
	float CapsuleOffset = 0.f;
	
	if (Querier->IsA<ACharacter>())
	{
		ACharacter* Char = Cast<ACharacter>(Querier);

		if (Char->GetCapsuleComponent())
			CapsuleOffset += Char->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}

	for (int i = 0; i <= CylinderRingPairs; i++)
	{
		float HeightOffset = CylinderRingsHeight * i;

		if (CapsuleOffset > 0) 
			HeightOffset += CapsuleOffset * FMath::Sign(HeightOffset);

		QueryEnv(Querier->GetActorForwardVector(), HeightOffset);
		
		if (i == 0) continue;

		QueryEnv(Querier->GetActorForwardVector(), -HeightOffset);
	}

	return true;
}

FVector UBotEnvQuerier::GetRotationAxis(EAxis::Type RotAxis)
{
	switch (RotationAxis)
	{
		case(EAxis::Y):
			return Querier->GetActorForwardVector();
		case(EAxis::X):
			return Querier->GetActorRightVector();
		case(EAxis::Z):
		default:
			return Querier->GetActorUpVector();
	}
	return FVector();
}

bool UBotEnvQuerier::IsValidLocation(FVector Loc)
{
	if (QueryConditions.Num() <= 0) return true;

	bool bIsValidLoc = true;
	for (int i = 0; i < QueryConditions.Num(); i++)
	{
		bool LambdaResult = QueryConditions[i](Loc);
		if (!LambdaResult)
		{
			bIsValidLoc = false;
			continue;
		}
	}

	return bIsValidLoc;
}

void UBotEnvQuerier::CheckValidLocations()
{
	if (QueryPointChecks.Num() <= 0) return;

	if (ValidLocations.Num() <= 0) return;
	
	for (int i = 0; i < QueryPointChecks.Num(); i++)
	{
		if (QueryPointChecks[i]->IsExclusiveCheck()) continue;

		for (int j = 0; j < ValidLocations.Num(); j++)
		{
			QueryPointChecks[i]->CheckLocation(ValidLocations[j].Location);
		}

		AddCheckResultWeights(QueryPointChecks[i]->GetCheckResults(), QueryPointChecks[i]->IsLowestValueFirst());
	}
}

void UBotEnvQuerier::CheckValidLocationConditions()
{
	if (QueryConditions.Num() <= 0) return;

	if (ValidLocations.Num() <= 0) return;

	TArray<FNavLocation> LocsCopy = ValidLocations;

	for (int i = 0; i < LocsCopy.Num(); i++)
		if (!IsValidLocation(LocsCopy[i].Location))
			ValidLocations.Remove(LocsCopy[i]);
}

void UBotEnvQuerier::AddCheckResultWeights(TMap<FVector, float> CheckResults, bool bLowestValueFirst)
{
	TArray<FVector> CheckedLocs;
	CheckResults.GetKeys(CheckedLocs);
	float Weight = 0.f;
	for (int i = 0; i < CheckedLocs.Num(); i++)
	{
		bool bCanDivide = !(CheckResults[CheckedLocs[0]] == 0.f && CheckResults[CheckedLocs[i]] == 0.f);

		if(bLowestValueFirst)
			Weight = bCanDivide ? (CheckResults[CheckedLocs[0]] / CheckResults[CheckedLocs[i]]) / QueryPointChecks.Num() : 1;
		
		else Weight = bCanDivide ? (CheckResults[CheckedLocs[i]] / CheckResults[CheckedLocs[0]]) / QueryPointChecks.Num() : 0;

		if (WeightedLocs.Contains(CheckedLocs[i]))
			WeightedLocs[CheckedLocs[i]] += Weight;

		else WeightedLocs.Add(CheckedLocs[i], Weight);
	}

	WeightedLocs.ValueSort([](const float A, const float B) { return A > B; });
}

TArray<FNavLocation> UBotEnvQuerier::GetDistanceAdjustedPoints()
{
	TArray<FNavLocation> FinalLocations;

	if (PointsProximityTolerance <= 0.f)
		PointsProximityTolerance = 50.f;

	TArray<float> Keys;
	LocationsByAxis.GetKeys(Keys);

	for (float Key : Keys)
		CheckAxisLocationsTolerance(LocationsByAxis[Key], FinalLocations);

	return FinalLocations;
}

void UBotEnvQuerier::CheckAxisLocationsTolerance(TArray<FNavLocation>& AxisLocations, TArray<FNavLocation>& FinalLocations)
{
	ProjectAndFilterNavPoints(AxisLocations);

	TMap<float, FNavLocation> SortedFwdLocs;
	TMap<float, FNavLocation> SortedBwdLocs;
	FVector QuerierToLoc;
	for (FNavLocation Loc : AxisLocations)
	{	
		QuerierToLoc = Loc.Location - Querier->GetActorLocation();

		float Distance = QuerierToLoc.Size() * (Bot->IsTargetForward(Loc.Location) ? 1 : -1);

		if (Distance > 0)
			SortedFwdLocs.Add(Distance, Loc);

		else SortedBwdLocs.Add(Distance, Loc);
	}

	SortedFwdLocs.KeySort([](const float A, const float B) { return A > B; });
	SortedBwdLocs.KeySort([](const float A, const float B) { return A < B; });

	TArray<float> Keys;
	SortedFwdLocs.GetKeys(Keys);
	TArray<FNavLocation> FilteredLocations;

	GetFilteredAxisLocations(Keys, SortedFwdLocs, FilteredLocations);
	
	Keys.Empty();
	SortedBwdLocs.GetKeys(Keys);

	GetFilteredAxisLocations(Keys, SortedBwdLocs, FilteredLocations);
	
	FinalLocations.Append(FilteredLocations);
}

void UBotEnvQuerier::GetFilteredAxisLocations(TArray<float>& SortedKeys, TMap<float, FNavLocation>& SortedLocs, TArray<FNavLocation>& ResultLocs)
{
	if (SortedKeys.Num() <= 0) return;

	TArray<float> KeysCopy = SortedKeys;
	if (SortedKeys.Num() < 2 && SortedKeys[0])
	{
		ResultLocs.Add(SortedLocs[SortedKeys[0]]);
		SortedKeys.Empty();
		return;
	}

	if (FMath::Abs(KeysCopy[0]) - FMath::Abs(KeysCopy[1]) <= PointsProximityTolerance)
	{
		SortedKeys.Remove(KeysCopy[1]);
	}
	else
	{
		ResultLocs.Add(SortedLocs[SortedKeys[0]]);
		SortedKeys.Remove(KeysCopy[0]);

		if (SortedKeys.Num() <= 1)
		{
			ResultLocs.Add(SortedLocs[SortedKeys[0]]);
			SortedKeys.Empty();
			return;
		}

	}

	GetFilteredAxisLocations(SortedKeys, SortedLocs, ResultLocs);
}


void UBotEnvQuerier::RegisterQueryChecks()
{
	RegisterQueryCheck<UEnvQueryCheck>(EEnvQueryCheck::GENERIC);
	RegisterQueryCheck<UEnvQueryVisibilityCheck>(EEnvQueryCheck::VISIBILITY);
	RegisterQueryCheck<UEnvQueryDistanceCheck>(EEnvQueryCheck::DISTANCE);
}

void UBotEnvQuerier::ProjectAndFilterNavPoints(TArray<FNavLocation>& Points)
{
	const ANavigationData* NavData = nullptr;
	if (ProjectionData.TraceMode != EEnvQueryTrace::None)
		NavData = FindNavigationDataForQuery();
	
	const UObject* CompOwner = Bot;
	
	if (NavData && Querier && (ProjectionData.TraceMode == EEnvQueryTrace::Navigation))
		RunNavProjection(*NavData, *Bot, ProjectionData, Points);
}

// From UE source code
ANavigationData* UBotEnvQuerier::FindNavigationDataForQuery()
{
	if (!NavMesh)
	{
		UNavigationSystemV1* Mesh = UNavigationSystemV1::GetCurrent(Bot->GetWorld());

		if (!Mesh) return nullptr;

		NavMesh = Mesh;
	}

	// try to match navigation agent for querier
	INavAgentInterface* NavAgent = Bot ? Cast<INavAgentInterface>(Bot) : NULL;
	if (NavAgent)
	{
		const FNavAgentProperties& NavAgentProps = NavAgent->GetNavAgentPropertiesRef();
		return NavMesh->GetNavDataForProps(NavAgentProps, NavAgent->GetNavAgentLocation());
	}

	return NavMesh->GetDefaultNavDataInstance();
}

// From UE source code
void UBotEnvQuerier::RunNavProjection(const ANavigationData& NavData, const UObject& CompOwner, const FEnvTraceData& TraceData, TArray<FNavLocation>& Points, bool bDiscardUnprojected)
{
	FSharedConstNavQueryFilter NavigationFilter = UNavigationQueryFilter::GetQueryFilter(NavData, &CompOwner, TraceData.NavigationFilter);
	TArray<FNavigationProjectionWork> Workload;
	Workload.Reserve(Points.Num());

	if (TraceData.ProjectDown == TraceData.ProjectUp)
	{
		for (const auto& Point : Points)
		{
			Workload.Add(FNavigationProjectionWork(Point.Location));
		}
	}
	else
	{
		const FVector VerticalOffset = FVector(0, 0, (TraceData.ProjectUp - TraceData.ProjectDown) / 2);
		for (const auto& Point : Points)
		{
			Workload.Add(FNavigationProjectionWork(Point.Location + VerticalOffset));
		}
	}

	const FVector ProjectionExtent(TraceData.ExtentX, TraceData.ExtentX, (TraceData.ProjectDown + TraceData.ProjectUp) / 2);
	NavData.BatchProjectPoints(Workload, ProjectionExtent, NavigationFilter);

	for (int32 Idx = Workload.Num() - 1; Idx >= 0; Idx--)
	{
		if (Workload[Idx].bResult)
		{
			Points[Idx] = Workload[Idx].OutLocation;
			Points[Idx].Location.Z += TraceData.PostProjectionVerticalOffset;
		}
		else if (bDiscardUnprojected)
		{
			Points.RemoveAt(Idx, 1, false);
		}
	}
}
