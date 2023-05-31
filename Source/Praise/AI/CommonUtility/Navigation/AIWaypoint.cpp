// Fill out your copyright notice in the Description page of Project Settings.


#include "AIWaypoint.h"
#include "../../../Structs/CommonUtility/FLogger.h"
#include "../../../Structs/CommonUtility/FUtilities.h"
#include "../../../Environment/Buildings/BaseBuilding.h"

AAIWaypoint::AAIWaypoint()
{
	PrimaryActorTick.bCanEverTick = false;
	WaypointType = EWaypointType::PATH_POINT;
	WaypointLabel = "";
	bIsWaitingSpot = false;
	WaitingTime = 0;
	bCanWanderArround = false;
	WanderingRadius = 0;
	
}

void AAIWaypoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (WaypointLabel != "")
		Tags.Add(*WaypointLabel);

	Tags.Add(*FUtilities::EnumToString(*FString("EWaypointType"), (int)WaypointType));

	AddActorTags();

	if (Tags.Contains("WaitingSpot")) {
		bIsWaitingSpot = true;
		if (WaitingTime <= 0.f)
			WaitingTime = 5.f;
	}

	if (bCanWanderArround && WanderingRadius <= 0.f)
		WanderingRadius = 250.f;

	if (bIsBuildingNode)
		SetupBuildingWaypoint();
}

void AAIWaypoint::BeginPlay()
{
	Super::BeginPlay();

	if (WaypointLabel != "")
		Tags.Add(*WaypointLabel);
	
	AddActorTags();
}

void AAIWaypoint::AddActorTags()
{
	for (int i = 0; i < WaypointTags.Num(); i++)
		if (!Tags.Contains(*WaypointTags[i]))
			Tags.Add(*WaypointTags[i]);
}

void AAIWaypoint::SetupBuildingWaypoint()
{
	ABaseBuilding* ParentBuilding = nullptr;
	TryAddBuildingWaypoint(ParentBuilding);
}

bool AAIWaypoint::TryAddBuildingWaypoint(ABaseBuilding*& OutBuilding)
{
	if (GetParentActor() && GetParentActor()->IsA<ABaseBuilding>())
	{
		OutBuilding = Cast<ABaseBuilding>(GetParentActor());
		OutBuilding->AddBuildingWaypoint(this);
		
		FName SubZoneTag = *(OutBuilding->GetName().Append("_Building_SubZone"));
		
		if (!ActorHasTag(SubZoneTag))
			Tags.Add(SubZoneTag);

		return true;
	}

	return false;
}


bool AAIWaypoint::TryAddTag(FString NewTag)
{
	if (WaypointTags.Contains(NewTag)) return false;

	WaypointTags.Add(*NewTag);

	AddActorTags();

	return true;
}
