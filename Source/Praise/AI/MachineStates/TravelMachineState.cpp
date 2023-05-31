// Fill out your copyright notice in the Description page of Project Settings.


#include "TravelMachineState.h"
#include "../CommonUtility/Navigation/AIRoute.h"
#include "../CommonUtility/Navigation/AIPath.h"
#include "../../Environment/Buildings/BaseBuilding.h"
#include "../CommonUtility/Navigation/BuildingEntryNode.h"
#include "../CommonUtility/Factions/NeutralsFaction.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../../Components/GameMode/FactionsManagerComponent.h"

UTravelMachineState::UTravelMachineState() : Super()
{


}

void UTravelMachineState::AddStateDecisions()
{
	TryAddStateDecision(EBotDecision::SELECT_TARGET, EBotDecision::ENGAGE_COMBAT);
}

bool UTravelMachineState::OnEnterState(ABaseBotController* NewBotController)
{
	if (!Super::OnEnterState(NewBotController)) return false;

	if (!Bot->GetCharFaction()) return false;

	if (!Bot->GetCharSpawnZone()->GetWaypointHandler()) return false;

	StateMachine->GetBotBB()->ClearValue(BB_PRIORITY_LOCATION);

	WaypointHandler = StateMachine->GetWaypointHandler();

	bDidInit = false;
	
	if (!WaypointHandler) return false;

	StoppingDistance = 120.f;
	bIsTimeLimited = false;
	UnblockPathTries = 0;
	TravelRoute = nullptr;

	StateMachine->GetBotBB()->SetValueAsBool(BB_SUBZONE_PATROL, false);
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, false);	
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_VALID_LOCATION, false); 

	NavMesh = UNavigationSystemV1::GetCurrent(Bot->GetWorld());

	if (!NavMesh) return false;
				
	if (!StateMachine->HasValidBBTarget(BB_TRAVEL_DESTINATION)) return false;

	if (!TrySetAvailableNodes())
	{
		StateMachine->GetBotBB()->SetValueAsBool(BB_WANTS_TO_TRAVEL, false);
		StateMachine->GetBotBB()->ClearValue(BB_TRAVEL_DESTINATION);
		return false;
	}

	UObject* Destination = StateMachine->GetBotBB()->GetValueAsObject(BB_TRAVEL_DESTINATION);

	if (Destination->IsA<ABasePraiseCharacter>())
	{
		if(!TryGetClosestAvailableNodeToTarget(Cast<AActor>(Destination), RouteDestination))
			return false;
	}
	else
	{
		if (!Destination->IsA<ANodeWaypoint>()) return false;

		RouteDestination = Cast<ANodeWaypoint>(Destination);
	}

	if (!RouteOrigin && !RouteDestination) return false;

	if (RouteOrigin->GetNodeName() == RouteDestination->GetNodeName()) return false;

	SetNewRoute(RouteOrigin, RouteDestination, AvailableNodes);

	if (!TravelRoute) return false;

	if (!TravelRoute->IsValid()) return false;

	bDidInit = true;

	return bDidInit;
}

void UTravelMachineState::OnExitState()
{
	Super::OnExitState();

	StateMachine->GetBotBB()->SetValueAsBool(BB_WANTS_TO_TRAVEL, false);
	StateMachine->GetBotBB()->ClearValue(BB_TRAVEL_DESTINATION);
	StateMachine->GetBotBB()->ClearValue(BB_CURRENT_WP);
	StateMachine->ClearKnownWaypoints();

	BotController->StopMovement();
}

void UTravelMachineState::RunState()
{
	if (StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_VALID_LOCATION)) 
	{
		if (!StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_REACHED_TARGET))
		{
			float DistanceToTarget = FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION)) ?
				(StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION) - Bot->GetActorLocation()).Size() : HasCurrentWaypoint() ?
				(GetCurrentWP()->GetActorLocation() - Bot->GetActorLocation()).Size() : 0.f;

			if (DistanceToTarget <= StoppingDistance)
				OnTargetReached();
		}
	}

	else {
		if (!HasCurrentWaypoint())
			SetNextPathPoint();
	}

	Super::RunState();
}

