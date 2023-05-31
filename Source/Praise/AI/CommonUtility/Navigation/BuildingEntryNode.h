// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NodeWaypoint.h"
#include "BuildingEntryNode.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API ABuildingEntryNode : public ANodeWaypoint
{
	GENERATED_BODY()

public:
	ABuildingEntryNode();

	FORCEINLINE class ABaseBuilding* GetNodeBuilding() const { return NodeBuilding; }
	FORCEINLINE bool IsMainDoor() const { return bIsMainDoor; }
	FORCEINLINE bool IsZoneEntry() const { return bIsZoneEntry; }
	FORCEINLINE bool IsPublicZone() const { return bIsPublicZone; }
private:
	virtual void SetupBuildingWaypoint() override;

	class ABaseBuilding* NodeBuilding;
	bool bIsMainDoor;
	bool bIsZoneEntry;
	bool bIsPublicZone;
};
