// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolMachineState.h"
#include "../CommonUtility/Navigation/AIRoute.h"
#include "../CommonUtility/Navigation/AIPath.h"
#include "../../Environment/Buildings/BaseBuilding.h"
#include "../CommonUtility/Navigation/BuildingEntryNode.h"
#include "../CommonUtility/Factions/NeutralsFaction.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"

UPatrolMachineState::UPatrolMachineState() : Super()
{
	WaypointWaitingTime = 5.f;
	
	SubZonePatrolSecs = 0;
	MinSubZonePatrolSecs = 60;
	MaxSubZonePatrolSecs = 90;
}

void UPatrolMachineState::AddStateDecisions()
{
	TryAddStateDecision(EBotDecision::RUN_AWAY);
	TryAddStateDecision(EBotDecision::SELECT_TARGET, EBotDecision::ENGAGE_COMBAT);
}

bool UPatrolMachineState::OnEnterState(ABaseBotController* NewBotController)
{
	if (!Super::OnEnterState(NewBotController)) return false;
	
	if (!Bot->GetCharFaction()) return false;

	if (!Bot->GetCharSpawnZone()) return false;

	if (!Bot->GetCharSpawnZone()->GetWaypointHandler()) return false;

	StateMachine->GetBotBB()->ClearValue(BB_PRIORITY_LOCATION);

	WaypointHandler = StateMachine->GetWaypointHandler();

	if (WaypointHandler)
	{
		if (!TrySetAvailableNodes())
		{
			StateMachine->GetBotBB()->SetValueAsBool(BB_CAN_PATROL, false);
			bDidInit = false;
			return false;
		}

		if (StateMachine->GetBotBB()->GetValueAsBool(BB_CLEAR_ROUTE_ON_PATROL_ENTER))
			StateMachine->ClearRoute();
		
		StoppingDistance = 120.f;
		bIsTimeLimited = false;
		MaxUnblockPathTriesPerWP = 3;
		UnblockPathTries = 0;
		SubZonePatrolSecs = 0;
									
		StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, false);
		StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_VALID_LOCATION, false); 
		
		NavMesh = UNavigationSystemV1::GetCurrent(Bot->GetWorld());

		if (!NavMesh)
		{
			bDidInit = false;
			return bDidInit;
		}
		
		if (StateMachine->GetBotBB()->GetValueAsBool(BB_ONLY_SUBZONE_PATROL))
			StateMachine->GetBotBB()->SetValueAsBool(BB_SUBZONE_PATROL, true);

		if (!StateMachine->HasRoute())
		{
			if (StateMachine->GetBotBB()->GetValueAsBool(BB_SUBZONE_PATROL))
			{
				FString SubZoneTag = Bot->GetCharSpawnZone()->GetZoneName().Append(FString("_SubZone"));
				ANodeWaypoint* ZoneEntry = nullptr;

				if (!WaypointHandler->FindNodeByLabel(SubZoneTag, ZoneEntry, true))
				{
					bDidInit = false;
					return false;
				}

				else SetSubZonePatrol(ZoneEntry);
			}
			else
			{
				ANodeWaypoint* RouteOrigin = Cast<ANodeWaypoint>(WaypointHandler->GetWaypoints(WP_SPAWN_NODE)[0]);

				if (RouteOrigin->IsA<ABuildingEntryNode>())
				{
					ABuildingEntryNode* BuildingNode = Cast<ABuildingEntryNode>(RouteOrigin);

					if (BuildingNode->IsPublicZone())
					{
						SetNewRoute(RouteOrigin, AvailableNodes);
					}
					else
					{
						TArray<ANodeWaypoint*> ZonePublicNodes = Bot->GetCharSpawnZone()->GetZoneNodes(true, FName("PrivateZone"), /*bExcludeTagged:*/ true);

						if (ZonePublicNodes.Num() <= 0) return false;

						RouteOrigin = ZonePublicNodes[0];

						if (RouteOrigin)
							SetNewRoute(RouteOrigin, AvailableNodes);
					}
				}

				else SetNewRoute(RouteOrigin, AvailableNodes);
			}
		}

		else SetNewAvailableRoute();
	}

	else bDidInit = false;

	bDidInit = true;

	return bDidInit;
}

void UPatrolMachineState::OnExitState()
{
	BotController->MoveToActor(Bot);

	Super::OnExitState();

	StateMachine->GetBotBB()->ClearValue(BB_CURRENT_WP);
	StateMachine->ClearKnownWaypoints();

	BotController->StopMovement();
}

