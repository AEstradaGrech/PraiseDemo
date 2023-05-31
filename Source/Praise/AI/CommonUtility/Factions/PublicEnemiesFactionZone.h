// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FactionZone.h"
#include "../../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "PublicEnemiesFactionZone.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API APublicEnemiesFactionZone : public AFactionZone
{
	GENERATED_BODY()
public:
	APublicEnemiesFactionZone();
protected:
	virtual void SetZoneWaypoints() override;
	virtual void HandleCharDeath(AActor* DeadChar, AActor* Killer);
	virtual class AAIWaypoint* GetZoneSpawnPoint() override;
	virtual void AddZoneBotKnownLocations(class ABaseBotCharacter* SpawnedBot, FString Zone);
};
