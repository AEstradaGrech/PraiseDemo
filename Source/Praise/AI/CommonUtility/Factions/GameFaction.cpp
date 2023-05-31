// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFaction.h"
#include "../../../Structs/CommonUtility/FLogger.h"
#include "../../../Praise.h"
#include "../../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../../Components/GameMode/BotSpawnerComponent.h"
#include "../../../Components/GameMode/MsgCommandsFactoryComponent.h"
#include "../../../Components/GameMode/MapLocationsManagerComponent.h"
#include "../../../Components/GameMode/FactionsManagerComponent.h"
#include "../../../Components/Actor/MsgDispatcherComponent.h"
#include "../Factions/NeutralsFactionZone.h"
#include "../Factions/CreaturesFactionZone.h"
#include "../Factions/PublicEnemiesFactionZone.h"
#include "../Navigation/NodeWaypoint.h"
#include "../Navigation/AIPath.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/DataTable.h"

AGameFaction::AGameFaction()
{
	FactionMsgDispatcher = CreateDefaultSubobject<UMsgDispatcherComponent>(TEXT("Faction Msg Dispatcher"));

	WaypointHandler = CreateDefaultSubobject<UWaypointHandler>(TEXT("Faction Waypoint Handler"));

	static const ConstructorHelpers::FObjectFinder<UDataTable> ZonesDTFile(TEXT("DataTable'/Game/Core/DataTables/Factions/DT_FactionZones.DT_FactionZones'"));

	if (ZonesDTFile.Succeeded())
		FactionZonesDT = ZonesDTFile.Object;

	EnemiesRecallCheckSeconds = 30 * 1.f;

	CacheBotBPs();
}

void AGameFaction::CacheBotBPs()
{
	ConstructorHelpers::FClassFinder<ABaseBotCharacter> CitizenCharClass(TEXT("/Game/Core/Blueprints/Characters/Bots/BP_CitizenBotCharacter"));

	if (CitizenCharClass.Succeeded())
		BotBPs.Add(BP_CITIZEN_BOT, CitizenCharClass.Class);

	ConstructorHelpers::FClassFinder<ABaseBotCharacter> SeekerCharClass(TEXT("/Game/Core/Blueprints/Characters/Bots/BP_SeekerDemonBotCharacter"));

	if (SeekerCharClass.Succeeded())
		BotBPs.Add(BP_SEEKER_BOT, SeekerCharClass.Class);

	ConstructorHelpers::FClassFinder<ABaseBotCharacter> GuardCharClass(TEXT("/Game/Core/Blueprints/Characters/Bots/BP_GuardBotCharacter"));

	if (GuardCharClass.Succeeded())
		BotBPs.Add(BP_GUARD_BOT, GuardCharClass.Class);
}

void AGameFaction::CacheFactionLocations()
{
	if (!LocationsManager) return;

	if (!WaypointHandler)
		WaypointHandler = NewObject<UWaypointHandler>(this, FName("Faction WaypointHandler"));

	TArray<ANodeWaypoint*> FactionNodes = LocationsManager->GetFactionNodes(Faction);

	for (ANodeWaypoint* Node : FactionNodes)
	{
		if(Node->GetWaypointType() == EWaypointType::ZONE_ENTRY && Node->ActorHasTag(*WP_MAIN_ZONE))
			WaypointHandler->AddNode(Node); 

		if (Node->ActorHasTag(*WP_VIP)) 
			WaypointHandler->AddNode(Node, true);

		WaypointHandler->AddWaypoint(Node->GetZoneName(), Node); 
	}}

void AGameFaction::CacheCommonPathNodes()
{
	if (!LocationsManager) return;

	if (!WaypointHandler)
		WaypointHandler = NewObject<UWaypointHandler>(this, FName("Faction Waypoint Handler"));

	TArray<ANodeWaypoint*> CommonNodes = LocationsManager->GetFactionNodes(ECharFaction::NONE);

	for (ANodeWaypoint* Node : CommonNodes)
		if (Node->GetWaypointType() != EWaypointType::PATH_POINT)
			WaypointHandler->AddNode(Node);
}

