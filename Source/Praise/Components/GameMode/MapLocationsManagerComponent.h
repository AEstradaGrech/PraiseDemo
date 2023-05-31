// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Enums/Characters/ECharFaction.h"
#include "MapLocationsManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRAISE_API UMapLocationsManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMapLocationsManagerComponent();

	FORCEINLINE TMap<ECharFaction, TArray<class ANodeWaypoint*>> GetFactionNodesMap() { return FactionNodes; }
	FORCEINLINE TMap<FString, class UAIPath*> GetLevelPathsMap() const { return LevelPaths; }

	TArray<class AAIWaypoint*> GetZoneBuildingLinks(FString ZoneName) const;
	void AddPath(UAIPath* NewPath);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TMap<FString, class UAIPath*> LevelPaths;
	TMap<ECharFaction, TArray<class ANodeWaypoint*>> FactionNodes;
	TMap<FString, TArray<class AAIWaypoint*>> ZoneBuildingLinks;
	bool IsValidCommonNode(class ANodeWaypoint* Node);
	void AddFactionNode(ECharFaction Faction, ANodeWaypoint* Node);
	
public:	

	TArray<class AAIWaypoint*> GetCommonWaypointsByTag(FString WaypointTag);
	void CacheCommonMapLocations(bool bCacheLevelPaths = true); 
	void SetupLevelPaths();
	TArray<class ANodeWaypoint*> GetFactionNodes(ECharFaction Faction);
	TArray<class ANodeWaypoint*> GetFactionZoneNodes(ECharFaction Faction, FString ZoneName);
	TArray<class ANodeWaypoint*> GetAllNodes();
	TArray<class ANodeWaypoint*> SelectFactionNodes(ECharFaction Faction, TArray<class ANodeWaypoint*> Collection);
	class ANodeWaypoint* FindNode(FString NodeName, TArray<class ANodeWaypoint*> Collection);

	class UAIPath* GetPath(FString PathName);
	class UAIPath* GetPath(class ANodeWaypoint* NodeA, class ANodeWaypoint* NodeB);
	TArray<class UAIPath*> GetFactionPaths(ECharFaction Faction);
};
