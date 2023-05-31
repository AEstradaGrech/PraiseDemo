// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FactionZone.h"
#include "../../../Structs/AI/Factions/FFactionZoneDTStruct.h"
#include "../../../Interfaces/AI/GameFactionInterface.h"
#include "NeutralsFactionZone.generated.h"

UCLASS()
class PRAISE_API ANeutralsFactionZone : public AFactionZone
{
	GENERATED_BODY()
	
protected:
	virtual void SetZoneWaypoints() override;
	virtual void AddZoneBotKnownLocations(class ABaseBotCharacter* SpawnedBot, FString Zone);
};