void AGameFaction::CacheZoneVIPs(AFactionZone* Zone)
{
	TArray<ANodeWaypoint*> ZoneVIPs = Zone->GetZoneNodes(true);

	for (ANodeWaypoint* Node : ZoneVIPs)
		if (Node->ActorHasTag(*WP_VIP))
			WaypointHandler->AddNode(Node, true);
}


void AGameFaction::OnKnownEnemiesRecallCheck()
{
	if (!ensure(GetWorld() != nullptr)) return;

	if (!GetWorld()->GetGameState()) return;

 	float Timestamp = GetWorld()->GetGameState()->GetGameTimeSinceCreation();

	TArray<AActor*> Keys;
	KnownEnemies.GetKeys(Keys);
	
	for (AActor* Enemy : Keys)
	{
		if (Enemy && Timestamp - KnownEnemies[Enemy] >= KnownEnemiesResetSeconds) {
			KnownEnemies.Remove(Enemy);
		}
	}
}

UWaypointHandler* AGameFaction::GetWaypointHandler() const
{
	return WaypointHandler;
}

bool AGameFaction::InitFaction(FGameFactionDTStruct* FactionConfig, UBotSpawnerComponent* Spawner, UMapLocationsManagerComponent* LocsManager, UFactionsManagerComponent* FactionsMan, UMsgCommandsFactoryComponent* MsgsFactory)
{
	if (!FactionZonesDT) return false;

	if (FactionConfig->FactionZones.Num() <= 0) return false;

	BotSpawner = Spawner;
	LocationsManager = LocsManager;
	FactionsManager = FactionsMan;
	MsgCommandsFactory = MsgsFactory;

	Faction = FactionConfig->Faction;
	FactionName = FactionConfig->FactionName;
	KnownEnemiesResetSeconds = FactionConfig->KnownEnemiesResetSeconds;
	
	for (int i = 0; i < FactionConfig->EnemyFactions.Num(); i++)
		EnemyFactions.Add((ECharFaction)FactionConfig->EnemyFactions[i]);
	for (int i = 0; i < FactionConfig->FriendlyFactions.Num(); i++)
		FriendlyFactions.Add((ECharFaction)FactionConfig->FriendlyFactions[i]);

	CacheFactionLocations();

	CacheCommonPathNodes();

	if(GetWorld())
		GetWorld()->GetTimerManager().SetTimer(KnownEnemiesRecallTimer, this, &AGameFaction::OnKnownEnemiesRecallCheck, EnemiesRecallCheckSeconds, true);

	switch (Faction)
	{
		case ECharFaction::NONE:
			return InitFactionZones<ANeutralsFactionZone>(FactionConfig);
		case ECharFaction::CREATURE:
			return InitFactionZones<ACreaturesFactionZone>(FactionConfig);
		case ECharFaction::PUBLIC_ENEMY:
			return InitFactionZones<APublicEnemiesFactionZone>(FactionConfig);
		case ECharFaction::CREED:
		case ECharFaction::CZORT:
		case ECharFaction::PAGAN:
		case ECharFaction::BAAL:
		case ECharFaction::PROTECTOR:
		case ECharFaction::BOTVILLE:
		default:
			return InitFactionZones<AFactionZone>(FactionConfig);
	}

	return InitFactionZones<AFactionZone>(FactionConfig);

}


bool AGameFaction::TryGetConnectedPathNodes(ANodeWaypoint* Origin, TArray<ANodeWaypoint*>& PathNodes, TArray<ANodeWaypoint*>& CheckedNodes, bool bIncludeCommonNodes)
{
	if (!FactionsManager) return false;

	TArray<ANodeWaypoint*> Nodes;

	if (Faction != ECharFaction::NONE)
		Nodes.Append(FactionsManager->GetFactionNodes(ECharFaction::NONE));

	Nodes.Append(GetPathNodes());

	return TryGetConnectedPathNodes(Origin, PathNodes, Nodes, CheckedNodes, bIncludeCommonNodes);
}

