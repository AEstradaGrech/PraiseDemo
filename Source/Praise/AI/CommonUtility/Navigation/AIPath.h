// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AIWaypoint.h"
#include "NodeWaypoint.h"
#include "../../../Enums/Characters/ECharFaction.h"
#include "AIPath.generated.h"

/**
 * Arista
 */
UCLASS()
class PRAISE_API UAIPath : public UObject
{
	GENERATED_BODY()
	
public:
	UAIPath();

	FORCEINLINE FString GetPathName() const { return PathName; }
	FORCEINLINE bool IsValid() const { return NodeA && NodeB && PathWaypoints.Num() > 0; }
	FORCEINLINE bool IsBuildingLink() const { return bIsBuildingLink; }
	FORCEINLINE bool DidSetupPathPoints() const { return bDidSetupPathPoints; }
	FORCEINLINE ANodeWaypoint* GetNodeA() const { return NodeA; }
	FORCEINLINE ANodeWaypoint* GetNodeB() const { return NodeB; }
	FORCEINLINE TArray<AAIWaypoint*> GetPathPoints() const { return PathWaypoints; }
	FORCEINLINE ECharFaction GetPathFaction() const { return Faction; }
	FORCEINLINE FString GetPathZone() const { return ZoneName; }

	void Setup(ANodeWaypoint* A, ANodeWaypoint* B, TArray<AAIWaypoint*> PathPoints, ECharFaction Faction, FString PathZoneName, bool bIsLink = false);
	void AddPathPoint(AAIWaypoint* Point);

	AAIWaypoint* GetNextPathPoint();
	AAIWaypoint* GetCurrentPathPoint() const;
	void RemoveCurrentPathPoint();
	void SetupPathPoints(bool bDrawPath = false);
	void SetPathDirection(ANodeWaypoint* PathEndNode);
	void DebugPath();
private:
	FString PathName;
	float TotalDistance;
	bool bIsBuildingLink;
	bool bDidSetupPathPoints;
	ECharFaction Faction;
	FString ZoneName;
	ANodeWaypoint* NodeA;
	ANodeWaypoint* NodeB;
	TArray<AAIWaypoint*> PathWaypoints;

	TArray<AAIWaypoint*> SetPathPoints(AAIWaypoint* Node, TMap<AAIWaypoint*, float>& SortedPoints, bool bDrawPath = false);
};
