// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AIRoute.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UAIRoute : public UObject
{
	GENERATED_BODY()
	
public:
	UAIRoute();

	FORCEINLINE FString GetRouteName() const { return RouteName; }
	FORCEINLINE class ANodeWaypoint* GetOrigin() const { return OriginNode; }
	FORCEINLINE class ANodeWaypoint*  GetDestination() const { return DestinationNode; }
	FORCEINLINE TArray<class ANodeWaypoint*> GetPathNodes() const { return PathNodes; }
	FORCEINLINE TMap<FString, class UAIPath*> GetPathsMap() const { return RoutePaths; }
	FORCEINLINE class UAIPath* GetCurrentPath() const { return CurrentPath; }
	FORCEINLINE class AAIWaypoint* GetCurrentPathPoint() const { return CurrentPathPoint; }
	bool IsValid() const;
	bool IsFinished() const;
	FORCEINLINE bool IsPaused() const { return bIsPaused; }
	TArray<class UAIPath*> GetAllPaths() const;

	void Setup(class ANodeWaypoint* Origin, class ANodeWaypoint* Destination, TArray<ANodeWaypoint*> Nodes, TArray<class UAIPath*> Paths);
	class UAIPath* GetPath(FString PathName) const;
	class UAIPath* GetNextPath();
	void OnPathPointReached(class AAIWaypoint* Point);
	void OnPathNodeReached(class ANodeWaypoint* ReachedNode); 
	void UpdateCurrentPathPoint(); 
	void PauseRoute(bool bPause);
	void PreviewRoute();
private:

	UPROPERTY()
	FString RouteName;
	UPROPERTY()
	class ANodeWaypoint* OriginNode;
	UPROPERTY()
	class ANodeWaypoint* DestinationNode;
	UPROPERTY()
	TArray<class ANodeWaypoint*> PathNodes;
	UPROPERTY()
	TMap<FString, class UAIPath*> RoutePaths;
	UPROPERTY()
	TArray<class ANodeWaypoint*> CurrentPathNodes;
	UPROPERTY()
	class AAIWaypoint* CurrentPathPoint;
	UPROPERTY()
	class UAIPath* CurrentPath;

	bool bIsPaused;
	UPROPERTY()
		TArray<class AAIWaypoint*> SavedPathPoints;

	TQueue<class AAIWaypoint*> PathPointsQ;
	class UAIPath* FindRoutePath(class ANodeWaypoint* NodeA, class ANodeWaypoint* NodeB);
};