bool AGameFaction::TryGetClosestNode(FVector Origin, ANodeWaypoint*& OutNode, bool bCheckAll)
{
	TArray<ANodeWaypoint*> AllNodes;
	ANodeWaypoint* ClosestNode = nullptr;
	switch (bCheckAll)
	{
		case(true):
			
			AllNodes = LocationsManager->GetAllNodes();

			ClosestNode = WaypointHandler->GetClosestNode(Origin, AllNodes);

			if (!ClosestNode) return false;

			OutNode = ClosestNode;

			return true;

		case(false):

			ClosestNode = WaypointHandler->GetClosestNode(Origin);

			if (!ClosestNode) return false;

			OutNode = ClosestNode;

			return true;
	}

	return false;
}

bool AGameFaction::TryGetClosestCommonNode(FVector Origin, ANodeWaypoint*& OutNode)
{
	if (!LocationsManager) return false;

	ANodeWaypoint* Node = WaypointHandler->GetClosestNode(Origin, LocationsManager->GetFactionNodes(ECharFaction::NONE));

	if (Node)
	{
		OutNode = Node;
		return true;
	}

	return false;
}

bool AGameFaction::TryGetConnectedPathNodes(ANodeWaypoint* Origin, TArray<ANodeWaypoint*>& PathNodes, TArray<ANodeWaypoint*>& Nodes, TArray<ANodeWaypoint*>& CheckedNodes, bool bIncludeCommonNodes)
{
	if (!CheckedNodes.Contains(Origin))
		CheckedNodes.Add(Origin);

	if (Origin->GetAdjacentNodes().Num() == 0)
	{
		if (Origin->FactionID() != Faction)
		{
			if (bIncludeCommonNodes && Faction != ECharFaction::NONE)
			{
				if (Origin->FactionID() != ECharFaction::NONE && !Origin->ActorHasTag("MainZone"))
					return false;
			}
		}

		if (!PathNodes.Contains(Origin))
			PathNodes.Add(Origin);

		return true;
	}

	TArray<ANodeWaypoint*> FactionNodes;
	for (ANodeWaypoint* Adjacent : WaypointHandler->GetNodes(Origin->GetAdjacentNodes(), Nodes))
	{
		if (CheckedNodes.Contains(Adjacent)) continue;

		if (Adjacent->FactionID() == Faction)
			FactionNodes.Add(Adjacent);

		if (Faction == ECharFaction::NONE ||(bIncludeCommonNodes && Faction != ECharFaction::NONE)) 
		{
			if(Adjacent->FactionID() == ECharFaction::NONE)
				FactionNodes.Add(Adjacent);

			if (Adjacent->GetWaypointType() == EWaypointType::ZONE_ENTRY) 
			{
				for (FString AdjacentTag : Adjacent->GetAdjacentNodes())
				{
					if (AdjacentTag.Contains("Common Path"))
					{
						FactionNodes.Add(Adjacent);
						break;
					}
				}
			}
		}

		CheckedNodes.Add(Adjacent);
		PathNodes.Add(Adjacent);
	}

	if (FactionNodes.Num() == 0) return true;

	for (ANodeWaypoint* Node : FactionNodes)
	{
		TryGetConnectedPathNodes(Node, PathNodes, Nodes, CheckedNodes, bIncludeCommonNodes);
	}
		

	return true;
}

void AGameFaction::HandleNewFoE(AActor* FoE, bool bIsEnemy)
{
	float Timestamp = GetWorld()->GetGameState()->GetGameTimeSinceCreation();

	switch (bIsEnemy)
	{
		case(true):
			if (!KnownEnemies.Contains(FoE))
				KnownEnemies.Add(FoE, Timestamp);

			else KnownEnemies[FoE] = Timestamp;

			break;
		case(false):
			if (!KnownAllies.Contains(FoE))
				KnownAllies.Add(FoE, Timestamp);

			else KnownAllies[FoE] = Timestamp; 

			break;
	}
}

TArray<ANodeWaypoint*> AGameFaction::GetPathNodes() const
{
	if(!WaypointHandler) return TArray<ANodeWaypoint*>();
	
	return WaypointHandler->GetPathNodes();
}

