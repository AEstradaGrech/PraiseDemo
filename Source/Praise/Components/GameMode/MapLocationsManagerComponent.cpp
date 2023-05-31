// Fill out your copyright notice in the Description page of Project Settings.


#include "MapLocationsManagerComponent.h"
#include "kismet/GameplayStatics.h"
#include "../../Structs/CommonUtility/FLogger.h"
#include "../../Structs/CommonUtility/FUtilities.h"
#include "../../AI/CommonUtility/Navigation/AIPath.h"
#include "../../AI/CommonUtility/Navigation/AIWaypoint.h"
#include "../../AI/CommonUtility/Navigation/NodeWaypoint.h"
#include "../../Praise.h"

// Sets default values for this component's properties
UMapLocationsManagerComponent::UMapLocationsManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


TArray<AAIWaypoint*> UMapLocationsManagerComponent::GetZoneBuildingLinks(FString ZoneName) const
{
	if (ZoneBuildingLinks.Contains(ZoneName))
		return ZoneBuildingLinks[ZoneName];

	return TArray<AAIWaypoint*>();
}

void UMapLocationsManagerComponent::AddPath(UAIPath* NewPath)
{
	if (!NewPath->IsValid()) return;

	if (!NewPath->DidSetupPathPoints())
		NewPath->SetupPathPoints();

	if (!LevelPaths.Contains(NewPath->GetPathName()))
		LevelPaths.Add(NewPath->GetPathName(), NewPath);
}

// Called when the game starts
void UMapLocationsManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (LevelPaths.Num() > 0)
	{
		TArray<FString> Keys;
		LevelPaths.GetKeys(Keys);
		for (FString Key : Keys)
			if(!LevelPaths[Key]->DidSetupPathPoints())
				LevelPaths[Key]->SetupPathPoints();
	}
}

void UMapLocationsManagerComponent::SetupLevelPaths()
{
	TArray<AActor*> OutPathPoints;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AAIWaypoint::StaticClass(), FName("PATH_POINT"), OutPathPoints);
	TArray<ANodeWaypoint*> LevelNodes = GetAllNodes();
	UAIPath* Path = nullptr;
	ANodeWaypoint* NodeA = nullptr;
	ANodeWaypoint* NodeB = nullptr;
	for (AActor* Result : OutPathPoints)
	{
		AAIWaypoint* PathPoint = Cast<AAIWaypoint>(Result);

		if (!PathPoint) continue;

		TArray<FName> NodeTags;
		for (int i = 0; i < PathPoint->Tags.Num(); i++)
			if (PathPoint->Tags[i].ToString().Contains(FString("Nodes=")) || PathPoint->Tags[i].ToString().Contains(FString("BuildingLink=")))
				NodeTags.Add(PathPoint->Tags[i]);

		if (NodeTags.Num() == 0) continue;

		for (FName Tag : NodeTags)
		{
			std::vector<std::string> splittedTag = FUtilities::Split(FUtilities::FStringToStdString(Tag.ToString()), '=');

			FString PathName = FUtilities::StdStringToFString(splittedTag[splittedTag.size() - 1]);

			if (splittedTag[0] == "Nodes")
			{
				if (!LevelPaths.Contains(PathName))
				{
					std::vector<std::string> splittedPathName = FUtilities::Split(FUtilities::FStringToStdString(PathName), '-');

					FString NodeAName = FUtilities::StdStringToFString(splittedPathName[0]);
					FString NodeBName = FUtilities::StdStringToFString(splittedPathName[1]);

					NodeA = FindNode(NodeAName, LevelNodes);
					NodeB = FindNode(NodeBName, LevelNodes);

					if (NodeA && NodeB)
					{
						Path = NewObject<UAIPath>(this, *PathName);

						Path->Setup(NodeA, NodeB, TArray<AAIWaypoint*>{ PathPoint }, NodeA->FactionID(), NodeA->GetZoneName());

						LevelPaths.Add(PathName, Path);
					}

				}

				else LevelPaths[PathName]->AddPathPoint(PathPoint);
			}
			
			if (splittedTag[0] == "BuildingLink")
			{
				std::vector<std::string> splittedLinkName = FUtilities::Split(splittedTag[1], '-');

				std::vector<std::string> splittedZoneNode = FUtilities::Split(splittedLinkName[0], '_');

				FString ZoneLinkName = FUtilities::StdStringToFString(splittedZoneNode[0]);

				if (ZoneBuildingLinks.Contains(ZoneLinkName))
				{
					if (!ZoneBuildingLinks[ZoneLinkName].Contains(PathPoint))
						ZoneBuildingLinks[ZoneLinkName].Add(PathPoint);					
				}

				else ZoneBuildingLinks.Add(ZoneLinkName, TArray<AAIWaypoint*> { PathPoint });
			}
		}
		
	}
}

bool UMapLocationsManagerComponent::IsValidCommonNode(ANodeWaypoint* Node)
{
	if (!Node->IsValid()) return false;

	if (Node->FactionID() == ECharFaction::NONE) return true; 

	return Node->GetWaypointType() == EWaypointType::ZONE_ENTRY && Node->Tags.Contains(*WP_MAIN_ZONE);
}

void UMapLocationsManagerComponent::AddFactionNode(ECharFaction Faction, ANodeWaypoint* Node)
{
	if (FactionNodes.Contains(Faction)) {
		if (!FactionNodes[Faction].Contains(Node))
			FactionNodes[Faction].Add(Node);
	}
	else FactionNodes.Add(Faction, TArray<ANodeWaypoint*>{ Node });
}