void UPatrolMachineState::RunState()
{
	if (StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_VALID_LOCATION))
	{ 
		if (!StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_REACHED_TARGET))
		{
			float DistanceToTarget = bIsWanderingArroundWP && FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION)) ? 
				(StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION) - Bot->GetActorLocation()).Size() : HasCurrentWaypoint() ? 
				(GetCurrentWP()->GetActorLocation() - Bot->GetActorLocation()).Size() : 0.f;

			if (DistanceToTarget <= StoppingDistance)
				OnTargetReached();

			if (Bot->GetVelocity().Size() <= 0 && !Bot->IsBeingDamaged())
			{
				FNavLocation Loc;
				if (NavMesh && NavMesh->ProjectPointToNavigation(GetCurrentWP()->GetActorLocation(), Loc))
					BotController->MoveToLocation(Loc.Location, 50.f, true, true);

				else BotController->MoveToActor(GetCurrentWP());
			}

		}
	}

	else {
		if (!HasCurrentWaypoint())
			SetNextPathPoint();
	}
	
	
	Super::RunState();
}

bool UPatrolMachineState::HasCurrentWaypoint()
{
	return StateMachine->GetBotBB() && StateMachine->GetBotBB()->GetValueAsObject(BB_CURRENT_WP);
}

AAIWaypoint* UPatrolMachineState::GetCurrentWP()
{
	return StateMachine->GetBotBB() ? Cast<AAIWaypoint>(StateMachine->GetBotBB()->GetValueAsObject(BB_CURRENT_WP)) : nullptr;
}

void UPatrolMachineState::SetCurrentWP(AAIWaypoint* PathPoint)
{
	if (!StateMachine->GetBotBB()->GetValueAsBool(BB_SUBZONE_PATROL))
	{
		if (!PatrolRoute) return;

		if (!PatrolRoute->GetCurrentPath()) return;

		StateMachine->AddKnownWaypoints(PatrolRoute->GetCurrentPath()->GetPathPoints(), true);
	}
	
	StateMachine->GetBotBB()->SetValueAsObject(BB_CURRENT_WP, PathPoint);
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_VALID_LOCATION, true);
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, false);
	
	FNavLocation Loc;
	if (NavMesh && NavMesh->ProjectPointToNavigation(PathPoint->GetActorLocation(), Loc))
		BotController->MoveToLocation(Loc.Location, 50.f, true, true);
	
	else BotController->MoveToActor(PathPoint);	
}

void UPatrolMachineState::SetSubZonePatrol(ANodeWaypoint* SubZoneEntry)
{
	if (!SubZoneEntry->IsA<ANodeWaypoint>()) return;

	if (SubZoneEntry->GetWaypointLabel().IsEmpty()) return;

	FString SubZoneLabel = SubZoneEntry->IsA<ABuildingEntryNode>() ? Cast<ABuildingEntryNode>(SubZoneEntry)->GetNodeBuilding()->GetBuildingName() : SubZoneEntry->GetWaypointLabel();

	if (!Bot->GetCharSpawnZone()->GetWaypointHandler()->HasCollection(SubZoneLabel))
	{
		SetNewRoute(SubZoneEntry, AvailableNodes);
		return;
	}

	TArray<AAIWaypoint*> SubZoneWPs = Bot->GetCharSpawnZone()->GetWaypointHandler()->GetWaypoints(SubZoneLabel);

	if (SubZoneWPs.Num() > 0)
	{
		StateMachine->GetBotBB()->SetValueAsBool(BB_SUBZONE_PATROL, true);
		
		StateMachine->AddKnownWaypoints(SubZoneWPs, true);

		SubZonePatrolSecs = FMath::RandRange(MinSubZonePatrolSecs, MaxSubZonePatrolSecs);

		SubZoneEnterTimestamp = StateMachine->GetTimeSinceGameStart();

		if(!StateMachine->GetBotBB()->GetValueAsBool(BB_ONLY_SUBZONE_PATROL))
			Bot->GetWorld()->GetTimerManager().SetTimer(ResumeSubZonePatrolTimer, this, &UPatrolMachineState::ResumeSubZonePatrol, SubZonePatrolSecs + 10.f, false);

		SetNextPathPoint();

		return;
	}

	else SetNewRoute(SubZoneEntry, AvailableNodes);
}

