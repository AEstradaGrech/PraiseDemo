// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FightingBotCharacter.h"
#include "SeekerBotCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API ASeekerBotCharacter : public AFightingBotCharacter
{
	GENERATED_BODY()
	
public:
	ASeekerBotCharacter();
	FORCEINLINE bool IsSeekingClosestTarget() const { return bSeekClosestTarget; }
	FORCEINLINE bool IsSeekingOnlyPlayers() const { return bSeekOnlyPlayers; }
	FORCEINLINE float GetSearchDistance() const { return SearchDistance; }
	FORCEINLINE bool ShouldWanderIfNoTargets() const { return bShouldWanderIfNoTargets && MaxTimeWandering > 0.f; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		bool bSeekClosestTarget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		float SearchDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		bool bSeekOnlyPlayers;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		bool bShouldWanderIfNoTargets;
	
};
