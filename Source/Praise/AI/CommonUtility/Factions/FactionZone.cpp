// Fill out your copyright notice in the Description page of Project Settings.


#include "FactionZone.h"
#include "Components/BoxComponent.h"
#include "../../../Structs/CommonUtility/FLogger.h"
#include "../../../Interfaces/CombatSystem/Targeteable.h"
#include "../../../Components/GameMode/BotSpawnerComponent.h"
#include "../../../Components/GameMode/MapLocationsManagerComponent.h"
#include "../../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../../../Interfaces/AI/Factioneable.h"
#include "../Navigation/WaypointHandler.h"
#include "../../../Environment/Buildings/BaseBuilding.h"
#include "../Navigation/AIPath.h"
#include "../Navigation/BuildingEntryNode.h"
#include "../../../Components/Actor/AI/BotBrainComponent.h"
#include "../../../Characters/AI/BotCharacters/BaseBotCharacter.h"

AFactionZone::AFactionZone()
{
	InitBotsDelaySeconds = 5.f;

	ZoneMsgDispatcher = CreateDefaultSubobject<UMsgDispatcherComponent>(TEXT("Zone Msg Dispatcher"));
}

bool AFactionZone::InitZone(FFactionZoneDTStruct* ZoneConfig, IGameFactionInterface* FactionInterface)
{
	GameFaction = FactionInterface;
	if (!ensure(GameFaction->GetBotSpawner()->GetWorld() != nullptr)) return false;
	
	if (!(GameFaction->GetBotSpawner() && GameFaction->GetMapLocationsManager())) return false;

	

	ZoneFaction = ZoneConfig->ZoneFaction;
	FactionName = ZoneConfig->FactionName;
	ZoneName = ZoneConfig->ZoneName;
	InitBotsDelaySeconds = ZoneConfig->InitBotsDelaySeconds;
	ZoneBots = ZoneConfig->ZoneBots;
	NeutralHitsTolerance = ZoneConfig->NeutralHitsTolerance;
	BotRespawnSeconds = ZoneConfig->BotRespawnSeconds;
	bBuildingBotsEnabled = ZoneConfig->bBuildingBotsEnabled;
	bHasZonePaths = ZoneConfig->bHasZonePaths;
	NavMesh = UNavigationSystemV1::GetCurrent(GameFaction->GetBotSpawner()->GetWorld());

	if (!NavMesh) return false;

	if (InitBotsDelaySeconds <= 0.f)
		InitBotsDelaySeconds = 10.f;

	SetZoneWaypoints();

	

	if (ZoneConfig->bHasZoneCollider)
	{
		if (!WaypointHandler->HasCollection(WP_ZONE_COL_LOC)) return false;
		
		ANodeWaypoint* ZoneColliderLoc = Cast<ANodeWaypoint>(WaypointHandler->GetWaypoints(WP_ZONE_COL_LOC)[0]);

		if (ZoneColliderLoc && !InitZoneCollider(ZoneColliderLoc->GetActorLocation(), ZoneConfig->ZoneBoxExtents))
			return false;
	}
		
	GameFaction->GetBotSpawner()->GetWorld()->GetTimerManager().SetTimer(BotSpawnTimerHandle, this, &AFactionZone::InitBots, InitBotsDelaySeconds, false);

	return true;
}

TArray<AAIWaypoint*> AFactionZone::GetSubZoneWaypoints(FString Label) const
{
	return WaypointHandler ? WaypointHandler->GetWaypoints(Label) : TArray<AAIWaypoint*>();
}

TArray<ANodeWaypoint*> AFactionZone::GetZoneNodes(bool bOnlyVipNodes, FName Tag, bool bExcludeTagged)
{
	TArray<ANodeWaypoint*> Nodes = bOnlyVipNodes ? WaypointHandler->GetVipNodes() : WaypointHandler->GetPathNodes();
	TArray<ANodeWaypoint*> FilteredNodes;
	if (Tag.IsNone())
		return Nodes;

	for (ANodeWaypoint* Node : Nodes) {
		if (Node->ActorHasTag(Tag))
		{
			if (!bExcludeTagged)
				FilteredNodes.Add(Node);
		}
		else
		{
			if (bExcludeTagged)
				FilteredNodes.Add(Node);
		}
	}
	return FilteredNodes;
}