void UPatrolMachineState::OnTargetReached()
{
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_VALID_LOCATION, false); 
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, true);
	UnblockPathTries = 0;

	if (!HasCurrentWaypoint()) return;

	WaypointWaitingTime = 0;

	AAIWaypoint* WP = GetCurrentWP();

	if (StateMachine->GetBotBB()->GetValueAsBool(BB_SUBZONE_PATROL))
	{
		if (bIsWanderingArroundWP)
		{
			StateMachine->GetBotBB()->ClearValue(BB_PRIORITY_LOCATION);

			Bot->GetWorld()->GetTimerManager().SetTimer(WanderArroundTimer, this, &UPatrolMachineState::WandeArroundWP, FMath::RandRange(5.f, 8.f), false);
		}
		else
		{
			if (WP->IsWaitingSpot() && WP->GetWaitingTime() > 0)
			{
				WaypointWaitingTime = WP->GetWaitingTime();

				if (WP->CanWanderArround())
					WandeArroundWP();

				Bot->GetWorld()->GetTimerManager().SetTimer(WaypointWaitingTimer, this, &UPatrolMachineState::SetNextPathPoint, GetWaitingTime(), false);

				return;
			}

			else SetNextPathPoint();
		}
	}
	else
	{
		SetNextPathPoint();

		if (!StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_VALID_LOCATION)) 
		{
			if (PatrolRoute->IsFinished())
			{
				StateMachine->ClearKnownWaypoints();

				ANodeWaypoint* DestinationNode = PatrolRoute->GetDestination();

				if (DestinationNode && DestinationNode->GetWaypointType() == EWaypointType::ZONE_ENTRY)
				{
					if (DestinationNode->ActorHasTag(*WP_SUB_ZONE))
					{
						SetSubZonePatrol(DestinationNode);
					}

					if (DestinationNode->ActorHasTag(*WP_MAIN_ZONE))
					{
						SetNewRoute(Cast<ANodeWaypoint>(WP), AvailableNodes);
					}
				}

				else SetNewRoute(Cast<ANodeWaypoint>(WP), AvailableNodes);		
			}
		}
	}
}

float UPatrolMachineState::GetWaitingTime(bool bExactTime)
{
	if (!bExactTime) {
		int rnd = FMath::FRandRange(-1, 1);
		return WaypointWaitingTime + (WaypointWaitingTime * 25 / 100) * (rnd == 0 ? 1 : rnd);
	}

	return WaypointWaitingTime;
}

void UPatrolMachineState::WandeArroundWP()
{
	bIsWanderingArroundWP = false;

	if (!HasCurrentWaypoint()) return;

	AAIWaypoint* CurrentWaypoint = GetCurrentWP();

	if (!CurrentWaypoint->CanWanderArround()) return;

	float WanderingRadius = CurrentWaypoint->GetWanderingRadius();

	if (WanderingRadius <= 0) return;

	FNavLocation Loc;

	if (NavMesh->GetRandomReachablePointInRadius(CurrentWaypoint->GetActorLocation(), WanderingRadius, Loc))
	{
		bIsWanderingArroundWP = true;
		StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_VALID_LOCATION, true); 
		StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, false);
		StateMachine->GetBotBB()->SetValueAsVector(BB_PRIORITY_LOCATION, Loc.Location);
		BotController->MoveToLocation(Loc.Location, 50.f, true, true);
	}

	else SetNextPathPoint();
	
	
}

void UPatrolMachineState::ResumeSubZonePatrol()
{
	Bot->GetWorld()->GetTimerManager().ClearTimer(ResumeSubZonePatrolTimer);
	Bot->GetWorld()->GetTimerManager().ClearTimer(WanderArroundTimer);
	bIsWanderingArroundWP = false;

	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_VALID_LOCATION, false); 
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, false);
	StateMachine->GetBotBB()->ClearValue(BB_PRIORITY_LOCATION);

	TArray<AAIWaypoint*> SubZoneWPs = StateMachine->GetKnownWaypoints();

	TArray<ANodeWaypoint*> SubZoneEntries;
	for (AAIWaypoint* WP : SubZoneWPs)
		if (WP->GetWaypointType() == EWaypointType::ZONE_ENTRY && WP->IsA<ANodeWaypoint>())
			SubZoneEntries.Add(Cast<ANodeWaypoint>(WP));

	TArray<ANodeWaypoint*> Nodes;
	for (ANodeWaypoint* Node : AvailableNodes)
		if (!SubZoneEntries.Contains(Node))
			Nodes.Add(Node);

	if (SubZoneEntries.Num() > 0)
		SetNewRoute(SubZoneEntries[FMath::RandRange(0, SubZoneEntries.Num() - 1)], Nodes);

	else SetNewRoute(StateMachine->GetRandomKnownLocation(), AvailableNodes);
}


