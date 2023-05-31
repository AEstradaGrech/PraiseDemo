// Fill out your copyright notice in the Description page of Project Settings.


#include "WaypointHandler.h"
#include "AIPath.h"
#include "AIRoute.h"
#include "../../../Praise.h"
#include "../../../Structs/CommonUtility/FLogger.h"

UWaypointHandler::UWaypointHandler()
{

}

TArray<ANodeWaypoint*> UWaypointHandler::GetVipNodes(FName Tag) const
{
	TArray<ANodeWaypoint*> Results;
	for (ANodeWaypoint* Node : VipNodes)
		if (Node->ActorHasTag(Tag))
			Results.Add(Node);

	return Results;
}

TArray<ANodeWaypoint*> UWaypointHandler::GetVipNodes(FName MainTag, TArray<FName> FilterTags, bool bIsAndMatch) const
{
	TArray<ANodeWaypoint*> Results;
	for (ANodeWaypoint* Node : VipNodes)
	{
		if (Node->ActorHasTag(MainTag))
		{
			int MatchCount = 0;

			for (FName Tag : FilterTags)
			{
				if (Node->Tags.Contains(Tag))
				{
					if (!bIsAndMatch) {
						Results.Add(Node);
						break;
					}

					else ++MatchCount;
				}		
			}

			if (MatchCount == FilterTags.Num())
				Results.Add(Node);
		}
	}

	return Results;
}

TArray<UAIPath*> UWaypointHandler::GetAllPaths() const
{
	TArray<UAIPath*> Results;
	TArray<FString> Keys;
	Paths.GetKeys(Keys);
	for (int i = 0; i < Keys.Num(); i++)
		Results.Add(Paths[Keys[i]]);

	return Results;
}

TArray<AAIWaypoint*> UWaypointHandler::GetWaypoints(FString Label) const
{
	if (WaypointCollections.Contains(Label))
		return WaypointCollections[Label];

	return TArray<AAIWaypoint*>();
}

TArray<AAIWaypoint*> UWaypointHandler::GetWaypoints(FName Tag, FString Label) const
{
	TArray<AAIWaypoint*> WPs = GetWaypoints(Label);
	TArray<AAIWaypoint*> SelectedWPs;
	
	for (AAIWaypoint* WP : WPs)
		if (WP->Tags.Contains(Tag))
			SelectedWPs.Add(WP);
	
	return SelectedWPs;
}

UAIPath* UWaypointHandler::GetPath(FString PathName) const
{
	if (Paths.Contains(PathName))
		return Paths[PathName];

	return nullptr;
}

UAIRoute* UWaypointHandler::GetRoute(FString RouteName)
{
	return Routes.Contains(RouteName) ? Routes[RouteName] : nullptr;
}

void UWaypointHandler::UpdateNodeArray(ANodeWaypoint* Node, bool bToVipArray, bool bClearExistent)
{
	switch (bToVipArray)
	{
	case(false):

		if (VipNodes.Contains(Node))
			VipNodes.Remove(Node);

		AddNode(Node, false, bClearExistent);

		break;
	case(true):

		if (PathNodes.Contains(Node))
			PathNodes.Remove(Node);

		AddNode(Node, true, bClearExistent);

		break;
	}
}

void UWaypointHandler::AddNode(ANodeWaypoint* Node, bool bIsVip, bool bClearExistent)
{
	switch (bIsVip)
	{
		case(false):

			if (bClearExistent)
				PathNodes.Empty();

			if(!PathNodes.Contains(Node))
				PathNodes.Add(Node);

			break;
		case(true):

			if (bClearExistent)
				VipNodes.Empty();

			if(!VipNodes.Contains(Node))
				VipNodes.Add(Node);

			break;
	}
}

void UWaypointHandler::AddNodes(TArray<ANodeWaypoint*> Nodes, bool bIsVipCollection, bool bClearExistent)
{
	switch (bIsVipCollection)
	{
		case(false):
			if (bClearExistent)
				PathNodes.Empty();
			break;
		case(true):
			if (bClearExistent)
				VipNodes.Empty();
			break;
	}

	for (ANodeWaypoint* Node : Nodes)
		AddNode(Node, bIsVipCollection);
}

bool UWaypointHandler::FindNode(int NodeID, ANodeWaypoint*& OutNode, bool bIsVip)
{
	switch (bIsVip)
	{
		case(false):
			for (ANodeWaypoint* Node : PathNodes) 
			{
				if (Node->GetNodeID() != NodeID) continue;
		
				OutNode = Node;
			
				return true;
			}
			break;
		case(true):
			for (ANodeWaypoint* Node : VipNodes)
			{
				if (Node->GetNodeID() != NodeID) continue;

				OutNode = Node;

				return true;
			}
			break;
		}

	return false;
}

