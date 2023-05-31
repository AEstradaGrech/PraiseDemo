// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AIWaypoint.h"
#include "NodeWaypoint.h"
#include "WaypointHandler.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UWaypointHandler : public UObject
{
	GENERATED_BODY()
public:
	UWaypointHandler();

	FORCEINLINE TMap<FString, class UAIPath*> GetPathsMap() const { return Paths; }
	FORCEINLINE TMap<FString, class UAIRoute*> GetRoutesMap() const { return Routes; }

	TArray<ANodeWaypoint*> GetPathNodes() const { return PathNodes; }
	TArray<ANodeWaypoint*> GetVipNodes() const { return VipNodes; }
	TArray<ANodeWaypoint*> GetVipNodes(FName Tag) const;
	TArray<ANodeWaypoint*> GetVipNodes(FName MainTag, TArray<FName> FilterTags, bool bIsAndMatch = true) const;
	TArray<class UAIPath*> GetAllPaths() const;

	TArray<AAIWaypoint*> GetWaypoints(FString Label = "") const;
	TArray<AAIWaypoint*> GetWaypoints(FName Tag, FString Label = "") const;
	class UAIPath* GetPath(FString PathName) const;
	class UAIRoute* GetRoute(FString RouteName);

	void UpdateNodeArray(ANodeWaypoint* Node, bool bToVipArray, bool bClearExistent = false);
	void AddNode(ANodeWaypoint* Node, bool bIsVip = false, bool bClearExistent = false);
	void AddNodes(TArray<ANodeWaypoint*> Nodes, bool bIsVipCollection = false, bool bClearExistent = false);
	bool FindNode(int NodeID, ANodeWaypoint*& OutNode, bool bIsVip = false);
	bool FindNode(int NodeID, ANodeWaypoint*& OutNode, TArray<ANodeWaypoint*> Nodes, int Max = 0, int Min = 0);
	bool FindNode(FString NodeName, ANodeWaypoint*& OutNode, bool bIsVip = false);
	bool FindNodeByLabel(FString NodeName, ANodeWaypoint*& OutNode, bool bIsVip = false);
	bool FindNodeByLabel(FString NodeName, ANodeWaypoint*& OutNode, TArray<ANodeWaypoint*> Nodes);
	bool FindNode(FName Tag, ANodeWaypoint*& OutNode, bool bIsVip = false);
	class ANodeWaypoint* GetClosestNode(FVector Location, bool bOnlyVipNodes = false);
	class ANodeWaypoint* GetClosestNode(FVector Location, TArray<ANodeWaypoint*> Nodes, FName NodeTag = FName(""));
	class ANodeWaypoint* GetClosestNode(class AActor* Character, TArray<ANodeWaypoint*> Nodes);
	class ANodeWaypoint* GetClosestNode(class AActor* Character, bool bOnlyVipNodes = false);
	void AddWaypoint(FString WaypointLabel, AAIWaypoint* Waypoint);
	void AddWaypoints(FString WaypointLabel, TArray<AAIWaypoint*> Waypoints, bool bClearExistent = false);
	void AddPath(class UAIPath* Path);
	void AddRoute(class UAIRoute* Route);
	void AddRoute(class UAIRoute* Route, FString KeyName);
	bool HasCollection(FString CollectionLabel);
	void ClearCollection(FString CollectionLabel);
	void ClearNodes(bool bIsVip = false);
	void RemoveRoute(FString KeyName);
	TArray<ANodeWaypoint*> GetNodes(TArray<FString> NodeNames, TArray<ANodeWaypoint*>& Nodes);
	TArray<ANodeWaypoint*> GetNodes(TArray<FString> NodeNames, TArray<ANodeWaypoint*>& Nodes, int Min, int Max);
	TArray<AAIWaypoint*> ReverseWaypoints(TArray<AAIWaypoint*>& PathPoints);
	TArray<ANodeWaypoint*> ReverseWaypoints(TArray<ANodeWaypoint*>& Nodes);
	ANodeWaypoint* GetLastAvailableNode(TArray<ANodeWaypoint*>& LevelNodes, TArray<ANodeWaypoint*>& CheckedNodes, TArray<ANodeWaypoint*>& Nodes);

	TArray<ANodeWaypoint*> GetRoute(ANodeWaypoint* Origin, ANodeWaypoint* Destination, TArray<ANodeWaypoint*>& AvailableNodes);
	
	bool GetRoute_BFS(ANodeWaypoint* Destination, TArray<ANodeWaypoint*>& AvailableNodes, TMap<ANodeWaypoint*, TArray<ANodeWaypoint*>>& CheckedLevels, TArray<ANodeWaypoint*>& CheckedNodes, TArray<ANodeWaypoint*>& PathLinks, TQueue<TTuple<ANodeWaypoint*, ANodeWaypoint*>>& SearchQ);
private:

	void GetNodesPath_BFS(ANodeWaypoint* Origin, TArray<ANodeWaypoint*>& DestinationAndPrev, TMap<ANodeWaypoint*, TArray<ANodeWaypoint*>>& CheckedLevels);

	TArray<ANodeWaypoint*> PathNodes; 
	TArray<ANodeWaypoint*> VipNodes;  
	TMap<FString, TArray<AAIWaypoint*>> WaypointCollections; 
	TMap<FString, class UAIPath*> Paths;
	TMap<FString, class UAIRoute*> Routes;
	// Paths?
};
