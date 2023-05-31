// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FactionZone.h"
#include "CreaturesFactionZone.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API ACreaturesFactionZone : public AFactionZone
{
	GENERATED_BODY()
public:
	ACreaturesFactionZone();

protected:
	virtual void SetZoneWaypoints() override;
	virtual void AddZoneBotKnownLocations(class ABaseBotCharacter* SpawnedBot, FString Zone);
	virtual void HandleCharDeath(AActor* DeadChar, AActor* Killer) override;
	virtual class AAIWaypoint* GetZoneSpawnPoint() override;
};
