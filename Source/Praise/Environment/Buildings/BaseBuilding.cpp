// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBuilding.h"
#include "../../Components/GameMode/BotSpawnerComponent.h"
#include "../../AI/CommonUtility/Factions/GameFaction.h"
#include "PublicBuilding.h"
#include "../../AI/CommonUtility/Factions/FactionZone.h"
#include "../../Characters/Player/PraisePlayerCharacter.h"

// Sets default values
ABaseBuilding::ABaseBuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bSpawnBotsIndoor = false;
	bShouldSpawnBots = true;

	BuildingRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Building Root Component"));
	RootComponent = BuildingRootComp;
	RootComponent->SetMobility(EComponentMobility::Static);

	Walls = CreateDefaultSubobject<USceneComponent>(TEXT("Building Walls"));
	Walls->AttachTo(RootComponent);
	Walls->SetMobility(EComponentMobility::Static);

	InteriorWalls = CreateDefaultSubobject<USceneComponent>(TEXT("Interior Walls"));
	InteriorWalls->AttachTo(RootComponent);
	InteriorWalls->SetMobility(EComponentMobility::Static);

	Floor = CreateDefaultSubobject<USceneComponent>(TEXT("Floor"));
	Floor->AttachTo(RootComponent);
	Floor->SetMobility(EComponentMobility::Static);

	Roof = CreateDefaultSubobject<USceneComponent>(TEXT("Roof"));
	Roof->AttachTo(RootComponent);
	Roof->SetMobility(EComponentMobility::Static);

	Props = CreateDefaultSubobject<USceneComponent>(TEXT("Building Props"));
	Props->AttachTo(RootComponent);
	Props->SetMobility(EComponentMobility::Static);

	Doors = CreateDefaultSubobject<USceneComponent>(TEXT("Doors"));
	Doors->AttachTo(RootComponent);
	Doors->SetMobility(EComponentMobility::Movable);

	Lights = CreateDefaultSubobject<USceneComponent>(TEXT("Lights"));
	Lights->AttachTo(RootComponent);
	Lights->SetMobility(EComponentMobility::Static);

	Waypoints = CreateDefaultSubobject<USceneComponent>(TEXT("Waypoints"));
	Waypoints->AttachTo(RootComponent);
	Waypoints->SetMobility(EComponentMobility::Movable);

	Faction = ECharFaction::NONE;
	ZoneName = "Common Path";
	bHasOwner = false;
	bAllowNonOwnersIn = true;
	bIsPrivate = false;
	bIsOpen = true;
	bIsOnFire = false;
	BuildingState = 100.f;

	BotRespawnSeconds = -1.f; 
	
}

void ABaseBuilding::AddBuildingWaypoint(AAIWaypoint* NewWP)
{
	if (!BuildingWaypoints.Contains(NewWP))
	{
		BuildingWaypoints.Add(NewWP);

		if (NewWP->IsA<ANodeWaypoint>() && NewWP->GetWaypointType() == EWaypointType::ZONE_ENTRY)
		{
			TArray<FString> BuildingLinkTags;
			ZoneLinkNodes.GetKeys(BuildingLinkTags);
			for(FString Tag : BuildingLinkTags)
				if(NewWP->ActorHasTag(ZoneLinkNodes[Tag]) && !BuildingEntries.Contains(NewWP))
					BuildingEntries.Add(Cast<ANodeWaypoint>(NewWP));
		}
	}
}

