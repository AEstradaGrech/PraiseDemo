// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryVisibilityCheck.h"
#include "DrawDebugHelpers.h"
#include "../../../Structs/CommonUtility/FLogger.h"
#include "../../../Characters/BasePraiseCharacter.h"
#include "GameFramework/Character.h"


UEnvQueryVisibilityCheck::UEnvQueryVisibilityCheck() : Super()
{

}

bool UEnvQueryVisibilityCheck::CheckLocation(FVector PointLocation)
{
	if (!Querier) return false;

	if (!CheckedActor)
		CheckedActor = Querier;

	if (!CheckedActor->IsA<ACharacter>())
		bFromQuerierEyesLoc = false;

	if (PointCheckResults.Contains(PointLocation)) return false;

	if (!ensure(Querier->GetWorld() != nullptr)) return false;
	FVector OriginLoc = bFromQuerierEyesLoc ? Cast<ACharacter>(CheckedActor)->GetPawnViewLocation() : CheckedActor->GetActorLocation();
	float Weight = bPreferNonVisible ? 0.f : 1.f;
	FVector FinalLocation = PointLocation + PointHeightOffset;
	FVector TraceOrigin = OriginLoc + QuerierHeightOffset;
	FVector QuerierToPoint =  FinalLocation - TraceOrigin;
	FVector TraceEnd = TraceOrigin + (QuerierToPoint.GetSafeNormal() * QuerierToPoint.Size());
	
	FColor TraceCol = FColor::Red;
	TArray<FHitResult> HitResults;
	FHitResult BlockResult;
	bool bHasHit = false;
	if (bIsObjectQuery)
	{
		FCollisionObjectQueryParams QueryParams;
		QueryParams.ObjectTypesToQuery = QueryParams.AllStaticObjects;

		FCollisionQueryParams ColParams;
		ColParams.AddIgnoredActor(CheckedActor);
		
		if (CheckedActor != Querier) {
			ColParams.AddIgnoredActor(Querier);
		}

		bHasHit = Querier->GetWorld()->LineTraceMultiByObjectType(HitResults, TraceOrigin, TraceEnd, QueryParams, ColParams);
	}
	else
	{
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(CheckedActor);

		if (CheckedActor != Querier) {
			QueryParams.AddIgnoredActor(Querier);
		}
		
		bHasHit = Querier->GetWorld()->LineTraceSingleByChannel(BlockResult, TraceOrigin, TraceEnd, ECollisionChannel::ECC_GameTraceChannel4, QueryParams);
	}

	if (bHasHit) 
	{	
		if (bIsObjectQuery) 
			for (FHitResult Hit : HitResults) 
				bHasHit = Hit.Actor.IsValid() && !Hit.Actor.Get()->IsA<APawn>();
				
		if (bHasHit)
			Weight = bPreferNonVisible ? 1.f : 0.f;
		
	}

	if (Weight > 0 && bDebug)
	{
		DrawDebugSphere(Querier->GetWorld(), TraceEnd, 50.f, 8, FColor::Magenta, false, 5.f);
	}

	PointCheckResults.Add(PointLocation, Weight);
	PointCheckResults.ValueSort([this](const float A, const float B) { return bLowestValueFirst ? A < B : A > B; });

	return bIsExclusiveCheck ? Weight > 0 : true;
}