ANodeWaypoint* AFactionZone::GetBuildingEntryNode(FString BuildingEditorName, FString EntryTag)
{
	for (ABaseBuilding* Building : ZoneBuildings)
	{
		if (Building->GetBuildingName() == BuildingEditorName)
		{
			for (ANodeWaypoint* Entry : Building->GetBuildingEntries())
			{
				if (Entry->ActorHasTag(*EntryTag))
					return Entry;
			}
		}
	}

	return nullptr;
}

void AFactionZone::BeginPlay()
{
	Super::BeginPlay();

}

void AFactionZone::InitBots()
{
	if (!GameFaction) return;

	if (!(GameFaction->GetBotSpawner() && GameFaction->GetMapLocationsManager())) return;

	TArray<FString> Keys;
	ZoneBots.GetKeys(Keys);

	for (FString Key : Keys)
	{
		if (ZoneBots[Key] <= 0) continue;

		if (SpawnedBots.Contains(Key))
		{
			if (SpawnedBots[Key].Num() >= ZoneBots[Key]) continue;

			int Remaining = ZoneBots[Key] - SpawnedBots[Key].Num();

			if (Remaining <= 0) continue;

			SpawnRemainingBotType(Key, Remaining);
			
		}

		else SpawnRemainingBotType(Key, ZoneBots[Key]);
	}
	
	if (bBuildingBotsEnabled)
		SpawnBuildingBots();
}

void AFactionZone::SetZoneWaypoints()
{
	if (!GameFaction) return;
	
	if (!GameFaction->GetMapLocationsManager()) return;

	TArray<ANodeWaypoint*> ZoneNodes = GameFaction->GetZoneNodes(ZoneName);

	WaypointHandler = NewObject<UWaypointHandler>(this);

	for (ANodeWaypoint* Node : ZoneNodes)
	{
		if (Node->GetWaypointLabel() == WP_CROSSROAD)
			WaypointHandler->AddNode(Node, true);

		if (Node->ActorHasTag(*WP_OUTDOOR))
			WaypointHandler->AddNode(Node);

		if (Node->ActorHasTag(*WP_VIP)) 
			WaypointHandler->AddNode(Node, true);

		if (Node->ActorHasTag(*WP_ZONE_COL_LOC))
			WaypointHandler->AddWaypoint(WP_ZONE_COL_LOC, Node);

		if (Node->GetWaypointType() == EWaypointType::ZONE_ENTRY)
		{
			WaypointHandler->AddNode(Node, true);

			if (Node->ActorHasTag(*WP_SUB_ZONE) && !Node->IsA<ABuildingEntryNode>()) 
			{
				TArray<AAIWaypoint*> SubZoneWps = GameFaction->GetMapLocationsManager()->GetCommonWaypointsByTag(Node->GetWaypointLabel());

				WaypointHandler->AddWaypoints(Node->GetWaypointLabel(), SubZoneWps);
			}
		}
	}

	TArray<AAIWaypoint*> BuildingLinks = GameFaction->GetMapLocationsManager()->GetZoneBuildingLinks(ZoneName);

	if (BuildingLinks.Num() > 0)
		WaypointHandler->AddWaypoints(FString("BuildingLinks"), BuildingLinks);

	SetZoneBuildingWaypoints();
}

