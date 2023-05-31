// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingEntryNode.h"
#include "../../../Environment/Buildings/BaseBuilding.h"

ABuildingEntryNode::ABuildingEntryNode() : Super()
{
	bIsMainDoor = false;
	bIsZoneEntry = false;
}

void ABuildingEntryNode::SetupBuildingWaypoint()
{
	ABaseBuilding* ParentBuilding = nullptr;

	if (TryAddBuildingWaypoint(ParentBuilding) && ParentBuilding)
	{
		NodeBuilding = ParentBuilding;
		Faction = ParentBuilding->FactionID();
		ZoneName = ParentBuilding->GetBuildingZoneName();
		WaypointLabel = ParentBuilding->GetBuildingName();
		WaypointLabel.Append("_Building_SubZone");

		if (Tags.Contains(FName("MainDoorNode")))
			bIsMainDoor = true;

		if (Tags.Contains(FName("SubZoneEntry")))
			bIsZoneEntry = true;

		if (!Tags.Contains(FName("BuildingEntryNode")))
			Tags.Add(FName("BuildingEntryNode"));

		bIsPublicZone = ParentBuilding->AllowNonOwnersIn();

		if(!bIsPublicZone)
			Tags.Add(FName("PrivateZone"));
	}
}