bool UWaypointHandler::FindNode(int NodeID, ANodeWaypoint*& OutNode, TArray<ANodeWaypoint*> Nodes, int Max, int Min)
{
	if (Max <= 0)
		Max = Nodes.Num() - 1;

	int Mid = (Max + Min) / 2;

	if (Nodes[Mid]->GetNodeID() == NodeID)
	{
		OutNode = Nodes[Mid];
		return true;
	}

	if (NodeID > Mid)
		Min = Mid;
	
	else Max = Mid;

	return FindNode(NodeID, OutNode, Nodes, Max, Min);
}

bool UWaypointHandler::FindNodeByLabel(FString NodeLabel, ANodeWaypoint*& OutNode, bool bIsVip)
{
	switch (bIsVip)
	{
	case(false):
		for (ANodeWaypoint* Node : PathNodes)
		{
			if (Node->GetWaypointLabel() != NodeLabel) continue;

			OutNode = Node;

			return true;
		}
		break;
	case(true):
		for (ANodeWaypoint* Node : VipNodes)
		{
			if (Node->GetWaypointLabel() != NodeLabel) continue;

			OutNode = Node;

			return true;
		}
		break;
	}
	return false;
}

bool UWaypointHandler::FindNodeByLabel(FString NodeLabel, ANodeWaypoint*& OutNode, TArray<ANodeWaypoint*> Nodes)
{
	for (ANodeWaypoint* Node : Nodes)
	{
		if (Node->GetNodeName() == NodeLabel)
		{
			OutNode = Node;
			return true;
		}
	}

	return false;
}

bool UWaypointHandler::FindNode(FString NodeName, ANodeWaypoint*& OutNode, bool bIsVip)
{
	switch (bIsVip)
	{
	case(false):
		for (ANodeWaypoint* Node : PathNodes)
		{
			if (Node->GetNodeName() != NodeName) continue;

			OutNode = Node;

			return true;
		}
		break;
	case(true):
		for (ANodeWaypoint* Node : VipNodes)
		{
			if (Node->GetNodeName() != NodeName) continue;

			OutNode = Node;

			return true;
		}
		break;
	}
	return false;
}

bool UWaypointHandler::FindNode(FName Tag, ANodeWaypoint*& OutNode, bool bIsVip)
{
	switch (bIsVip)
	{
		case(false):

			for (ANodeWaypoint* Node : PathNodes)
			{
				if (!Node->Tags.Contains(Tag)) continue;

				OutNode = Node;

				return true;
			}

			break;
		case(true):

			for (ANodeWaypoint* Node : VipNodes)
			{
				if (!Node->Tags.Contains(Tag)) continue;

				OutNode = Node;

				return true;
			}

			break;
	}

	return false;
}

ANodeWaypoint* UWaypointHandler::GetClosestNode(AActor* Character, TArray<ANodeWaypoint*> Nodes)
{
	return GetClosestNode(Character->GetActorLocation(), Nodes);
}

ANodeWaypoint* UWaypointHandler::GetClosestNode(FVector Location, bool bOnlyVipNodes)
{
	TArray<ANodeWaypoint*> Nodes = bOnlyVipNodes ? VipNodes : PathNodes;

	return GetClosestNode(Location, Nodes);
}

ANodeWaypoint* UWaypointHandler::GetClosestNode(FVector Location, TArray<ANodeWaypoint*> Nodes, FName NodeTag)
{
	float Distance = 0.f;
	ANodeWaypoint* ClosestNode = nullptr;

	for (ANodeWaypoint* Node : Nodes)
	{
		if (!NodeTag.IsNone() && !Node->ActorHasTag(NodeTag)) continue;

		float DistanceToTarget = (Node->GetActorLocation() - Location).Size();

		if (Distance <= 0.f || DistanceToTarget < Distance)
		{
			ClosestNode = Node;
			Distance = DistanceToTarget;
		}
	}

	return ClosestNode;
}

ANodeWaypoint* UWaypointHandler::GetClosestNode(AActor* Character, bool bOnlyVipNodes)
{
	TArray<ANodeWaypoint*> Nodes = bOnlyVipNodes ? VipNodes : PathNodes;

	return GetClosestNode(Character, Nodes);
}

void UWaypointHandler::AddWaypoint(FString CollectionLabel, AAIWaypoint* Waypoint)
{
	if (WaypointCollections.Contains(CollectionLabel))
	{
		if (!WaypointCollections[CollectionLabel].Contains(Waypoint))
			WaypointCollections[CollectionLabel].Add(Waypoint);
	}

	else WaypointCollections.Add(CollectionLabel, TArray<AAIWaypoint*>{ Waypoint });
}