void AFactionZone::SetZoneBuildingWaypoints()
{
	TArray<AActor*> Buildings;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ABaseBuilding::StaticClass(), *ZoneName, Buildings);

	for (AActor* Result : Buildings)
	{
		ABaseBuilding* Building = Cast<ABaseBuilding>(Result);

		if (Building->GetBuildingName().IsEmpty()) continue;

		if(!ZoneBuildings.Contains(Building))
			ZoneBuildings.Add(Building);
		
		TArray<ANodeWaypoint*> BuildingEntries = Building->GetBuildingEntries();

		if (BuildingEntries.Num() == 0) continue;

		for (ANodeWaypoint* WP : BuildingEntries)
		{
			if (WaypointHandler->GetPathNodes().Contains(WP)) continue;

			WP->SetNodeID(WaypointHandler->GetPathNodes().Num());

			WaypointHandler->AddNode(WP);
		
			if (!Building->IsPrivate())
				WaypointHandler->AddNode(WP, true);

			if (GameFaction)
				GameFaction->GetWaypointHandler()->AddWaypoint(ZoneName, WP);
		}

		WaypointHandler->AddWaypoints(Building->GetBuildingName(), Building->GetBuildingWaypoints());

		SetupBuildingPathLinks(Building);
	}

	if (WaypointHandler)
	{
		for (UAIPath* BuildingLinkPath : WaypointHandler->GetAllPaths())
			BuildingLinkPath->SetupPathPoints();
	}
}

void AFactionZone::SetupBuildingPathLinks(ABaseBuilding* ZoneBuilding)
{
	if (ZoneBuilding->GetBuildingName().IsEmpty()) return;

	TArray<AAIWaypoint*> ZoneBuildingLinks = WaypointHandler->GetWaypoints(FString("BuildingLinks"));

	if (ZoneBuildingLinks.Num() == 0) return;
	
	TArray<FString> LinkTags;
	ZoneBuilding->GetZoneLinkNodes().GetKeys(LinkTags);
	
	ANodeWaypoint* Link = nullptr;
	
	for (FString LinkTag : LinkTags)
	{
		TArray<AAIWaypoint*> Links;
		if (WaypointHandler->FindNode(LinkTag, Link, false))
		{
			FString LinkName = Link->GetNodeName().Append("-").Append(ZoneBuilding->GetBuildingName());

			FName Tag = *(FString("BuildingLink=").Append(LinkName));

			for (AAIWaypoint* WP : ZoneBuildingLinks)
				if (WP->GetWaypointTags().Contains(Tag.ToString()))
					Links.Add(WP);

			UAIPath* BuildingLinkPath = NewObject<UAIPath>(GameFaction->GetMapLocationsManager(), *LinkName);

			ANodeWaypoint* BuildingNode = nullptr;

			for(ANodeWaypoint* BuildingWP : ZoneBuilding->GetBuildingEntries())
				if(BuildingWP->GetWaypointTags().Contains(ZoneBuilding->GetZoneLinkNodes()[LinkTag].ToString()))
					BuildingNode = BuildingWP;

			if (Link && BuildingNode) {
				Link->AddAdjacentNodeTag(BuildingNode->GetNodeName());
				BuildingNode->AddAdjacentNodeTag(Link->GetNodeName());
				BuildingLinkPath->Setup(Link, BuildingNode, Links, ZoneFaction, LinkName, true);
				WaypointHandler->AddPath(BuildingLinkPath);
			}
		}
	}
}

void AFactionZone::SpawnBuildingBots()
{
	if (ZoneBuildings.Num() <= 0) return;

	TArray<ABaseBotCharacter*> BuildingBots;
	for (int i = 0; i < ZoneBuildings.Num(); i++)
		if(ZoneBuildings[i]->ShouldSpawnBots())
			ZoneBuildings[i]->TrySpawnBuildingBots(GameFaction->GetBotSpawner(), Cast<AGameFaction>(GameFaction), this, BuildingBots);

	ProcessSpawnedBuildingBots(BuildingBots);
}

