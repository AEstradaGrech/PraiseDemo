// Fill out your copyright notice in the Description page of Project Settings.


#include "AIRoute.h"
#include "AIPath.h"
#include "NodeWaypoint.h"

UAIRoute::UAIRoute() : Super()
{

}

void UAIRoute::Setup(ANodeWaypoint* Origin, ANodeWaypoint* Destination, TArray<ANodeWaypoint*> Nodes, TArray<UAIPath*> Paths)
{	
	if (!Origin->IsValid()) return;

	if (!Destination->IsValid()) return;

	if (Nodes.Num() < 2) return;

	if (Paths.Num() <= 0) return;

	if (Nodes[0]->GetNodeName() != Origin->GetNodeName())
	{
		Nodes.Empty();
		return;
	}

	OriginNode = Origin;
	DestinationNode = Destination;
	PathNodes = Nodes;

	for (int i = 0; i < Paths.Num(); i++)
		RoutePaths.Add(Paths[i]->GetPathName(), Paths[i]);

	RouteName = Origin->GetNodeName().Append("-").Append(Destination->GetNodeName());

	CurrentPathNodes = TArray<ANodeWaypoint*>{ PathNodes[0], PathNodes[1] };

	PathNodes.RemoveAt(0);

	CurrentPath = GetNextPath();

	if (!CurrentPath)
	{
		PathNodes.Empty();
		RouteName.Empty();
	}
	else
	{
		CurrentPath->SetPathDirection(CurrentPathNodes[1]);
		
		CurrentPathPoint = CurrentPath->GetCurrentPathPoint();

		if (!CurrentPathPoint)
		{
			PathNodes.Empty();
			RouteName.Empty();
			return;
		}

		PathPointsQ.Empty();

		for (int i = 0; i < CurrentPath->GetPathPoints().Num(); i++)
			PathPointsQ.Enqueue(CurrentPath->GetPathPoints()[i]);
	}

}

UAIPath* UAIRoute::GetPath(FString PathName) const
{
	return RoutePaths.Contains(PathName) ? RoutePaths[PathName] : nullptr;
}

UAIPath* UAIRoute::GetNextPath()
{
	if (CurrentPathNodes.Num() < 2) return nullptr;

	UAIPath* NextPath = FindRoutePath(CurrentPathNodes[0], CurrentPathNodes[1]);

	if (!NextPath)
	{
		CurrentPathNodes.Empty();
		PathNodes.Empty();
		return nullptr;
	}

	return NextPath;
}

void UAIRoute::OnPathPointReached(AAIWaypoint* Point)
{
	if (Point->IsA<ANodeWaypoint>())
		OnPathNodeReached(Cast<ANodeWaypoint>(Point));

	else UpdateCurrentPathPoint();
}

void UAIRoute::OnPathNodeReached(ANodeWaypoint* ReachedNode)
{
	if (CurrentPathNodes.Num() != 2) return;

	if (ReachedNode->GetNodeName() == CurrentPathNodes[1]->GetNodeName())
	{
		if (PathNodes.Num() > 1)
		{
			PathNodes.RemoveAt(0);

			ANodeWaypoint* NextNode = PathNodes[0];

			TArray<ANodeWaypoint*> NewPathNodes = TArray<ANodeWaypoint*>{ CurrentPathNodes[1], NextNode };

			CurrentPathNodes = NewPathNodes;

			CurrentPath = GetNextPath();

			if (!CurrentPath) return; 

			CurrentPath->SetPathDirection(CurrentPathNodes[1]);

			PathPointsQ.Empty();
			
			for (int i = 0; i < CurrentPath->GetPathPoints().Num(); i++)
				PathPointsQ.Enqueue(CurrentPath->GetPathPoints()[i]);

			if ((*PathPointsQ.Peek())->GetUniqueID() == CurrentPathPoint->GetUniqueID() && CurrentPathPoint->GetUniqueID() == CurrentPathNodes[0]->GetUniqueID())
				PathPointsQ.Pop();

			UpdateCurrentPathPoint();
		}
		else
		{
			PathNodes.Empty();
			CurrentPathNodes.Empty();
			CurrentPathPoint = ReachedNode; 
		}
	}
	else
	{
		UpdateCurrentPathPoint();
	}
}

void UAIRoute::UpdateCurrentPathPoint()
{
	if (!CurrentPath) return;

	if (PathPointsQ.IsEmpty()) return;

	AAIWaypoint* NextPoint;

	PathPointsQ.Dequeue(NextPoint);

	if (!NextPoint) return;
	
	CurrentPathPoint = NextPoint;
}

void UAIRoute::PauseRoute(bool bPause)
{
	AAIWaypoint* Point = nullptr;
	switch (bPause)
	{
		case(true):
			
			while (!PathPointsQ.IsEmpty())
			{
				PathPointsQ.Dequeue(Point);
				
				if(!SavedPathPoints.Contains(Point))
					SavedPathPoints.Add(Point);

				if (PathPointsQ.IsEmpty())
					break;
			}
			
			bIsPaused = bPause;

			break;
		case(false):

			if (SavedPathPoints.Num() <= 0)
			{
				RouteName.Empty(); 
				return;
			};

			CurrentPathPoint = SavedPathPoints[0];

			for (int i = 0; i < SavedPathPoints.Num(); i++)
				if(SavedPathPoints[i]->GetUniqueID() != CurrentPathPoint->GetUniqueID())
					PathPointsQ.Enqueue(SavedPathPoints[i]);	
			
			SavedPathPoints.Empty();

			bIsPaused = bPause;

			break;
	}
}

void UAIRoute::PreviewRoute()
{
	if (!IsValid()) return;

	TArray<FString> Keys;
	RoutePaths.GetKeys(Keys);

	for (int i = 0; i < Keys.Num(); i++)
		RoutePaths[Keys[i]]->DebugPath();
}

UAIPath* UAIRoute::FindRoutePath(ANodeWaypoint* NodeA, ANodeWaypoint* NodeB)
{
	FString PathName = NodeA->GetNodeName().Append(FString("-")).Append(NodeB->GetNodeName());

	if (!RoutePaths.Contains(PathName))
	{
		PathName = NodeB->GetNodeName().Append(FString("-")).Append(NodeA->GetNodeName());

		return RoutePaths[PathName];
	}

	else
	{
		if(RoutePaths.Contains(PathName))
			return RoutePaths[PathName];
	}

	return nullptr;
}

bool UAIRoute::IsValid() const
{
	return OriginNode && DestinationNode && !RouteName.IsEmpty();
}

bool UAIRoute::IsFinished() const
{
	if (!IsValid()) return false;

	if (!CurrentPathPoint) return true;
	
	if (CurrentPathPoint->GetWaypointType() != EWaypointType::PATH_POINT)
		return Cast<ANodeWaypoint>(CurrentPathPoint)->GetNodeName() == DestinationNode->GetNodeName() && CurrentPathNodes.Num() == 0 && PathNodes.Num() == 0;
	
	return false;
}

TArray<class UAIPath*> UAIRoute::GetAllPaths() const
{
	TArray<UAIPath*> Paths;
	TArray<FString> Keys;
	RoutePaths.GetKeys(Keys);
	for (FString Key : Keys)
		Paths.Add(RoutePaths[Key]);

	return Paths;
}