// esto convierte el edificio en privado
bool ABaseBuilding::TrySpawnBuildingBots(UBotSpawnerComponent* BotSpawner, AGameFaction* BuildingFaction, AFactionZone* BuildingZone, TArray<ABaseBotCharacter*>& OutBots)
{
	if (!BuildingZone->ShouldSpawnBuildingBots()) return false;

	if (!bShouldSpawnBots) return false;

	if (DefaultBots.Num() <= 0) return false;

	if (BuildingWaypoints.Num() <= 0) return false;

	if (BuildingEntries.Num() <= 0) return false;

	if (BotRespawnSeconds < 0)
		BotRespawnSeconds = BuildingZone->GetBotRespawnSeconds();

	AAIWaypoint* BuildingZoneEntry = nullptr;

	TArray<AAIWaypoint*> SpawnPoints;
	for (AAIWaypoint* WP : BuildingWaypoints)
	{
		if (bSpawnBotsIndoor && !WP->ActorHasTag(*WP_INDOOR)) continue;
		
		SpawnPoints.Add(WP);

		if (WP->ActorHasTag(*WP_SUB_ZONE)) 
			BuildingZoneEntry = WP;
	}

	if (!BuildingZoneEntry) return false;

	AAIWaypoint* SpawnPoint = BuildingZoneEntry;

	if (SpawnPoints.Num() == 0)
		SpawnPoints.Add(SpawnPoint);

	TArray<FString> Keys;
	DefaultBots.GetKeys(Keys);

	for (FString BPTag : Keys)
	{
		if (DefaultBots[BPTag] <= 0) continue;

		int SpawnedBots = 0;

		for (ABasePraiseCharacter* Char : AssignedChars)
			if (Char->IsA<ABaseBotCharacter>())
				++SpawnedBots;

		if (SpawnedBots >= DefaultBots[BPTag]) continue;

		for (int i = 0; i < DefaultBots[BPTag] - SpawnedBots; i++)
		{
			SpawnPoint = SpawnPoints[FMath::Clamp(FMath::RandRange(0, SpawnPoints.Num() - 1), 0, SpawnPoints.Num())];

			ABaseBotCharacter* NewBot = BotSpawner->GetBot(BPTag, BuildingFaction, BuildingZone, SpawnPoint->GetActorLocation());

			if (!NewBot) continue;

			if (!NewBot->DidBrainInit())
				NewBot->TryInitBrain();
			
			if (TryAssignChar(NewBot)) 
			{
				if (bIsPrivate)
					for (ANodeWaypoint* Entry : BuildingEntries)
						BuildingZone->GetWaypointHandler()->UpdateNodeArray(Entry, false); 

				NewBot->SetCharHome(this);

				if (bOnlyBuildingPatrols) 
					NewBot->SetOnlySubZonePatrols(bOnlyBuildingPatrols);

				NewBot->GetBrain()->AddWaypoints(BuildingZoneEntry->GetWaypointLabel(), BuildingWaypoints, false); 
			}

			NewBot->GetBrain()->GetBotBB()->SetValueAsObject(BB_SPAWN_POINT, SpawnPoint);
			NewBot->GetBrain()->AddWaypoints(WP_SPAWN_NODE, TArray<AAIWaypoint*>{  BuildingZoneEntry }, false);

			OutBots.Add(NewBot);
		}

	}

	if (BotRespawnSeconds <= 0)
		bShouldSpawnBots = false;

	return OutBots.Num() > 0;
}

void ABaseBuilding::OnBuildingCharDead(ABasePraiseCharacter* Char)
{
	if (Char->GetCharHome() != this) return;

	if(AssignedChars.Contains(Char))
		AssignedChars.Remove(Char);
	
	if (AssignedChars.Num() == 0 && BotRespawnSeconds <= 0)
		SetIsPrivate(false);
	
	if (Char->IsA<APraisePlayerCharacter>())
	{
		// TODO
	}
}

ANodeWaypoint* ABaseBuilding::GetMainDoorNode()
{
	ANodeWaypoint* MainNode = nullptr;
	if (BuildingEntries.Num() <= 0)
	{
		if (BuildingWaypoints.Num() <= 0) return nullptr;

		for (AAIWaypoint* WP : BuildingWaypoints)
		{

			if (WP->IsA<ANodeWaypoint>() && WP->GetWaypointType() == EWaypointType::ZONE_ENTRY)
				BuildingEntries.Add(Cast<ANodeWaypoint>(WP));

			if (WP->IsA<ANodeWaypoint>() && WP->ActorHasTag(FName("MainDoorNode")))
				MainNode = Cast<ANodeWaypoint>(WP);
		}
	}
	else
	{
		for (ANodeWaypoint* Node : BuildingEntries)
			if (Node->ActorHasTag("MainDoorNode"))
				MainNode = Node;
	}

	return MainNode;
}

void ABaseBuilding::SetIsPrivate(bool bPrivate)
{
	switch (bPrivate)
	{
		case(true):

			bIsPrivate = true;

			bIsOpen = false;

			bHasOwner = true; 

			if (Tags.Contains("PublicZone"))
				Tags.Remove("PublicZone");

			if (!Tags.Contains(FName("PrivateZone")))
				Tags.Add(FName("PrivateZone"));

			break;

		case(false):

			bIsPrivate = false;

			bIsOpen = true;

			bHasOwner = false; 

			if (Tags.Contains("PrivateZone"))
				Tags.Remove("PrivateZone");

			if (!Tags.Contains(FName("PublicZone")))
				Tags.Add(FName("PublicZone"));

			break;
	}



}

bool ABaseBuilding::TryAssignChar(ABasePraiseCharacter* Char)
{
	if (Faction == Char->GetCharFaction()->FactionID())
	{
		if (!AssignedChars.Contains(Char))
			AssignedChars.Add(Char);

		SetIsPrivate(true);

		return true;
	}

	return false;
}

void ABaseBuilding::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!ZoneName.IsEmpty())
		Tags.Add(*ZoneName);
}

// Called when the game starts or when spawned
void ABaseBuilding::BeginPlay()
{
	Super::BeginPlay();

	if (bDebugWaypoints)
		for(AAIWaypoint* WP : BuildingWaypoints)
			DrawDebugSphere(GetWorld(), WP->GetActorLocation(), 50.f, 8, WP->IsA<ANodeWaypoint>() ? FColor::Green : FColor::Cyan, true);
	

	if (bDisableLights)
	{
		TArray<USceneComponent*>OutLights;
		Lights->GetChildrenComponents(true, OutLights);

		if (OutLights.Num() > 0)
		{
			for (USceneComponent* Light : OutLights)
				if(Light->ComponentHasTag(FName("IndoorLight")))
					Light->SetVisibility(false, true);
		}
	}
}