void AFactionZone::SetupSpawnedBot(ABaseBotCharacter* NewBot)
{
	if (!NewBot->DidBrainInit())
		NewBot->TryInitBrain();

	if (!NewBot->DidBrainInit()) return;

	NewBot->OnNotifyDead.AddDynamic(this, &AFactionZone::HandleCharDeath);

	if (NewBot->GetCharFaction())
		NewBot->OnNewFoE.BindUFunction(NewBot->GetCharFaction(), FName("HandleNewFoE"));

	if (NewBot->GetCharSpawnZone()) 
	{
		if (NewBot->GetBotBrain<UStateMachineBrainComponent>())
		{
			NewBot->GetBotBrain<UStateMachineBrainComponent>()->SetStatesFactory(GameFaction->GetBotSpawner()->GetMachineStatesFactory());

			TArray<ANodeWaypoint*> SpawnNodes = GetZoneNodes(true);
			
			if (SpawnNodes.Num() > 0) {
				
				AddZoneBotKnownLocations(NewBot, ZoneName);
				
				if (!bHasZonePaths)
					NewBot->SetOnlySubZonePatrols(true);

				NewBot->GetBrain()->GetBotBB()->SetValueAsBool(BB_CAN_PATROL, NewBot->GetBrain()->GetKnownLocations().Num() > 0);
			}
		}
	}	
}

void AFactionZone::ProcessSpawnedBuildingBots(TArray<class ABaseBotCharacter*> NewBots)
{
	for (ABaseBotCharacter* NewBot : NewBots)
	{
		SetupSpawnedBot(NewBot);

		if (SpawnedBots.Contains(NewBot->GetCharBPName()))
			SpawnedBots[NewBot->GetCharBPName()].Add(NewBot);

		else SpawnedBots.Add(NewBot->GetCharBPName(), TArray<ABaseBotCharacter*>{ NewBot });

	}
}

void AFactionZone::SpawnRemainingBotType(FString BotBPKey, int Max)
{
	AAIWaypoint* SpawnPoint = nullptr;
	ABaseBotCharacter* SpawnedBot = nullptr;
	
	for (int i = 0; i < Max; i++)
	{
		SpawnPoint = GetZoneSpawnPoint();

		if (!SpawnPoint) continue;

		if (SpawnBot(BotBPKey, SpawnPoint, SpawnedBot))
		{
			if (BotBPKey != SpawnedBot->GetCharBPName())
			{
				SpawnedBot->GetDamage(SpawnedBot->GetHealth(), SpawnedBot);
				return;
			}

			if (SpawnedBots.Contains(BotBPKey))
				SpawnedBots[BotBPKey].Add(SpawnedBot);
			
			else SpawnedBots.Add(BotBPKey, TArray<ABaseBotCharacter*>{ SpawnedBot });

			if(!SpawnedBot->ActorHasTag(*BotBPKey))
				SpawnedBot->Tags.Add(*BotBPKey);
		}
			
	}
}

bool AFactionZone::SpawnBot(FString BotBPKey, ABaseBotCharacter*& OutBot, FVector SpawnLocation)
{
	if (!GameFaction) return false;

	if (!GameFaction->GetBotSpawner()) return false;

	ABaseBotCharacter* NewBot = GameFaction->GetBotSpawner()->GetBot(BotBPKey, Cast<AGameFaction>(GameFaction), this, SpawnLocation);

	if (NewBot) 
	{
		SetupSpawnedBot(NewBot);

		OutBot = NewBot;

		return true;
	}

	return false;
}

bool AFactionZone::SpawnBot(FString BotBP, AAIWaypoint* SpawnPoint, ABaseBotCharacter*& OutBot)
{
	if (SpawnBot(BotBP, OutBot, SpawnPoint->GetActorLocation()))
	{
		if (OutBot->GetBrain() && OutBot->GetBrain()->DidInit())
		{
			OutBot->GetBrain()->GetBotBB()->SetValueAsObject(BB_SPAWN_POINT, SpawnPoint);

			if (!SpawnPoint->IsA<ANodeWaypoint>())
			{
				if (SpawnPoint->ActorHasTag(*WP_OUTDOOR))
				{
					FName SubZoneTag;

					for (FName Tag : SpawnPoint->Tags)
						if (Tag.ToString().Contains("_SubZone"))
							SubZoneTag = Tag;

					ANodeWaypoint* SubZoneEntry = nullptr;
					if (WaypointHandler->FindNode(SubZoneTag, SubZoneEntry))
						OutBot->GetBrain()->AddWaypoints(WP_SPAWN_NODE, TArray<AAIWaypoint*>{ SubZoneEntry }, false);
				}
			}

			else OutBot->GetBrain()->AddWaypoints(WP_SPAWN_NODE, TArray<AAIWaypoint*>{ SpawnPoint }, false); 
		}
		
		return true;
	}

	return false;
}