TArray<ANodeWaypoint*> AGameFaction::GetLocations() const
{
	if (!WaypointHandler) return TArray<ANodeWaypoint*>();

	return WaypointHandler->GetVipNodes();
}

TArray<ANodeWaypoint*> AGameFaction::GetZoneNodes(FString ZoneName) const
{
	TArray<ANodeWaypoint*>Nodes;
	
	if (!WaypointHandler && !LocationsManager) return Nodes;

	TArray<AAIWaypoint*> ZoneWaypoints = WaypointHandler->GetWaypoints(ZoneName);
		
	if (ZoneWaypoints.Num() == 0)
	{
		TArray<ANodeWaypoint*> NodesCol = LocationsManager->GetFactionZoneNodes(Faction, ZoneName);

		for (ANodeWaypoint* Node : NodesCol)
		{
			ZoneWaypoints.Add(Node);
			WaypointHandler->AddWaypoint(ZoneName, Node);
		}
	}
	else
	{
		for (AAIWaypoint* Node : ZoneWaypoints)
			if (Node->IsA<ANodeWaypoint>())
				if (Cast<ANodeWaypoint>(Node)->GetZoneName() == ZoneName)
					Nodes.Add(Cast<ANodeWaypoint>(Node));
	}
		
	return Nodes;
}

TArray<ANodeWaypoint*> AGameFaction::GetZoneNodes(ECharFaction ZoneFaction, FString ZoneName) const
{	
	if (ZoneFaction != Faction)
	{
		if (!LocationsManager) return TArray<ANodeWaypoint*>();

		return LocationsManager->GetFactionZoneNodes(ZoneFaction, ZoneName);
	}

	else return GetZoneNodes(ZoneName);
	
	return TArray<ANodeWaypoint*>();
}

void AGameFaction::AddZonePaths(AFactionZone* Zone)
{
	if (!LocationsManager) return;

	TArray<UAIPath*> FactionPaths = LocationsManager->GetFactionPaths(Faction);

	for (UAIPath* Path : FactionPaths)
		if (Path->GetPathZone() == Zone->GetZoneName() && Path->IsValid())
			Zone->GetWaypointHandler()->AddPath(Path);
}

TArray<UAIPath*> AGameFaction::GetCommonPaths()
{
	TArray<UAIPath*> Paths;

	if (!LocationsManager) return Paths;

	return LocationsManager->GetFactionPaths(Faction);
}

TArray<UAIPath*> AGameFaction::GetZonePaths(FString ZoneName)
{
	TArray<UAIPath*> Paths;
	if (ZoneName.IsEmpty())
	{
		for (AFactionZone* Zone : FactionZones)
			for (UAIPath* Path : Zone->GetWaypointHandler()->GetAllPaths())
				Paths.Add(Path);
	}
	else
	{
		for (AFactionZone* Zone : FactionZones)
			if (Zone->GetZoneName() == ZoneName)
				for (UAIPath* Path : Zone->GetWaypointHandler()->GetAllPaths())
					Paths.Add(Path);
	}

	return Paths;
}

TArray<UAIPath*> AGameFaction::GetRoutePaths(TArray<ANodeWaypoint*>& RouteNodes)
{
	TArray<UAIPath*> RoutePaths;

	if (!LocationsManager) return RoutePaths;

	if (RouteNodes.Num() < 2) return RoutePaths;

	ANodeWaypoint* NodeA = nullptr;
	ANodeWaypoint* NodeB = nullptr;
	UAIPath* Path = nullptr;
	for (int i = 0; i < RouteNodes.Num() -1; i++)
	{
		NodeA = RouteNodes[i];
		NodeB = RouteNodes[i + 1];
		
		if (NodeA->IsValid() && NodeB->IsValid())
		{
			Path = LocationsManager->GetPath(NodeA, NodeB);

			if (!Path) return TArray<UAIPath*>();

			if (!Path->DidSetupPathPoints())
				Path->SetupPathPoints();
	
			Path->SetPathDirection(NodeB);

			RoutePaths.Add(Path);
		}

		else return TArray<UAIPath*>();
	}

	return RoutePaths;
}