bool UTravelMachineState::HasCurrentWaypoint()
{
	return StateMachine->GetBotBB() && StateMachine->GetBotBB()->GetValueAsObject(BB_CURRENT_WP);
}

AAIWaypoint* UTravelMachineState::GetCurrentWP()
{
	return StateMachine->GetBotBB() ? Cast<AAIWaypoint>(StateMachine->GetBotBB()->GetValueAsObject(BB_CURRENT_WP)) : nullptr;
}

void UTravelMachineState::SetCurrentWP(AAIWaypoint* PathPoint)
{
	StateMachine->GetBotBB()->SetValueAsObject(BB_CURRENT_WP, PathPoint);
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_VALID_LOCATION, true);
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, false);
	
	FNavLocation Loc;
	if (NavMesh && NavMesh->ProjectPointToNavigation(PathPoint->GetActorLocation(), Loc))
		BotController->MoveToLocation(Loc.Location, 50.f, true, true);

	else BotController->MoveToActor(PathPoint);
}

bool UTravelMachineState::TryGetClosestNodeToTarget(AActor* Target, ANodeWaypoint*& OutNode, bool bOnlyCommonNodes)
{
	if (!Bot->GetCharFaction()) return false;

	ANodeWaypoint* ClosestNode = nullptr;

	switch (bOnlyCommonNodes)
	{
		case(true):
			if (!Bot->GetCharFaction()->TryGetClosestCommonNode(Target->GetActorLocation(), ClosestNode)) return false;
			break;
		case(false):
			if (!Bot->GetCharFaction()->TryGetClosestNode(Target->GetActorLocation(), ClosestNode, true)) return false;
			break;
	}
	
	OutNode = ClosestNode;

	return true;	
}

bool UTravelMachineState::TryGetClosestAvailableNodeToTarget(AActor* Target, ANodeWaypoint*& OutNode)
{
	if (AvailableNodes.Num() <= 0 && !TrySetAvailableNodes()) return false;

	ANodeWaypoint* Node = WaypointHandler->GetClosestNode(Target, AvailableNodes);

	if (!Node) return false;

	OutNode = Node;

	return true;
}

void UTravelMachineState::OnTargetReached()
{
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_VALID_LOCATION, false); 
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, true);
	UnblockPathTries = 0;

	if (!HasCurrentWaypoint()) return;

	AAIWaypoint* WP = GetCurrentWP();

	if (StateMachine->GetBotBB()->GetValueAsBool(BB_SUBZONE_PATROL)) 
	{
		
	}
	else
	{
		SetNextPathPoint();

		if (!StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_VALID_LOCATION)) 
		{
			if (TravelRoute->IsFinished())
			{
				StateMachine->ClearKnownWaypoints();

				StateMachine->GetBotBB()->SetValueAsBool(BB_WANTS_TO_TRAVEL, false);
			}
		}
	}
}

bool UTravelMachineState::ShouldRun()
{
	return false;
}

bool UTravelMachineState::ShouldBackToIdle()
{
	if (bShouldResetLoop) return true;

	if (bIsTimeLimited && TimeInCurrentState >= MaxTimeInCurrentState)
		return true;

	if (!StateMachine->GetBotBB()->GetValueAsBool(BB_WANTS_TO_TRAVEL)) return true;

	return false;
}

bool UTravelMachineState::ShouldUnblockPath()
{
	if (Super::ShouldUnblockPath())
	{
		StateMachine->GetBotBB()->SetValueAsBool(BB_WANTS_TO_TRAVEL, false);
	}

	return false;
}

void UTravelMachineState::SetNewRoute(ANodeWaypoint* OriginNode, ANodeWaypoint* DestinationNode, TArray<ANodeWaypoint*> Nodes)
{
	StateMachine->ClearKnownWaypoints();

	UAIRoute* NewRoute = nullptr;

	if (TrySetNewRoute(OriginNode, DestinationNode, Nodes, NewRoute))
	{
		if (NewRoute->IsValid() && !NewRoute->IsFinished())
		{
			TravelRoute = NewRoute;

			SetCurrentWP(TravelRoute->GetCurrentPathPoint());
		}
	}

	else
	{
		if(!StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_VALID_LOCATION))
			bShouldResetLoop = true;
	}
}

