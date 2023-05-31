// Fill out your copyright notice in the Description page of Project Settings.


#include "PublicEnemiesFactionZone.h"
#include "../../../Praise.h"
#include "../Navigation/AIWaypoint.h"
#include "../../../Characters/AI/BotCharacters/SeekerBotCharacter.h"
#include "../../../Characters/Player/PraisePlayerCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "../Navigation/WaypointHandler.h"
#include "../../../Components/Actor/AI/BotBrainComponent.h"
#include "../../../Components/GameMode/MapLocationsManagerComponent.h"

APublicEnemiesFactionZone::APublicEnemiesFactionZone()
{
	
}


void APublicEnemiesFactionZone::SetZoneWaypoints()
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

		TArray<ANodeWaypoint*> FactionNodes = GameFaction->GetMapLocationsManager()->GetFactionNodes(ECharFaction::PUBLIC_ENEMY);

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

			if(Node->ActorHasTag(FName("PublicBuilding")))
				WaypointHandler->AddNode(Node, true);
		}
	}
}

AAIWaypoint* APublicEnemiesFactionZone::GetZoneSpawnPoint()
{
	if (!WaypointHandler) return nullptr;

	if (WaypointHandler->GetPathNodes().Num() <= 0) return nullptr;

	if (ZoneName == FString("UWorld"))
	{
		TArray<ANodeWaypoint*> SpawnNodes;
		for (ANodeWaypoint* Node : WaypointHandler->GetPathNodes())
		{
			if (Node->GetWaypointType() == EWaypointType::PATH_END)
				SpawnNodes.Add(Node);

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
		if (!bHasZonePaths)
		{
			FString ZoneTag = ZoneName + FString("_SubZone");

			ANodeWaypoint* ZoneEntry = nullptr;

			if (WaypointHandler->FindNodeByLabel(ZoneTag, ZoneEntry, true))
			{
				TArray<AAIWaypoint*> ZoneWPs = WaypointHandler->GetWaypoints(ZoneTag);

				if (ZoneWPs.Num() <= 0) return ZoneEntry;

				return WaypointHandler->GetWaypoints(ZoneTag)[FMath::RandRange(0, ZoneWPs.Num() - 1)];
			}
		}
		
		else return WaypointHandler->GetPathNodes()[FMath::RandRange(0, WaypointHandler->GetPathNodes().Num() - 1)];
		
	}

	return nullptr;
}

void APublicEnemiesFactionZone::AddZoneBotKnownLocations(ABaseBotCharacter* SpawnedBot, FString Zone)
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

void APublicEnemiesFactionZone::HandleCharDeath(AActor* DeadChar, AActor* Killer)
{
	Super::HandleCharDeath(DeadChar, Killer);
}