bool UPatrolMachineState::ShouldUnblockPath()
{
	return false;
}

bool UPatrolMachineState::ShouldRun()
{
	return false; 
}

bool UPatrolMachineState::ShouldBackToIdle()
{
	if (bIsTimeLimited && TimeInCurrentState >= MaxTimeInCurrentState)
		return true;

	if (!StateMachine->GetBotBB()->GetValueAsBool(BB_CAN_PATROL)) return true;

	return StateMachine->GetKnownWaypoints().Num() <= 0 && !HasCurrentWaypoint() && StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION) == FVector::ZeroVector;
}

void UPatrolMachineState::SetNewRoute(ANodeWaypoint* OriginNode, TArray<ANodeWaypoint*> Nodes)
{
	StateMachine->ClearKnownWaypoints();

	UAIRoute* NewRoute = nullptr;
	
	if (Nodes.Contains(OriginNode))
		Nodes.Remove(OriginNode);

	if (TrySetNewRoute(OriginNode, Nodes, NewRoute))
	{
		if (NewRoute->IsValid() && !NewRoute->IsFinished())
		{
			StateMachine->GetBotBB()->SetValueAsBool(BB_SUBZONE_PATROL, false);

			PatrolRoute = NewRoute;

			SetCurrentWP(PatrolRoute->GetCurrentPathPoint());
		}
	}
}

void UPatrolMachineState::SetNewAvailableRoute()
{
	ANodeWaypoint* AvailableNode = GetAvailableNode(false);

	if (!AvailableNode)
	{
		bDidInit = false;
	}

	else SetNewRoute(AvailableNode, WaypointHandler->GetVipNodes());
}

bool UPatrolMachineState::TrySetNewRoute(ANodeWaypoint* Origin,TArray<ANodeWaypoint*> KnownLocations, UAIRoute*& OutRoute)
{
	if (KnownLocations.Num() <= 0) return false;

	if (KnownLocations.Contains(Origin))
		KnownLocations.Remove(Origin);

	ANodeWaypoint* NewDestination = KnownLocations[FMath::Clamp(FMath::RandRange(0, KnownLocations.Num() - 1), 0, KnownLocations.Num())];

	if (Origin->GetNodeName() == NewDestination->GetNodeName()) return false;

	if (!NewDestination->IsValid())
	{
		KnownLocations.Remove(NewDestination);
		
		return TrySetNewRoute(Origin, KnownLocations, OutRoute);;
	}

	//This will give allways the shortest path, but the combination of the sorted BFS, DFS - ASC, DESC returns 
	// a path to the target but not necessarily the shortest path and seems better to me for random patrols

	//	TArray<ANodeWaypoint*> PathNodes = WaypointHandler->GetRoute(Origin, NewDestination, AvailableNodes);
	
	TArray<ANodeWaypoint*> SearchGraph;
	TArray<ANodeWaypoint*> PathNodes = StateMachine->GetRouteNodes(Origin, NewDestination, AvailableNodes, SearchGraph, FMath::RandRange(0, 100) > 50, FMath::RandRange(0,100) > 50);

	if (PathNodes.Num() == 0) return false;

	TArray<UAIPath*> RoutePaths = Bot->GetCharFaction()->GetRoutePaths(PathNodes);

	UAIRoute* Route = NewObject<UAIRoute>();

	Route->Setup(Origin, NewDestination, PathNodes, RoutePaths);

	if (StateMachine->TrySetRoute(Route, WP_CURRENT_ROUTE))
	{
		AAIWaypoint* Current = Route->GetCurrentPathPoint();
		OutRoute = Route;
		return true;
	}

	return false;
}