bool UTravelMachineState::TrySetNewRoute(ANodeWaypoint* Origin, ANodeWaypoint* Destination, TArray<ANodeWaypoint*> KnownLocations, UAIRoute*& OutRoute)
{
	if (KnownLocations.Num() <= 0) return false;

	if (!Destination->IsValid()) return false;

	if (Origin && Destination)
	{
		if (Origin->GetNodeName() == Destination->GetNodeName())
		{
			SetCurrentWP(Origin);								

			return false;										
		}
		
		TArray<ANodeWaypoint*> PathNodes = StateMachine->GetShortestRouteNodes(Origin, Destination, AvailableNodes);

		if (PathNodes.Num() == 0) return false;

		TArray<UAIPath*> RoutePaths = Bot->GetCharFaction()->GetRoutePaths(PathNodes);

		UAIRoute* Route = NewObject<UAIRoute>();

		Route->Setup(Origin, Destination, PathNodes, RoutePaths);

		if (StateMachine->TrySetRoute(Route, WP_CURRENT_ROUTE))
		{
			AAIWaypoint* Current = Route->GetCurrentPathPoint();
			OutRoute = Route;
			return true;
		}
	}

	return false;
}

void UTravelMachineState::SetNextPathPoint()
{
	if (!TravelRoute) return;

	if (TravelRoute->IsValid() && !TravelRoute->IsFinished() && HasCurrentWaypoint())
	{
		TravelRoute->OnPathPointReached(GetCurrentWP());

		if (TravelRoute->IsFinished())
		{
			StateMachine->GetBotBB()->SetValueAsBool(BB_WANTS_TO_TRAVEL, false);
			StateMachine->GetBotBB()->ClearValue(BB_TRAVEL_DESTINATION);
			return;
		}

		if (TravelRoute && TravelRoute->IsValid() && !TravelRoute->IsFinished())
		{
			AAIWaypoint* TargetWaypoint = TravelRoute->GetCurrentPathPoint();

			if (!TargetWaypoint) return;

			SetCurrentWP(TargetWaypoint);
		}
	}
	else
	{
		StateMachine->GetBotBB()->SetValueAsBool(BB_WANTS_TO_TRAVEL, false);
		StateMachine->GetBotBB()->ClearValue(BB_TRAVEL_DESTINATION);
		return;
	}
	
}


bool UTravelMachineState::TrySetAvailableNodes()
{
	if (!Bot->GetCharFaction()) return false;
	
	if (!RouteOrigin)
	{
		ANodeWaypoint* ClosestZoneNode = nullptr;
		if (!TryGetClosestNodeToTarget(Bot, ClosestZoneNode, false))
			return false;

		if (ClosestZoneNode->FactionID() != ECharFaction::NONE)
		{
			TArray<ANodeWaypoint*> ZoneNodes = Bot->GetCharFaction()->GetZoneNodes(ClosestZoneNode->FactionID(), ClosestZoneNode->GetZoneName());

			ANodeWaypoint* ClosestZoneEntry = WaypointHandler->GetClosestNode(Bot->GetActorLocation(), ZoneNodes, FName("MainZone"));

			if (!ClosestZoneEntry) return false;

			RouteOrigin = ClosestZoneEntry;
		}

		else RouteOrigin = ClosestZoneNode;
	}
	TArray<ANodeWaypoint*> ConnectedNodes;
	TArray<ANodeWaypoint*> CheckedNodes;
	if (!Bot->GetCharFaction()->TryGetConnectedPathNodes(RouteOrigin, ConnectedNodes, CheckedNodes)) return false;

	AvailableNodes = ConnectedNodes;
	
	if (!AvailableNodes.Contains(RouteOrigin))
		AvailableNodes.Add(RouteOrigin);

	return AvailableNodes.Num() > 0;
}
