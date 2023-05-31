// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFactionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGameFactionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PRAISE_API IGameFactionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual class UBotSpawnerComponent* GetBotSpawner() const = 0;
	virtual class UMapLocationsManagerComponent* GetMapLocationsManager() const = 0;
	virtual TMap<FString, TSubclassOf<class ABaseBotCharacter>> GetFactionBotBPs() const = 0;
	virtual TArray<class ANodeWaypoint*> GetZoneNodes(FString ZoneName) const = 0;
	virtual TArray<class ANodeWaypoint*> GetZoneNodes(ECharFaction NodeFaction, FString ZoneName) const = 0;
	virtual TArray<class ANodeWaypoint*> GetPathNodes() const = 0;
	virtual TArray<class ANodeWaypoint*> GetLocations() const = 0;
	virtual class UWaypointHandler* GetWaypointHandler() const = 0;
	virtual void AddZonePaths(class AFactionZone* Zone) = 0;
	virtual TArray<class UAIPath*> GetCommonPaths() = 0;
	virtual TArray<class UAIPath*> GetZonePaths(FString ZoneName = "") = 0;
	virtual TArray<class UAIPath*> GetRoutePaths(TArray<class ANodeWaypoint*>& RouteNodes) = 0;
	virtual class UFactionsManagerComponent* GetFactionsManager() const = 0;
};
