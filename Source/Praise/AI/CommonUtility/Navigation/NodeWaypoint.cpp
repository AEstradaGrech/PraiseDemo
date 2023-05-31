// Fill out your copyright notice in the Description page of Project Settings.


#include "NodeWaypoint.h"
#include "../../../Structs/CommonUtility/FLogger.h"
#include "../../../Environment/Buildings/BaseBuilding.h"
#include "AIPath.h"

ANodeWaypoint::ANodeWaypoint()
{
	Faction = ECharFaction::NONE;
	ZoneName = "Common Path";
	NodeID = -1;
}

bool ANodeWaypoint::TryAddPath(UAIPath* NewPath)
{
	if (NewPath->GetPathName().IsEmpty()) return false;

	if (NodePaths.Contains(NewPath->GetPathName())) return false;
	
	return NodePaths.Add(NewPath->GetPathName(), NewPath) > 0;
}

void ANodeWaypoint::SetNodeID(int Value)
{
	NodeID = Value;
	FString Suffix = FString("_").Append(FString::FromInt(Value));
	WaypointLabel.Append(Suffix);
	NodeName = ZoneName+Suffix;
}

void ANodeWaypoint::AddAdjacentNodeTag(FString Tag)
{
	if (!AdjacentNodes.Contains(Tag))
		AdjacentNodes.Add(Tag);
}

bool ANodeWaypoint::IsValid() const
{
	return Tags.Contains(FName("LocationReference")) ? true : NodeID > -1 && !ZoneName.IsEmpty() && !NodeName.IsEmpty() && AdjacentNodes.Num() > 0;
}

void ANodeWaypoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Tags.Add(*FUtilities::EnumToString(*FString("ECharFaction"), (int)Faction));
	
	if (!ZoneName.IsEmpty())
	{
		Tags.Add(*ZoneName);
		
		if (NodeID > -1)
			NodeName = ZoneName + FString("_") + FString::FromInt(NodeID);
	}
}

void ANodeWaypoint::BeginPlay()
{
	Super::BeginPlay();

	if (bDebugNode && !bIsBuildingNode)
		DrawDebugSphere(GetWorld(), GetActorLocation(), 100.f, 8, FColor::Green, true);
}

