// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryTargetInRadiusCheck.h"
#include "../../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../../Characters/Player/PraisePlayerCharacter.h"

UEnvQueryTargetInRadiusCheck::UEnvQueryTargetInRadiusCheck() : Super()
{

}

bool UEnvQueryTargetInRadiusCheck::CheckLocation(FVector PointLocation)
{
	if(!Querier) return false;

	if (!CheckedActor)
		CheckedActor = Querier;

	if (PointCheckResults.Contains(PointLocation)) return false;

	if (!ensure(Querier->GetWorld() != nullptr)) return false;

	if (bPreferEmptyLocs)
		bLowestValueFirst = true;

	SweepRadius = FMath::Clamp<float>(SweepRadius, 100, 1000);

	FCollisionShape ColShape = FCollisionShape::MakeSphere(SweepRadius);
	FCollisionQueryParams ColParams;
	ColParams.AddIgnoredActor(CheckedActor);
	
	if(CheckedActor != Querier)
		ColParams.AddIgnoredActor(Querier);

	TArray<AActor*> HittenActors;
	TArray<FHitResult> HitResults;
	if (Querier->GetWorld()->SweepMultiByChannel(HitResults, PointLocation, PointLocation, FQuat::Identity, ECollisionChannel::ECC_Visibility, ColShape, ColParams))
	{
		for (FHitResult& Hit : HitResults)
		{
			if (!Hit.Actor.IsValid()) continue;

			if (IsValidSweepTarget(Hit.Actor.Get()))
			{
				if (bIsExclusiveCheck)
				{
					bool bCanSeeFromPoint = false;

					if (bCheckTargetsVisibility)
						bCanSeeFromPoint = TargetCanSeePoint(Hit.Actor.Get(), PointLocation);
						
					return bPreferEmptyLocs ?  !bCanSeeFromPoint : true;
				}
					
				if (TargetType == ETargetType::ITEM || TargetType == ETargetType::VIP)
				{
					
					HittenActors.Add(Hit.Actor.Get());
					
					continue;
				}

				bool bIsEnemy = Cast<ABasePraiseCharacter>(CheckedActor)->IsEnemyTarget(Cast<IFactioneable>(Hit.Actor));

				if (bCheckAllies && !bIsEnemy)
				{
					HittenActors.Add(Hit.Actor.Get());
				}

				else
				{
					if (bIsEnemy)
						HittenActors.Add(Hit.Actor.Get());
				}
			}
		}

	}

	float Weight = HittenActors.Num();

	if (Weight == 0 && bPreferEmptyLocs)
		Weight = 1.f;

	PointCheckResults.Add(PointLocation, Weight);
	PointCheckResults.ValueSort([this](const float A, const float B) { return bLowestValueFirst ? A < B : A > B; });

	return true;
}

bool UEnvQueryTargetInRadiusCheck::TargetCanSeePoint(AActor* Target, const FVector& PointLocation)
{
	bool bHasHit = false;

	FCollisionQueryParams ColParams;
	ColParams.AddIgnoredActor(CheckedActor);

	if (CheckedActor != Querier) {
		ColParams.AddIgnoredActor(Querier);
	}

	FVector OriginLoc = bVisibilityFromEyesLoc && Target->IsA<APawn>() ? Cast<APawn>(Target)->GetPawnViewLocation() : Target->GetActorLocation();

	FVector FinalLocation = PointLocation + PointHeightOffset;
	FVector TraceOrigin = OriginLoc + TargetHeightOffset + CheckedActor->GetActorForwardVector() * 50.f;
	FVector QuerierToPoint = FinalLocation - TraceOrigin;
	FVector TraceEnd = TraceOrigin + (QuerierToPoint.GetSafeNormal() * QuerierToPoint.Size());

	FColor TraceCol = FColor::Green;
	FHitResult HitResult;

	return !Querier->GetWorld()->LineTraceSingleByChannel(HitResult, TraceOrigin, TraceEnd, ECollisionChannel::ECC_GameTraceChannel4, ColParams);
}

bool UEnvQueryTargetInRadiusCheck::IsValidSweepTarget(AActor* HittenActor)
{
	if (bCheckPlayersAndNPCs)
	{
		if (HittenActor && (HittenActor->IsA<ABaseBotCharacter>() || HittenActor->IsA<APraisePlayerCharacter>()))
		{
			if (Cast<ABasePraiseCharacter>(HittenActor)->IsDead())
				return false;

			return true;
		}

		return false;
	}

	return HittenActor->Implements<UTargeteable>();
}