void UPatrolMachineState::SetNextPathPoint()
{
	if (StateMachine->GetBotBB()->GetValueAsBool(BB_SUBZONE_PATROL))
	{
		bIsWanderingArroundWP = false;
		StateMachine->GetBotBB()->ClearValue(BB_PRIORITY_LOCATION);
		
		if (StateMachine->GetBotBB()->GetValueAsBool(BB_ONLY_SUBZONE_PATROL) || StateMachine->GetTimeSinceGameStart() - SubZoneEnterTimestamp < SubZonePatrolSecs)
		{
			if (StateMachine->GetKnownWaypoints().Num() == 0) return;

			TArray<AAIWaypoint*> SubZoneEntries;
			TArray<AAIWaypoint*> SubZoneWPs;

			for (AAIWaypoint* WP : StateMachine->GetKnownWaypoints())
			{
				if (WP->GetWaypointType() == EWaypointType::ZONE_ENTRY)
				{
					SubZoneEntries.Add(WP);

					if (!WP->ActorHasTag(*WP_WAITING_SPOT)) continue;
				}

				SubZoneWPs.Add(WP);
			}

			if (SubZoneWPs.Num() > 0)
			{
				AAIWaypoint* NextWP = SubZoneWPs[FMath::RandRange(0, SubZoneWPs.Num() - 1)];

				if (NextWP)
					SetCurrentWP(NextWP);
			}
			else
			{
				TArray<ANodeWaypoint*> Nodes;
				for (ANodeWaypoint* Node : AvailableNodes)
					if (!SubZoneEntries.Contains(Node))
						Nodes.Add(Node);

				SetNewRoute(Cast<ANodeWaypoint>(GetCurrentWP()), Nodes.Num() > 0 ? Nodes : AvailableNodes);
			}
		}

		else ResumeSubZonePatrol();
	}
	else
	{
		if (!PatrolRoute) return;

		if (PatrolRoute->IsValid() && !PatrolRoute->IsFinished() && HasCurrentWaypoint())
		{
			PatrolRoute->OnPathPointReached(GetCurrentWP());

			if (PatrolRoute->IsFinished())
			{
				StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_VALID_LOCATION, false);

				return;
			}

			if (PatrolRoute && PatrolRoute->IsValid() && !PatrolRoute->IsFinished())
			{
				AAIWaypoint* TargetWaypoint = PatrolRoute->GetCurrentPathPoint();

				if (!TargetWaypoint) return;

				SetCurrentWP(TargetWaypoint);
			}
		}

		else
		{

			ANodeWaypoint* ClosestNode = WaypointHandler->GetClosestNode(Bot);

			if (!ClosestNode)
			{
				StateMachine->GetBotBB()->SetValueAsBool(BB_CAN_PATROL, false);
				return;
			}
		}
	}
}

ANodeWaypoint* UPatrolMachineState::GetAvailableNode(bool bRandomNode)
{
	ANodeWaypoint* Node = nullptr;

	if (AvailableNodes.Num() <= 0 && !TrySetAvailableNodes())
	{
		StateMachine->GetBotBB()->SetValueAsBool(BB_CAN_PATROL, false);
		return Node;
	}

	if (!bRandomNode)
	{
		Node = WaypointHandler->GetClosestNode(Bot, Bot->GetCharSpawnZone()->GetZoneNodes());

		if (!Node)
		{
			if(WaypointHandler->HasCollection(WP_SPAWN_NODE))
				Node = Cast<ANodeWaypoint>(WaypointHandler->GetWaypoints(WP_SPAWN_NODE)[0]);
		}
	}

	else Node = AvailableNodes[FMath::Clamp(FMath::RandRange(0, AvailableNodes.Num() - 1), 0, AvailableNodes.Num())];

	return Node;
}

bool UPatrolMachineState::ShouldEnterSubZone()
{
	return false;
}

bool UPatrolMachineState::TrySetAvailableNodes()
{
	if (!WaypointHandler->HasCollection(WP_SPAWN_NODE))
		return false; 

	if (Bot->GetCharSpawnZone() && Bot->GetCharSpawnZone()->GetZoneName() == FString("Common Path"))
	{
		if (!Bot->GetCharFaction()->IsA<ANeutralsFaction>()) return false;

		TArray<ANodeWaypoint*> CommonPathNodes = Bot->GetCharFaction()->GetPathNodes();

		if (CommonPathNodes.Num() <= 0) return false;

		ANodeWaypoint* Origin = Cast<ANodeWaypoint>(WaypointHandler->GetWaypoints(WP_SPAWN_NODE)[0]);

		if (!Origin) return false;

		TArray<ANodeWaypoint*> PathNodes;
		TArray<ANodeWaypoint*> CheckedNodes;

		if (Bot->GetCharFaction()->TryGetConnectedPathNodes(Origin, PathNodes, CommonPathNodes, CheckedNodes))
		{
			AvailableNodes = PathNodes;

			if (AvailableNodes.Num() <= 0) return false;
		}
	}
	else
	{
		AvailableNodes = AvailableNodes = Bot->GetCharSpawnZone()->GetZoneNodes(false, FName("PrivateZone"), /*bExcludeTagged:*/ true);

		if (AvailableNodes.Num() > 0)
		{
			if (Bot->HasBuildingAssigned())
				AvailableNodes.Append(Bot->GetCharHome()->GetBuildingEntries());
		}
		else
		{
			bDidInit = false;
			return false;
		}
	}

	return true;
}




