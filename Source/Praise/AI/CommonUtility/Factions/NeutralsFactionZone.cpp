// Fill out your copyright notice in the Description page of Project Settings.


#include "NeutralsFactionZone.h"
#include "../../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../../Components/Actor/AI/BotBrainComponent.h"
#include "../../CommonUtility/Navigation/WaypointHandler.h"


void ANeutralsFactionZone::SetZoneWaypoints()
{
	Super::SetZoneWaypoints();

	if (ZoneName == FString("Common Path"))
	{
		if (!GameFaction) return;

		if (!GameFaction->GetMapLocationsManager()) return;

		if(!WaypointHandler)
			WaypointHandler = NewObject<UWaypointHandler>(this);

		if (!WaypointHandler) return;

		TArray<ANodeWaypoint*> NodesAndEntries = GameFaction->GetPathNodes();

		for (ANodeWaypoint* Node : NodesAndEntries) {
			WaypointHandler->AddNode(Node);
			if (Node->GetWaypointType() == EWaypointType::ZONE_ENTRY) {
				WaypointHandler->AddNode(Node, true);
			}
		}
	}
}

void ANeutralsFactionZone::AddZoneBotKnownLocations(ABaseBotCharacter* SpawnedBot, FString Zone)
{
	if (!WaypointHandler) return;

	Super::AddZoneBotKnownLocations(SpawnedBot, Zone);

	if (ZoneName == FString("Common Path") && SpawnedBot->GetCharSpawnZone()->GetZoneName() == ZoneName)
		SpawnedBot->GetBrain()->AddKnownLocations(WaypointHandler->GetPathNodes()); 

	if (ZoneName == FString("UWorld"))
	{

	}
	else
	{
		FString ZoneTag = ZoneName + FString("_SubZone");

		ANodeWaypoint* ZoneEntry = nullptr;

		if (WaypointHandler->FindNodeByLabel(ZoneTag, ZoneEntry, true))
		{
			SpawnedBot->GetBrain()->AddKnownLocation(ZoneEntry);

			SpawnedBot->SetOnlySubZonePatrols(true);
			SpawnedBot->GetBrain()->GetBotBB()->SetValueAsBool(BB_SUBZONE_PATROL, true);
		}
	}
}
