// Fill out your copyright notice in the Description page of Project Settings.


#include "CreaturesFactionZone.h"
#include "../Navigation/AIWaypoint.h"
#include "../../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../../Components/Actor/AI/BotBrainComponent.h"
#include "../../../Characters/AI/BotCharacters/SeekerBotCharacter.h"
#include "../Navigation/WaypointHandler.h"
#include "../../../Components/GameMode/MapLocationsManagerComponent.h"

ACreaturesFactionZone::ACreaturesFactionZone()
{
	InitBotsDelaySeconds = 10.f;
}


void ACreaturesFactionZone::SetZoneWaypoints()
{
	Super::SetZoneWaypoints();

	if (ZoneName == FString("UWorld"))
	{
		if (!GameFaction) return;

		if (!GameFaction->GetMapLocationsManager()) return;

		if (!WaypointHandler)
			WaypointHandler = NewObject<UWaypointHandler>(this);

		WaypointHandler = NewObject<UWaypointHandler>(this);

		if (!WaypointHandler) return;

		TArray<ANodeWaypoint*> FactionNodes = GameFaction->GetMapLocationsManager()->GetFactionNodes(ECharFaction::CREATURE);

		for (ANodeWaypoint* FactionNode : FactionNodes)
		{
			if (FactionNode->GetWaypointType() == EWaypointType::ZONE_ENTRY && FactionNode->ActorHasTag(*WP_MAIN_ZONE))
			{
				if (FactionNode->ActorHasTag(*WP_OUTDOOR))
					WaypointHandler->AddNode(FactionNode);

				WaypointHandler->AddNode(FactionNode, true);
			}
		}

		TArray<ANodeWaypoint*> CommonNodes = GameFaction->GetMapLocationsManager()->GetFactionNodes(ECharFaction::NONE); 

		for (ANodeWaypoint* Node : CommonNodes) {
			
			if (Node->GetZoneName() == FString("Common Path"))
				WaypointHandler->AddNode(Node);

			if (Node->GetWaypointType() == EWaypointType::ZONE_ENTRY && Node->ActorHasTag(*WP_MAIN_ZONE) && Node->ActorHasTag(*WP_OUTDOOR))
				WaypointHandler->AddNode(Node, true);
		}
	}
		
}

void ACreaturesFactionZone::AddZoneBotKnownLocations(ABaseBotCharacter* SpawnedBot, FString Zone)
{
	if (!WaypointHandler) return;

	Super::AddZoneBotKnownLocations(SpawnedBot, Zone);

	if (ZoneName == FString("UWorld"))
	{
		SpawnedBot->GetBrain()->AddKnownLocations(WaypointHandler->GetPathNodes());
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

void ACreaturesFactionZone::HandleCharDeath(AActor* DeadChar, AActor* Killer)
{
	Super::HandleCharDeath(DeadChar, Killer);
}

AAIWaypoint* ACreaturesFactionZone::GetZoneSpawnPoint()
{
	if (!WaypointHandler) return nullptr;


	if (ZoneName == FString("UWorld"))
	{
		TArray<ANodeWaypoint*> SpawnNodes;
		for (ANodeWaypoint* Node : WaypointHandler->GetPathNodes())
		{
			if (Node->FactionID() == ZoneFaction)
			{
				if (Node->GetWaypointType() == EWaypointType::ZONE_ENTRY && Node->ActorHasTag(*WP_MAIN_ZONE))
					SpawnNodes.Add(Node);
			}
		}

		return SpawnNodes.Num() == 0 ? nullptr : SpawnNodes[FMath::RandRange(0, SpawnNodes.Num() - 1)];
	}
	else
	{
		if (WaypointHandler->GetVipNodes().Num() <= 0) return nullptr;

		FString ZoneTag = ZoneName + FString("_SubZone");

		ANodeWaypoint* ZoneEntry = nullptr;

		if (WaypointHandler->FindNodeByLabel(ZoneTag, ZoneEntry, true))
		{
			TArray<AAIWaypoint*> ZoneWPs = WaypointHandler->GetWaypoints(ZoneTag);

			if (ZoneWPs.Num() <= 0) return ZoneEntry;

			return WaypointHandler->GetWaypoints(ZoneTag)[FMath::RandRange(0, ZoneWPs.Num() - 1)];
		}
	}

	return nullptr;
}