void UMapLocationsManagerComponent::CacheCommonMapLocations(bool bCacheLevelPaths)
{
	TArray<ANodeWaypoint*> Nodes = GetAllNodes();
	
	if (Nodes.Num() <= 0) return;

	ANodeWaypoint* Node = nullptr;
	for (int i = 0; i < Nodes.Num(); i++)
	{
		Node = Nodes[i];

		if (Node)
		{
			if (Node->GetWaypointType() == EWaypointType::ZONE_ENTRY && Node->Tags.Contains(*WP_MAIN_ZONE))
			{
				AddFactionNode(ECharFaction::NONE, Node);
			}

			AddFactionNode(Node->FactionID(), Node);
		}
	}

	if (bCacheLevelPaths)
		SetupLevelPaths();
}


TArray<AAIWaypoint*> UMapLocationsManagerComponent::GetCommonWaypointsByTag(FString WaypointTag)
{
	TArray<AActor*> CommonWaypoints;
	TArray<AAIWaypoint*> TaggedWaypoints;

	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AAIWaypoint::StaticClass(), *WaypointTag, CommonWaypoints);

	for (AActor* Actor : CommonWaypoints)
		TaggedWaypoints.Add(Cast<AAIWaypoint>(Actor));

	return TaggedWaypoints;
}

TArray<ANodeWaypoint*> UMapLocationsManagerComponent::GetFactionNodes(ECharFaction Faction)
{
	if (!FactionNodes.Contains(Faction))
	{
		TArray<ANodeWaypoint*> Nodes = GetAllNodes();
		
		if (Nodes.Num() <= 0) return TArray<ANodeWaypoint*>();

		for (ANodeWaypoint* Node : Nodes) {
			if (Node->FactionID() == Faction)
				AddFactionNode(Node->FactionID(), Node);
			if (Node->GetWaypointType() == EWaypointType::ZONE_ENTRY && Node->ActorHasTag(*WP_MAIN_ZONE))
				AddFactionNode(ECharFaction::NONE, Node);
		}
		
		if (!FactionNodes.Contains(Faction)) return TArray<ANodeWaypoint*>();
	}

	return FactionNodes[Faction];
}

TArray<ANodeWaypoint*> UMapLocationsManagerComponent::GetFactionZoneNodes(ECharFaction Faction, FString ZoneName)
{
	TArray<ANodeWaypoint*> Results;

	if (FactionNodes.Contains(Faction))
	{
		for (ANodeWaypoint* Node : FactionNodes[Faction])
			if (Node->FactionID() == Faction && Node->GetZoneName() == ZoneName)
				Results.Add(Node);
	}
	else
	{
		TArray<ANodeWaypoint*> Nodes = GetAllNodes();
		
		for (ANodeWaypoint* Node : Nodes) {
			if (Node->FactionID() == Faction && Node->GetZoneName() == ZoneName)
			{
				AddFactionNode(Node->FactionID(), Node);

				Results.Add(Node);
			}

			if (Node->GetWaypointType() == EWaypointType::ZONE_ENTRY && Node->ActorHasTag(*WP_MAIN_ZONE) && ZoneName == FString("Common Path"))
				AddFactionNode(ECharFaction::NONE, Node);
		}
	}

	return Results;
}

TArray<ANodeWaypoint*> UMapLocationsManagerComponent::GetAllNodes()
{
	TArray<ANodeWaypoint*> Nodes;

	if (!ensure(GetWorld() != nullptr)) return Nodes;
	
	TArray<AActor*> OutNodes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANodeWaypoint::StaticClass(), OutNodes);
	
	for (AActor* Node : OutNodes)
		if(Cast<ANodeWaypoint>(Node)->IsValid())
			Nodes.Add(Cast<ANodeWaypoint>(Node));

	return Nodes;
}

TArray<ANodeWaypoint*> UMapLocationsManagerComponent::SelectFactionNodes(ECharFaction Faction, TArray<ANodeWaypoint*> Collection)
{
	TArray<ANodeWaypoint*> Nodes;

	for (ANodeWaypoint* Node : Collection)
		if (Node->FactionID() == Faction && Node->IsValid())
			Nodes.Add(Node);

	return Nodes;
}

ANodeWaypoint* UMapLocationsManagerComponent::FindNode(FString NodeName, TArray<class ANodeWaypoint*> Collection)
{
	for (ANodeWaypoint* Node : Collection)
		if (Node->GetNodeName() == NodeName && Node->IsValid())
			return Node;

	return nullptr;
}

UAIPath* UMapLocationsManagerComponent::GetPath(FString PathName)
{
	return LevelPaths.Contains(PathName) ? LevelPaths[PathName] : nullptr;
}

UAIPath* UMapLocationsManagerComponent::GetPath(ANodeWaypoint* NodeA, ANodeWaypoint* NodeB)
{
	UAIPath* Path = nullptr;

	if (NodeA->IsValid() && NodeB->IsValid())
	{
		FString Tag = NodeB->GetNodeName().Append(FString("-")).Append(NodeA->GetNodeName());

		if (LevelPaths.Contains(Tag))
			Path = LevelPaths[Tag];

		Tag = NodeA->GetNodeName().Append(FString("-")).Append(NodeB->GetNodeName());

		if (LevelPaths.Contains(Tag))
			Path = LevelPaths[Tag];
	}

	return Path;
}

TArray<UAIPath*> UMapLocationsManagerComponent::GetFactionPaths(ECharFaction Faction)
{
	TArray<UAIPath*> Paths;
	TArray<FString> Keys;
	LevelPaths.GetKeys(Keys);

	for (int i = 0; i < Keys.Num(); i++)
		if (LevelPaths[Keys[i]]->GetPathFaction() == Faction)
			Paths.Add(LevelPaths[Keys[i]]);
	
	return Paths;
}