void UWaypointHandler::AddWaypoints(FString CollectionLabel, TArray<AAIWaypoint*> Waypoints, bool bClearExistent)
{
	if (WaypointCollections.Contains(CollectionLabel)) 
	{	
		if (bClearExistent)
			WaypointCollections[CollectionLabel].Empty();

		for (AAIWaypoint* WP : Waypoints)
			AddWaypoint(CollectionLabel, WP);
	}

	else
	{
		WaypointCollections.Add(CollectionLabel, TArray<AAIWaypoint*>{Waypoints[0]});

		if(Waypoints.Num() > 1) 
			for (int i = 1; i < Waypoints.Num(); i++)
				AddWaypoint(CollectionLabel, Waypoints[i]);
	}
}

void UWaypointHandler::AddPath(UAIPath* NewPath)
{
	if (Paths.Contains(NewPath->GetPathName())) return;

	Paths.Add(NewPath->GetPathName(), NewPath);
}

void UWaypointHandler::AddRoute(UAIRoute* Route)
{
	if (!Route->IsValid()) return;

	if (Routes.Contains(Route->GetRouteName()))
		Routes[Route->GetRouteName()] = Route;

	else Routes.Add(Route->GetRouteName(), Route);
}

void UWaypointHandler::AddRoute(UAIRoute* Route, FString KeyName)
{
	if (!Route->IsValid()) return;

	if (Routes.Contains(KeyName))
		Routes[KeyName] = Route;

	else Routes.Add(KeyName, Route);
}

bool UWaypointHandler::HasCollection(FString CollectionLabel)
{
	return WaypointCollections.Contains(CollectionLabel) && WaypointCollections[CollectionLabel].Num() > 0;
}

void UWaypointHandler::ClearCollection(FString CollectionLabel)
{
	if (WaypointCollections.Contains(CollectionLabel))
		WaypointCollections.Remove(CollectionLabel);
}

void UWaypointHandler::ClearNodes(bool bIsVip)
{
	bIsVip ? VipNodes.Empty() : PathNodes.Empty();
}

void UWaypointHandler::RemoveRoute(FString KeyName)
{
	if (Routes.Contains(KeyName))
		Routes.Remove(KeyName);
}

TArray<ANodeWaypoint*> UWaypointHandler::GetNodes(TArray<FString> NodeNames, TArray<ANodeWaypoint*>& Nodes)
{
	TArray<ANodeWaypoint*> Results;

	for (ANodeWaypoint* Node : Nodes) {
		FString Name = Node->GetNodeName();
		if (NodeNames.Contains(Name)) {
			Results.Add(Node);
		}
	}

	return Results;
}

TArray<ANodeWaypoint*> UWaypointHandler::GetNodes(TArray<FString> NodeNames, TArray<ANodeWaypoint*>& Nodes, int Min, int Max)
{
	TArray<ANodeWaypoint*> Results;

	for (ANodeWaypoint* Node : Nodes) {
		FString Name = Node->GetNodeName();
		if (NodeNames.Contains(Name)) {
			Results.Add(Node);
		}
	}

	return Results;
}

TArray <AAIWaypoint*> UWaypointHandler::ReverseWaypoints(TArray<AAIWaypoint*>& PathPoints)
{
	TArray<AAIWaypoint*> Reversed;
	
	for (int i = PathPoints.Num() - 1; i >= 0; i--)
		Reversed.Add(PathPoints[i]);

	return Reversed;
}

TArray <ANodeWaypoint*> UWaypointHandler::ReverseWaypoints(TArray<ANodeWaypoint*>& Nodes)
{
	TArray<ANodeWaypoint*> Reversed;

	for (int i = Nodes.Num() - 1; i >= 0; i--)
		Reversed.Add(Nodes[i]);

	return Reversed;
}

ANodeWaypoint* UWaypointHandler::GetLastAvailableNode(TArray<ANodeWaypoint*>& LevelNodes, TArray<ANodeWaypoint*>& CheckedNodes, TArray<ANodeWaypoint*>& Nodes)
{
	if (LevelNodes.Num() == 0) return nullptr;

	ANodeWaypoint* Node = LevelNodes[LevelNodes.Num() - 1];
	
	if (!CheckedNodes.Contains(Node)) return Node;

	if (Node->GetAdjacentNodes().Num() == 0)
	{
		LevelNodes.Remove(Node);

		return GetLastAvailableNode(LevelNodes, CheckedNodes, Nodes);
	}

	TArray<ANodeWaypoint*> Adjacents = GetNodes(Node->GetAdjacentNodes(), Nodes);

	if (Adjacents.Num() == 0) return nullptr;

	bool bHasNewNodes = false;
	for (ANodeWaypoint* Adjacent : Adjacents)
	{
		if (!CheckedNodes.Contains(Adjacent))
		{
			bHasNewNodes = true;
			break;
		}
	}

	if (!bHasNewNodes)
	{
		LevelNodes.Remove(Node);

		return GetLastAvailableNode(LevelNodes, CheckedNodes, Nodes);
	}

	else
	{
		return Node;
	}
}