void AFactionZone::RemoveBot(ABaseBotCharacter* Bot)
{
	TArray<FString> Keys;
	SpawnedBots.GetKeys(Keys);

	for (FString Key : Keys)
	{
		if (SpawnedBots[Key].Contains(Bot))
		{
			SpawnedBots[Key].Remove(Bot);
		}
	}
}

void AFactionZone::AddZoneBotKnownLocations(ABaseBotCharacter* SpawnedBot, FString Zone)
{
	if (!WaypointHandler) return;

	SpawnedBot->GetBrain()->AddKnownLocations(WaypointHandler->GetVipNodes(*WP_OUTDOOR, TArray<FName>{ *WP_MAIN_ZONE, * WP_SUB_ZONE, * WP_CROSSROAD }, false)); 
}

void AFactionZone::HandleCharDeath(AActor* DeadChar, AActor* Killer)
{
	if (DeadChar->IsA<ABaseBotCharacter>())
	{
		ABaseBotCharacter* DeadBot = Cast<ABaseBotCharacter>(DeadChar);

		RemoveBot(DeadBot);

		float RespawnSeconds = DeadBot->HasBuildingAssigned() ? DeadBot->GetCharHome()->GetBotRespawnSeconds() : BotRespawnSeconds;

		if(RespawnSeconds > 0)
			GameFaction->GetBotSpawner()->GetWorld()->GetTimerManager().SetTimer(BotSpawnTimerHandle, this, &AFactionZone::InitBots, RespawnSeconds, false);
	}
}


bool AFactionZone::InitZoneCollider(FVector Location, FVector BoxExtent)
{
	ZoneCollider = NewObject<UBoxComponent>(this, FName("ZoneCollider"));
	ZoneCollider->RegisterComponent();
	ZoneCollider->InitializeComponent();
	ZoneCollider->SetRelativeLocation(Location);
	ZoneCollider->SetBoxExtent(BoxExtent);

	ZoneCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ZoneCollider->SetCollisionProfileName(FName("CharSensor"));
	ZoneCollider->SetActive(true);
	ZoneCollider->SetVisibility(true);
	ZoneCollider->SetHiddenInGame(false);
	return true;
}

void AFactionZone::OnZoneColliderBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UTargeteable>())
	{
		ITargeteable* Target = Cast<ITargeteable>(OtherActor);

		if (Target->IsTargetType() == ETargetType::PLAYER || Target->IsTargetType() == ETargetType::NPC)
		{
			if (OtherActor->Implements<UFactioneable>())
			{
				IFactioneable* Factioneable = Cast<IFactioneable>(OtherActor);

				if (Factioneable->FactionID() != ZoneFaction)
					ZoneForeigners.Add(OtherActor); 
			}
		}
	}
}

void AFactionZone::OnZoneColliderEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ZoneForeigners.Contains(OtherActor))
		ZoneForeigners.Remove(OtherActor);
}

FVector AFactionZone::GetZoneSpawnLocation() 
{
	AAIWaypoint* SpawnPoint = GetZoneSpawnPoint();

	return SpawnPoint ? SpawnPoint->GetActorLocation() : FVector::ZeroVector;
}

AAIWaypoint* AFactionZone::GetZoneSpawnPoint()
{
	if (!WaypointHandler) return nullptr;

	if (WaypointHandler->GetPathNodes().Num() <= 0) return nullptr;

	return WaypointHandler->GetPathNodes()[FMath::Clamp(FMath::RandRange(0, WaypointHandler->GetPathNodes().Num() - 1), 0, WaypointHandler->GetPathNodes().Num())];
}
