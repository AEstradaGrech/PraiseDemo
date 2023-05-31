// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPath.h"
#include "DrawDebugHelpers.h"
#include "../../../Structs/CommonUtility/FLogger.h"

UAIPath::UAIPath() : Super()
{
	TotalDistance = 0;
	bDidSetupPathPoints = false;
}

void UAIPath::Setup(ANodeWaypoint* A, ANodeWaypoint* B, TArray<AAIWaypoint*> PathPoints, ECharFaction PathFaction, FString PathZoneName, bool bIsLink)
{
	Faction = PathFaction;
	ZoneName = PathZoneName;
	NodeA = A;
	NodeB = B;
	PathWaypoints = PathPoints;
	bIsBuildingLink = bIsLink;
	PathName = A->GetNodeName().Append("-").Append(B->GetNodeName());

	PathWaypoints.Add(NodeA);
	PathWaypoints.Add(NodeB);

}

void UAIPath::AddPathPoint(AAIWaypoint* Point)
{
	if (NodeA && NodeB && !PathWaypoints.Contains(Point))
		PathWaypoints.Add(Point);
}

AAIWaypoint* UAIPath::GetNextPathPoint()
{
	if (!IsValid()) return nullptr;

	PathWaypoints.RemoveAt(0);

	return PathWaypoints.Num() > 0 ? PathWaypoints[0] : nullptr;
}

AAIWaypoint* UAIPath::GetCurrentPathPoint() const
{
	return IsValid() ? PathWaypoints[0] : nullptr;
}

void UAIPath::RemoveCurrentPathPoint()
{
	PathWaypoints.RemoveAt(0);
}

void UAIPath::SetupPathPoints(bool bDrawPath)
{
	if (!IsValid()) return;

	if (!PathWaypoints.Contains(NodeA))
		PathWaypoints.Add(NodeA);

	if (!PathWaypoints.Contains(NodeB))
		PathWaypoints.Add(NodeB);

	bool bIsLongPath = PathWaypoints.Num() > 3;
	TMap<AAIWaypoint*, float> NodeAPathPoints;
	TMap<AAIWaypoint*, float> NodeBPathPoints;
	FVector ToNodeA;
	FVector ToNodeB;
	for (AAIWaypoint* Point : PathWaypoints)
	{
		ToNodeA = Point->GetActorLocation() - NodeA->GetActorLocation();

		if (bIsLongPath)
		{
			ToNodeB = Point->GetActorLocation() - NodeB->GetActorLocation();

			if (ToNodeA.Size() < ToNodeB.Size())
				NodeAPathPoints.Add(Point, ToNodeA.Size());

			else NodeBPathPoints.Add(Point, ToNodeB.Size());
		}

		else NodeAPathPoints.Add(Point, ToNodeA.Size());
	}

	NodeAPathPoints.ValueSort([](const float A, const float B) { return A < B; });
	
	TArray<AAIWaypoint*> NodeASegment = SetPathPoints(NodeA, NodeAPathPoints, bDrawPath);
	PathWaypoints = NodeASegment;

	if (bIsLongPath)
	{
		NodeBPathPoints.ValueSort([](const float A, const float B) { return A > B; });

		TArray<AAIWaypoint*> NodeBSegment = SetPathPoints(NodeB, NodeBPathPoints, bDrawPath);

		if(bDrawPath)
			DrawDebugLine(GetWorld(), PathWaypoints.Last()->GetActorLocation(), NodeBSegment[0]->GetActorLocation(), FColor::Red, true);

		PathWaypoints.Append(NodeBSegment);
	}
	
	bDidSetupPathPoints = true;
}

void UAIPath::SetPathDirection(ANodeWaypoint* PathEndNode)
{
	if (!bDidSetupPathPoints)
		SetupPathPoints();

	if (PathWaypoints[0]->GetUniqueID() == PathEndNode->GetUniqueID())
	{
		TArray<AAIWaypoint*> ReversedPathPoints;

		for (int i = PathWaypoints.Num() - 1; i >= 0; i--)
			ReversedPathPoints.Add(PathWaypoints[i]);

		PathWaypoints = ReversedPathPoints;
	}
}

void UAIPath::DebugPath()
{
	AAIWaypoint* A = nullptr;
	AAIWaypoint* B = nullptr;
	for (int i = 0; i < PathWaypoints.Num() - 1; i++)
	{
		A = PathWaypoints[i];
		B = PathWaypoints[i + 1];

		if (A && B)
		{
			if(i == 0 && A->IsA<ANodeWaypoint>())
				DrawDebugSphere(NodeA->GetWorld(), A->GetActorLocation(), 100.f, 8, FColor::Green, false, 30.f);
			if(B->IsA<ANodeWaypoint>())
				DrawDebugSphere(NodeA->GetWorld(), B->GetActorLocation(), 100.f, 8, FColor::Green, false, 30.f);

			DrawDebugLine(NodeA->GetWorld(), A->GetActorLocation(), B->GetActorLocation(), FColor::Red, false, 30.f);
		}
	}

}

TArray<AAIWaypoint*> UAIPath::SetPathPoints(AAIWaypoint* Node, TMap<AAIWaypoint*, float>& SortedPoints, bool bDrawPath)
{
	TArray<AAIWaypoint*> PathPointsKeys;
	SortedPoints.GetKeys(PathPointsKeys);

	AAIWaypoint* A;
	AAIWaypoint* B;
	FVector Segment;
	if (PathPointsKeys.Num() > 1)
	{
		for (int i = 1; i < PathPointsKeys.Num(); i++)
		{
			A = PathPointsKeys[i - 1];
			B = PathPointsKeys[i];

			Segment = B->GetActorLocation() - A->GetActorLocation();
			
			TotalDistance += Segment.Size();

			if(bDrawPath)
				DrawDebugLine(A->GetWorld(), A->GetActorLocation(), B->GetActorLocation(), bIsBuildingLink ? FColor::Green : FColor::Red, true);
		}
	}

	return PathPointsKeys;
}