TArray<ANodeWaypoint*> UWaypointHandler::GetRoute(ANodeWaypoint* Origin, ANodeWaypoint* Destination, TArray<ANodeWaypoint*>& AvaliableNodes)
{
	TArray<ANodeWaypoint*> CheckedNodes;
	TMap<ANodeWaypoint*,TArray<ANodeWaypoint*>> CheckedLevels;
	TArray<ANodeWaypoint*> PathLinks;
	TQueue<TTuple<ANodeWaypoint*, ANodeWaypoint*>> Q;
	
	Q.Enqueue(TTuple<ANodeWaypoint*, ANodeWaypoint*>(Origin, Origin));

	if (GetRoute_BFS(Destination, AvaliableNodes, CheckedLevels, CheckedNodes, PathLinks, Q))
	{
		if (PathLinks.Num() < 2) return TArray<ANodeWaypoint*>();

		GetNodesPath_BFS(Origin, PathLinks, CheckedLevels);

		return ReverseWaypoints(PathLinks);
	}

	return TArray<ANodeWaypoint*>();
}

bool UWaypointHandler::GetRoute_BFS(ANodeWaypoint* Destination, TArray<ANodeWaypoint*>& AvailableNodes, TMap<ANodeWaypoint*, TArray<ANodeWaypoint*>>& CheckedLevels, TArray<ANodeWaypoint*>& CheckedNodes, TArray<ANodeWaypoint*>& PathLinks, TQueue<TTuple<ANodeWaypoint*, ANodeWaypoint*>>& SearchQ)
{
	if (SearchQ.IsEmpty()) return false;

	TTuple<ANodeWaypoint*, ANodeWaypoint*> CurrentTuple;

	if(!SearchQ.Dequeue(CurrentTuple)) return false;

	ANodeWaypoint* CurrentNode = CurrentTuple.Value; 

	if (CurrentNode == nullptr) return false;

	if (!CheckedLevels.Contains(CurrentNode))
	{
		CheckedLevels.Add(CurrentNode, TArray<ANodeWaypoint*>());
		CheckedNodes.Add(CurrentNode);

		if (CurrentNode == Destination)
		{
			PathLinks.Add(CurrentNode);
			return true;
		}

		TArray<ANodeWaypoint*> Adjacents = GetNodes(CurrentNode->GetAdjacentNodes(), AvailableNodes);;

		if (Adjacents.Num() > 0)
		{
			for (ANodeWaypoint* Node : Adjacents)
			{
				if (!CheckedLevels.Contains(Node) && !CheckedNodes.Contains(Node))
				{
					CheckedLevels[CurrentNode].Add(Node);
					CheckedNodes.Add(Node);

					if (Node == Destination)
					{
						PathLinks.Add(Node);
						PathLinks.Add(CurrentNode);
						return true;
					}
					else
					{
						SearchQ.Enqueue(TTuple<ANodeWaypoint*, ANodeWaypoint*>(CurrentNode, Node));
					}
				}
			}
		}
	}

	return !SearchQ.IsEmpty() ? GetRoute_BFS(Destination, AvailableNodes, CheckedLevels, CheckedNodes, PathLinks, SearchQ) : false;
}

void UWaypointHandler::GetNodesPath_BFS(ANodeWaypoint* Origin, TArray<ANodeWaypoint*>& DestinationAndPrev, TMap<ANodeWaypoint*, TArray<ANodeWaypoint*>>& CheckedLevels)
{
	TArray<ANodeWaypoint*> Keys;
	CheckedLevels.GetKeys(Keys);
	ANodeWaypoint* Prev = DestinationAndPrev.Last();

	if (Prev == Origin) return;

	for (ANodeWaypoint* Key : Keys)
	{
		if (CheckedLevels[Key].Contains(Prev))
		{
			DestinationAndPrev.Add(Key);
			CheckedLevels.Remove(Key);
			break;
		}
	}

	if (DestinationAndPrev.Last() != Origin)
		GetNodesPath_BFS(Origin, DestinationAndPrev, CheckedLevels);
}
