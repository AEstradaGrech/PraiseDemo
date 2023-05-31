// Fill out your copyright notice in the Description page of Project Settings.
#include "BotBrainComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "../../../Components/Actor/CharStats/BotStatsComponent.h"
#include "GameFramework/GameStateBase.h"
#include "kismet/GameplayStatics.h"
#include "../../../Enums/AI/EChaseMode.h"
#include "../../../AI/Tasks/ProcessKnownTargetsAsyncTask.h"
#include "../../../AI/Tasks/ProcessDamageAsyncTask.h"
#include "../../../AI/Tasks/TryEnvQueryAsyncTask.h"
#include "../../../AI/CommonUtility/Navigation/AIRoute.h"
#include "../../../Characters/AI/BotCharacters/SeekerBotCharacter.h"
#include "T200BrainComponent.h"
#include "BTBrainComponent.h"



// Sets default values for this component's properties
UBotBrainComponent::UBotBrainComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bAutoRegister = true;
	bAutoActivate = true;
	bSprintStamEnabled = false;
	bShouldUnblockPath = false;
	PerceptionCheckSeconds = 2.5f;
	MaxTimeBlocked = 5.f;
	TimeBlocked = 0;
	BotBB = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Bot Blackboard"));
	
	BotBB->SetValueAsBool(BB_SUBZONE_PATROL, false);
	BotBB->SetValueAsBool(BB_CAN_WANDER, true);
	
	SetDefaults();

	BotWaypointHandler = CreateDefaultSubobject<UWaypointHandler>(TEXT("Bot Waypoint Handler"));
	TargetUpdateSecs = 1.f;
	// ...
}



void UBotBrainComponent::ForceStopBrain()
{
	SetBrainEnabled(false);
}

float UBotBrainComponent::GetTimeSinceGameStart() const
{
	return  Bot && Bot->GetWorld() && Bot->GetWorld()->GetGameState() ? (Bot->GetWorld()->GetGameState()->GetGameTimeSinceCreation() + Bot->GetWorld()->GetGameState()->CreationTime) : 0.f;
}

void UBotBrainComponent::SetDefaults()
{
	bShouldStayInIdle = false;
	MaxKnownWaypoints = 10;
	RecentDamageResetTime = 15.f;
	DamageAlertResetTime = 7.f;
	SightRadius = 4000.f;

	UnblockPathLocRecallSecs = 30.f;
	UnblockPathRecallTimerSecs = 5.f;
	MaxUnblockPathRetries = 3;
	UnblockPathRetries = 0;
}

// Called when the game starts
void UBotBrainComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UBotBrainComponent::SetupBB()
{
	if (!BotBB) return;

	FVector SpawnLoc = FUtilities::IsValidVector(Bot->GetSpawnLocation()) ? Bot->GetSpawnLocation() : Bot->GetActorLocation();
	BotBB->SetValueAsVector(BB_SPAWN_LOC, SpawnLoc);
	BotBB->SetValueAsBool(BB_CAN_RUN, true);
	BotBB->SetValueAsBool(BB_CAN_TRAVEL, Bot->CanEverTravel());
	BotBB->SetValueAsBool(BB_CAN_PATROL, Bot->CanEverPatrol());
	BotBB->SetValueAsBool(BB_SELECT_CLOSEST_FOE, false);
	BotBB->SetValueAsVector(BB_COVER_SPOT, FVector::ZeroVector);

	BotBB->SetValueAsFloat(BB_GLOBAL_MAX_IDLE, Bot->GetMaxTimeInIdle());
	BotBB->SetValueAsFloat(BB_MAX_WANDERING, Bot->GetMaxTimeWandering());
	BotBB->SetValueAsFloat(BB_MAX_CHASING, Bot->GetMaxTimeChasing());
	BotBB->SetValueAsFloat(BB_MAX_RUNAWAY, Bot->GetMaxTimeRunningAway());
	BotBB->SetValueAsFloat(BB_MIN_RUNAWAY, BB_GLOBAL_MIN_RUNAWAY_VAL);
	BotBB->SetValueAsFloat(BB_MAX_SNITCH, BB_GLOBAL_MAX_SNITCH_VAL);
	BotBB->SetValueAsFloat(BB_MIN_SNITCH, BB_GLOBAL_MIN_SNITCH_VAL);

	BotBB->SetValueAsFloat(BB_CHASE_TRIGGER_DISTANCE, BB_CHASE_TRIGGER_DISTANCE_VAL);
	BotBB->SetValueAsFloat(BB_WANDERING_RADIUS, BB_WANDERING_RADIUS_VAL);
	BotBB->SetValueAsInt(BB_WANDERING_MODE, (int)EWanderMode::RANDOM);
	BotBB->SetValueAsInt(BB_CHASING_MODE, this->IsA<UT200BrainComponent>() ? (int)EChaseMode::SEEK : (int)EChaseMode::PURSUE);
	

	//DefaultChaseMode
	BotBB->SetValueAsBool(BB_ONEXIT_CLEAR_TARGET, true);
	BotBB->SetValueAsBool(BB_CLEAR_ROUTE_ON_PATROL_ENTER, false);
	BotBB->SetValueAsBool(BB_DEAD_TARGETS_ALLOWED, false);
	BotBB->SetValueAsBool(BB_ONLY_ENEMY_TARGETS_ALLOWED, true);
	BotBB->SetValueAsBool(BB_IS_PAUSED, false);
	BotBB->SetValueAsBool(BB_IS_PATH_BLOCKED, false);
	BotBB->SetValueAsBool(BB_HAS_REACHED_TARGET, false);
	BotBB->SetValueAsBool(BB_HAS_VALID_LOCATION, false);
	BotBB->SetValueAsBool(BB_WANTS_TO_TRAVEL, false);
	BotBB->SetValueAsBool(BB_CURRENT_PATH_IS_PARTIAL, false);
	BotBB->ClearValue(BB_TRAVEL_DESTINATION);

	ClearBBTarget();
}

void UBotBrainComponent::SetChaseMode(EChaseMode ChaseMode, float MaxChaseStateTime, bool bOnChaseExitClearTarget)
{
	BotBB->SetValueAsBool(BB_ONEXIT_CLEAR_TARGET, bOnChaseExitClearTarget);
	BotBB->SetValueAsInt(BB_CHASING_MODE, (int)ChaseMode);
	BotBB->SetValueAsFloat(BB_MAX_CHASING, MaxChaseStateTime);
}

void UBotBrainComponent::SetWanderMode(EWanderMode WanderMode, float MaxWanderStateTime, float WanderingRadius)
{
	BotBB->SetValueAsInt(BB_WANDERING_MODE, (int)WanderMode);
	BotBB->SetValueAsFloat(BB_WANDERING_RADIUS, WanderingRadius);
	BotBB->SetValueAsFloat(BB_MAX_WANDERING, MaxWanderStateTime);
}

bool UBotBrainComponent::BeginEnvQuery(FEnvQueryConfig& QueryConfig, TArray<FVector>& QueryResults, int MaxResults)
{
	if (bIsQueryingEnv) return false;

	if (!(Bot && EnvQuerier)) return false;

	bIsQueryingEnv = true;

	if (!EnvQuerier->TryQuery(QueryConfig)) {
		bIsQueryingEnv = false;
		return false;
	}

	if (MaxResults < 1)
		MaxResults = 1;

	QueryResults = EnvQuerier->GetBestResults(MaxResults);

	bIsQueryingEnv = false;

	return true;
}

void UBotBrainComponent::ClearBB()
{
	BotBB->ClearValue(BB_CAN_PATROL);
	BotBB->ClearValue(BB_CAN_WANDER);
	BotBB->ClearValue(BB_MAX_WANDERING);
	BotBB->ClearValue(BB_MAX_CHASING);
	BotBB->ClearValue(BB_MAX_RUNAWAY);
	BotBB->ClearValue(BB_MIN_RUNAWAY);
	BotBB->ClearValue(BB_MAX_SNITCH);
	BotBB->ClearValue(BB_MIN_SNITCH);
	BotBB->ClearValue(BB_IS_PATH_BLOCKED);
	BotBB->ClearValue(BB_HAS_REACHED_TARGET);
	BotBB->ClearValue(BB_HAS_VALID_LOCATION);

}

void UBotBrainComponent::ClearBBTarget(AActor* Target)
{
	TryClearBBTarget(BB_PRIORITY_TARGET, Target);
	TryClearBBTarget(BB_LAST_DAMAGER, Target);
	TryClearBBTarget(BB_MAX_AGGRO_TARG, Target);
	TryClearBBTarget(BB_MAX_DMG_TARG, Target);
	TryClearBBTarget(BB_LAST_SEEN_TARGET, Target);
}

void UBotBrainComponent::ClearBBTarget()
{
	if (!BotBB) return;

	BotBB->ClearValue(BB_PRIORITY_TARGET);
	BotBB->ClearValue(BB_LAST_SEEN_TARGET);
	BotBB->ClearValue(BB_LAST_DAMAGER);
	BotBB->ClearValue(BB_MAX_AGGRO_TARG);
	BotBB->ClearValue(BB_MAX_DMG_TARG);
	BotBB->ClearValue(BB_PRIORITY_LOCATION);
	BotBB->ClearValue(BB_LAST_PRIORITY_LOCATION);
	BotBB->ClearValue(BB_CURRENT_WP);
	BotBB->SetValueAsVector(BB_COVER_SPOT, FVector::ZeroVector);

	bShouldTarget = false;
}

void UBotBrainComponent::TryClearBBTarget(FName BBKey, AActor* Target)
{
	if (!BotBB) return;

	AActor* BBTarget = nullptr;

	BBTarget = Cast<AActor>(BotBB->GetValueAsObject(BBKey));

	if (BBTarget && BBTarget->GetUniqueID() == Target->GetUniqueID())
		BotBB->ClearValue(BBKey);
}

void UBotBrainComponent::OnUnblockPathLocsRecall()
{
	TArray<FVector> Keys;
	UnblockPathLocs.GetKeys(Keys);
	for (int i = 0; i < Keys.Num(); i++)
	{
		if (GetTimeSinceGameStart() - UnblockPathLocs[Keys[i]] >= UnblockPathLocRecallSecs)
			UnblockPathLocs.Remove(Keys[i]);
	}

	if(UnblockPathLocs.Num() > 0)
		Bot->GetWorld()->GetTimerManager().SetTimer(UnblockPathLocsCheckTimer, this, &UBotBrainComponent::OnUnblockPathLocsRecall, UnblockPathRecallTimerSecs, false);
}

void UBotBrainComponent::AddNeutralDamager(AActor* Damager, int ExtraHits)
{
	if (NeutralDamagers.Contains(Damager))
	{
		++NeutralDamagers[Damager];
		
		if (NeutralDamagers[Damager] >= Bot->GetNeutralHitsTolerance() + ExtraHits)
		{
			if (Damager->IsA<ABasePraiseCharacter>())
			{
				ABasePraiseCharacter* Character = Cast<ABasePraiseCharacter>(Damager);

				if (TryAddTarget(Cast<ITargeteable>(Character), KnownEnemies))
				{
					Bot->OnNewFoE.ExecuteIfBound(Damager, true);
					NeutralDamagers.Remove(Damager);
					
					BotBB->SetValueAsObject(BB_LAST_SEEN_TARGET, Damager);
					BotBB->SetValueAsVector(BB_LAST_KNOWN_TARGET_LOC, Damager->GetActorLocation());
					
					Character->OnNotifyDead.AddDynamic(Bot, &ABasePraiseCharacter::HandleCharDeadNotify);
					
					if (Bot->GetCharStatus() != ECharStatus::ALERTED)
						Bot->UpdateCharStatus(ECharStatus::ALERTED);

					AddRecallCheckTimer();
					AddLastKnownEnemyLocCheckTimer();
				}
				
			}
		}
	}

	else NeutralDamagers.Add(Damager, 1);

	GetWorld()->GetTimerManager().SetTimer(NeutralDamagersRecallCheckTimer, this, &UBotBrainComponent::ClearNeutralDamgers, 20.f, false);
}

void UBotBrainComponent::ClearLastKnownEnemyLoc()
{
	if (!BotBB) return;

	BotBB->ClearValue(BB_LAST_KNOWN_TARGET_LOC);
}

void UBotBrainComponent::TryResetRecentDamageMap()
{
	if (!BotBB) return;

	if (!ensure(GetWorld() != nullptr)) return;

	if (DamageReceivedTimeStamp > 0)
	{
		float ElapsedSecs = (GetWorld()->GetGameState()->GetServerWorldTimeSeconds() - DamageReceivedTimeStamp);

		if (ElapsedSecs >= DamageAlertResetTime) {
			BotBB->SetValueAsBool(BB_HAS_BEEN_DAMAGED, false);
			BotBB->ClearValue(BB_LAST_DAMAGER);
		}

		if (ElapsedSecs >= RecentDamageResetTime) {
			RecentlyReceivedDamageMap.Empty();

			BotBB->ClearValue(BB_MAX_AGGRO_TARG);
		}
	}
	

	if (RecentlyReceivedDamageMap.Num() <= 0 && !HasValidBBTarget(BB_LAST_DAMAGER))
	{
		BotBB->ClearValue(BB_LAST_DAMAGER_TO_BOT_VECTOR);

		if (KnownEnemies.Num() <= 0)					
			Bot->UpdateCharStatus(ECharStatus::NORMAL); 
	}
	
	else GetWorld()->GetTimerManager().SetTimer(ResetDamageTimeHandle, this, &UBotBrainComponent::TryResetRecentDamageMap, 2.f);
}

void UBotBrainComponent::ClearNeutralDamgers()
{
	NeutralDamagers.Empty();
}

bool UBotBrainComponent::TryInitCommonBB()
{
	if (!BotBB)
	{
		BotBB = NewObject<UBlackboardComponent>(this, FName("Common Blackboard"));
		
		if (!BotBB) return false;
		
		BotBB->RegisterComponent();
		BotBB->InitializeComponent();
	}

	if (!BotBB->GetBlackboardAsset()) 
	{
		UBlackboardData* Data = Cast<UBlackboardData>(StaticLoadObject(UBlackboardData::StaticClass(), NULL, *FString("/Game/Core/AI/BlackBoards/BB_CommonBlackboard")));

		if (!Data) return false;
		
		BBData = Data;
		
		if (BBData->IsValid() && !BotBB->InitializeBlackboard(*BBData)) return false;
	}

	return BotBB->InitializeBlackboard(*BotBB->GetBlackboardAsset());
}

void UBotBrainComponent::AddRecallCheckTimer()
{
	if (!Bot) return;

	if (!ensure(Bot->GetWorld() != nullptr)) return;

	Bot->GetWorld()->GetTimerManager().SetTimer(TargetRecallCheckTimer, this, &UBotBrainComponent::OnTargetRecallCheck, Bot->GetCharStats<UBotStatsComponent>()->GetTargetRecallSeconds(), false);	
}

void UBotBrainComponent::AddLastKnownEnemyLocCheckTimer()
{
	if (!Bot) return;

	if (!ensure(Bot->GetWorld() != nullptr)) return;

	Bot->GetWorld()->GetTimerManager().SetTimer(LastKnownEnemyCheckTimer, this, &UBotBrainComponent::ClearLastKnownEnemyLoc, Bot->GetCharStats<UBotStatsComponent>()->GetLastKnownEnemyRecallSeconds(), false);
}

AActor* UBotBrainComponent::GetClosestTarget(TArray<AActor*> Targets, ECharVector LocationCondition, bool bAllowDeadTargets)
{
	FVector Distance;
	AActor* Closest = nullptr;
	for (int i = 0; i < Targets.Num(); i++)
	{
		if (!Targets[i]) continue;

		FVector ToTarget = Targets[i]->GetActorLocation() - Bot->GetActorLocation();

		if (i == 0 || ToTarget.Size() < Distance.Size())
		{
			if (!bAllowDeadTargets)
				if (Targets[i]->Implements<UTargeteable>())
					if (Cast<ITargeteable>(Targets[i])->IsDead())
						continue;
		
			switch (LocationCondition)
			{
				case(ECharVector::FWD):
					if (!Bot->IsTargetForward(Targets[i]->GetActorLocation(), 45.f)) continue;
					break;
				case(ECharVector::BWD):
					if (Bot->IsTargetBackward(Targets[i]->GetActorLocation(), 45.f)) continue;
					break;
				case(ECharVector::LEFT):
					if (Bot->IsTargetAside(Targets[i]->GetActorLocation(), ECharVector::LEFT, 45.f)) continue;
					break;
				case(ECharVector::RIGHT):
					if (Bot->IsTargetAside(Targets[i]->GetActorLocation(), ECharVector::RIGHT, 45.f)) continue;
					break;
				case(ECharVector::NONE):
				default:
					break;
			}
			
			Distance = ToTarget;
			Closest = Targets[i];
		}
	}

	return Closest;
}

void UBotBrainComponent::ProcessItem(ITargeteable* Item)
{
	if (!Bot->GetCharStats() || !Bot->GetCharStats()->IsA<UBotStatsComponent>()) return;

	if (KnownItems.Num() < Cast<UBotStatsComponent>(Bot->GetCharStats())->GetMaxPerceivedTargets())
		if(TryAddKnownItem(Item))				
			AddRecallCheckTimer();
}

void UBotBrainComponent::ProcessCharacter(ABasePraiseCharacter* Character)
{
	if (Character == Bot) return;

	if (Bot->IsDead()) return;

	if (!Bot->GetCharStats() || !Bot->GetCharStats()->IsA<UBotStatsComponent>()) return;

	try 
	{
		if (Character->Implements<UFactioneable>())
		{
			if (Bot->GetIgnoredFactions().Contains(Cast<IFactioneable>(Character)->FactionID())) return;

			if (Bot->IsFriendTarget(Character) && KnownAllies.Num() <= Cast<UBotStatsComponent>(Bot->GetCharStats())->GetMaxPerceivedAllies())
				TryAddCharTarget(Character, false);

			if (Bot->IsEnemyTarget(Character) && KnownEnemies.Num() <= Cast<UBotStatsComponent>(Bot->GetCharStats())->GetMaxPerceivedEnemies())
				TryAddCharTarget(Character, true);
		}
	}
	catch (std::exception ex)
	{
		FLogger::LogTrace(__FUNCTION__ + FString(ex.what()));
	}
}

bool UBotBrainComponent::InitBrain(ABaseBotController* OwnerController, ABaseBotCharacter* BrainOwner)
{
	BotController = OwnerController; 
	Bot = BrainOwner; 
	
	SetDefaults();

	if (!this->IsA<UBTBrainComponent>())
	{
		bDidInit = TryInitCommonBB();

		if (!bDidInit) return false;
	}

		
	if (Bot->GetCharStats() && Cast<UBotStatsComponent>(Bot->GetCharStats()))
	{
		AttackAwaitSecs = Cast<UBotStatsComponent>(Bot->GetCharStats())->GetAttackAwaitSeconds();
		EvadeAwaitSecs = Cast<UBotStatsComponent>(Bot->GetCharStats())->GetEvadeAwaitSeconds();
	}

	else return false;

	SetupBB();

	if (BrainOwner->OnlySubZonePatrols())
		BotBB->SetValueAsBool(BB_ONLY_SUBZONE_PATROL, true);

	bDidInit = true;

	return bDidInit;
}

void UBotBrainComponent::SetBrainEnabled(bool bIsEnabled)
{
	bIsBrainEnabled = bIsEnabled; 
	SetComponentTickEnabled(bIsBrainEnabled);
	
	if (Bot && bIsBrainEnabled)
	{
		UBotEnvQuerier* Querier = NewObject<UBotEnvQuerier>(this);

		if (Querier)
		{
			Querier->Setup(Bot);
			EnvQuerier = Querier;
		}
	}

	if(!bIsEnabled)
		BotController->StopMovement();
}

bool UBotBrainComponent::HasRoute(FString KeyName)
{
	if (!BotWaypointHandler) return false;

	UAIRoute* CurrentRoute = BotWaypointHandler->GetRoute(KeyName);

	return CurrentRoute && CurrentRoute->IsValid();
}

UAIRoute* UBotBrainComponent::GetRoute(FString KeyName) const
{
	if (!BotWaypointHandler) return nullptr;

	return BotWaypointHandler->GetRoute(KeyName);
}

void UBotBrainComponent::ClearRoute(FString KeyName)
{
	if (!BotWaypointHandler) return;

	BotWaypointHandler->RemoveRoute(KeyName);
}

bool UBotBrainComponent::TrySetRoute(UAIRoute* NewRoute, FString KeyName)
{
	if (!BotWaypointHandler) return false;

	if (!NewRoute->IsValid()) return false;

	if (KeyName.IsEmpty())
		KeyName = NewRoute->GetRouteName();

	BotWaypointHandler->AddRoute(NewRoute, KeyName);

	return true;
}

ANodeWaypoint* UBotBrainComponent::GetCurrentDestination() const
{
	if (!BotWaypointHandler) return nullptr;

	UAIRoute* CurrentRoute = BotWaypointHandler->GetRoute(WP_CURRENT_ROUTE);

	if (!CurrentRoute) return nullptr;
	
	if (!CurrentRoute->IsValid()) return nullptr;

	return CurrentRoute->GetDestination();
}

TArray<ANodeWaypoint*> UBotBrainComponent::GetRouteNodes(ANodeWaypoint* InitialNode, ANodeWaypoint* DestinationNode, TArray<ANodeWaypoint*> AvailableNodes, TArray<ANodeWaypoint*>& OutSearchGraph, bool bIsDSF, bool bIsDescendingSearch)
{
	TArray<ANodeWaypoint*> PathNodes; 

	if (AvailableNodes.Num() <= 0) return PathNodes;

	switch (bIsDSF)
	{
		case(true):
			if (GetRouteNodes_DSF(InitialNode, DestinationNode, PathNodes, OutSearchGraph, AvailableNodes, bIsDescendingSearch))
				return BotWaypointHandler->ReverseWaypoints(PathNodes);
			break;
		case(false):
			TArray<ANodeWaypoint*> VertexNodes = TArray<ANodeWaypoint*>{ InitialNode };
			if (GetRouteNodes_BSF(VertexNodes, DestinationNode, PathNodes, OutSearchGraph, AvailableNodes, bIsDescendingSearch))
			{
				return BotWaypointHandler->ReverseWaypoints(PathNodes);
			}

			break;
	}
	
	
	return PathNodes.Num() >= 2 ? PathNodes : TArray<ANodeWaypoint*>();
}

TArray<ANodeWaypoint*> UBotBrainComponent::GetShortestRouteNodes(ANodeWaypoint* InitialNode, ANodeWaypoint* DestinationNode, TArray<ANodeWaypoint*> AvailableNodes)
{
	TArray<ANodeWaypoint*> AscendingGraph;
	TArray<ANodeWaypoint*> DescendingGraph;

	TArray<ANodeWaypoint*> AscendingSearch = GetRouteNodes(InitialNode, DestinationNode, AvailableNodes, AscendingGraph, false, false);
	TArray<ANodeWaypoint*> DescendingSearch = GetRouteNodes(InitialNode, DestinationNode, AvailableNodes, DescendingGraph, false, true);

	return (AscendingSearch.Num() == 0 && DescendingSearch.Num() == 0) ?
		   TArray<ANodeWaypoint*>() : AscendingSearch.Num() == 0 ?
		   DescendingSearch : DescendingSearch.Num() == 0 ? 
		   AscendingSearch : AscendingSearch.Num() <= DescendingSearch.Num() ? 
		   AscendingSearch : DescendingSearch;
}

bool UBotBrainComponent::GetRouteNodes_DSF(ANodeWaypoint* Vertex, ANodeWaypoint* TargetNode, TArray<ANodeWaypoint*>& PathNodes, TArray<ANodeWaypoint*>& CheckedNodes, TArray<ANodeWaypoint*>& Nodes, bool bIsDescendingSearch)
{
	if (CheckedNodes.Contains(Vertex)) return false;

	CheckedNodes.Add(Vertex);

	if (Vertex == TargetNode)
	{
		if (!PathNodes.Contains(Vertex))
			PathNodes.Add(Vertex);

		return true;
	}

	TArray<ANodeWaypoint*> Adjacents = BotWaypointHandler->GetNodes(Vertex->GetAdjacentNodes(), Nodes);

	if (Adjacents.Num() == 0) return false;

	if (bIsDescendingSearch)
		Adjacents.Sort([&](const ANodeWaypoint& A, const ANodeWaypoint& B) { return A.GetNodeID() > B.GetNodeID(); });

	else Adjacents.Sort([&](const ANodeWaypoint& A, const ANodeWaypoint& B) { return A.GetNodeID() < B.GetNodeID(); });

	for (int i = 0; i < Adjacents.Num(); i++)
	{
		if(Adjacents[i]->GetAdjacentNodes().Num() > 0)
		{
			if (GetRouteNodes_DSF(Adjacents[i], TargetNode, PathNodes, CheckedNodes, Nodes, bIsDescendingSearch))
			{
				PathNodes.Add(Vertex);
				return true;
			}
		}
		else
		{
			if (!CheckedNodes.Contains(Adjacents[i]))
				CheckedNodes.Add(Adjacents[i]);
		}
	}
	
	return false;
}

bool UBotBrainComponent::GetRouteNodes_BSF(TArray<ANodeWaypoint*>& LevelNodes, ANodeWaypoint* TargetNode, TArray<ANodeWaypoint*>& PathNodes, TArray<ANodeWaypoint*>& CheckedNodes, TArray<ANodeWaypoint*>& Nodes, bool bIsDescendingSearch)
{
	if (LevelNodes.Num() == 0) return false;

	ANodeWaypoint* LastNode = LevelNodes[LevelNodes.Num() -1];

	if (!CheckedNodes.Contains(LastNode))
		CheckedNodes.Add(LastNode);

	if (LastNode == TargetNode)
	{
		if(!PathNodes.Contains(LastNode)) 
			PathNodes.Add(LastNode);

		return true;
	}

	TArray<ANodeWaypoint*> VertexNodes;
	if (LastNode->GetAdjacentNodes().Num() > 0)
	{
		VertexNodes = BotWaypointHandler->GetNodes(LastNode->GetAdjacentNodes(), Nodes);
		bool bHasNewNodes = false;
		if (VertexNodes.Num() > 0)
		{
			if (bIsDescendingSearch)
				VertexNodes.Sort([&](const ANodeWaypoint& A, const ANodeWaypoint& B) { return A.GetNodeID() > B.GetNodeID(); });

			else VertexNodes.Sort([&](const ANodeWaypoint& A, const ANodeWaypoint& B) { return A.GetNodeID() < B.GetNodeID(); });

			for (int i = VertexNodes.Num() - 1; i >= 0; i--)
			{
				if (CheckedNodes.Contains(VertexNodes[i])) continue;

				bHasNewNodes = true;

				CheckedNodes.Add(VertexNodes[i]);

				if (VertexNodes[i] == TargetNode)
				{
					if(!PathNodes.Contains(VertexNodes[i]))
						PathNodes.Add(VertexNodes[i]);
					
					if (!PathNodes.Contains(LastNode))
						PathNodes.Add(LastNode);
					
					LevelNodes.Add(VertexNodes[i]);

					return true;
				}

				if(VertexNodes[i]->GetAdjacentNodes().Num() > 0)
					LevelNodes.Add(VertexNodes[i]);
			}

			if (!bHasNewNodes) 
				LevelNodes.Remove(LastNode);
		}
	}

	else LevelNodes.Remove(LastNode);

	if (GetRouteNodes_BSF(LevelNodes, TargetNode, PathNodes, CheckedNodes, Nodes, bIsDescendingSearch))
	{
		if(LevelNodes.Contains(LastNode) && !PathNodes.Contains(LastNode))
			PathNodes.Add(LastNode);

		return true;
	}

	else return false;
}

TArray<ANodeWaypoint*> UBotBrainComponent::GetKnownLocations() const
{
	if (!BotWaypointHandler) return TArray<ANodeWaypoint*>();

	return BotWaypointHandler->GetVipNodes();
}

ANodeWaypoint* UBotBrainComponent::GetRandomKnownLocation(TArray<ANodeWaypoint*> Excluded, TArray<ANodeWaypoint*> Locations)
{
	if(Locations.Num() == 0)
		Locations = BotWaypointHandler->GetVipNodes();

	if (Locations.Num() == 0) return nullptr;

	ANodeWaypoint* Loc = Locations[FMath::Clamp(FMath::RandRange(0, Locations.Num() - 1), 0, Locations.Num())];

	if (Excluded.Contains(Loc))
	{
		Locations.Remove(Loc);

		return GetRandomKnownLocation(Excluded, Locations);
	}

	else return Loc;
}

void UBotBrainComponent::AddKnownLocation(ANodeWaypoint* VipNode, bool bClearExistent)
{
	if (!BotWaypointHandler) return;

	BotWaypointHandler->AddNode(VipNode, true, bClearExistent);
}

void UBotBrainComponent::AddKnownLocations(TArray<class ANodeWaypoint*> VipNodes, bool bClearExistent)
{
	if (!BotWaypointHandler) return;

	BotWaypointHandler->AddNodes(VipNodes, true, bClearExistent);
}

void UBotBrainComponent::AddWaypoints(FString CollectionLabel, TArray<AAIWaypoint*> WPs, bool bClearExistent)
{
	if (!BotWaypointHandler) return;

	BotWaypointHandler->AddWaypoints(CollectionLabel, WPs, bClearExistent);
}

void UBotBrainComponent::AddKnowPathNodes(TArray<class ANodeWaypoint*> CurrentPathNodes, bool bClearExistent)
{
	if (!BotWaypointHandler) return;

	BotWaypointHandler->AddNodes(CurrentPathNodes, false, bClearExistent);
}

TArray<ITargeteable*> UBotBrainComponent::GetKnownCharTargets(bool bIncludeDeadTargets, bool bCheckOnlyEnemies, bool bCheckOnlyAllies)
{
	TArray<ITargeteable*> Targets = TArray<ITargeteable*>();
	if (!bCheckOnlyEnemies && !bCheckOnlyAllies)
	{
		Targets.Append(GetKnownEnemyTargets(bIncludeDeadTargets));
		Targets.Append(GetKnownFriendlyTargets(bIncludeDeadTargets));
		return Targets;
	}

	if (bCheckOnlyEnemies)
		return GetKnownEnemyTargets(bIncludeDeadTargets);

	if (bCheckOnlyAllies)
		return GetKnownFriendlyTargets(bIncludeDeadTargets);

	return Targets;
}

TArray<ITargeteable*> UBotBrainComponent::GetKnownEnemyTargets(bool bIncludeDeadTargets)
{
	TArray<ITargeteable*> Targets = TArray<ITargeteable*>();

	for (AActor* Enemy : GetKnownEnemies()) {
		if (Enemy && Enemy->Implements<UTargeteable>()) {
			if (!bIncludeDeadTargets && !Cast<ITargeteable>(Enemy)->IsDead())
				Targets.Add(Cast<ITargeteable>(Enemy));

			else Targets.Add(Cast<ITargeteable>(Enemy));
		}
	}

	return Targets;
}

TArray<ITargeteable*> UBotBrainComponent::GetKnownFriendlyTargets(bool bIncludeDeadTargets)
{
	TArray<ITargeteable*> Targets = TArray<ITargeteable*>();

	for (AActor* Ally : GetKnownAllies()) {
		if (Ally && Ally->Implements<UTargeteable>()) {
			if (!bIncludeDeadTargets && !Cast<ITargeteable>(Ally)->IsDead())
				Targets.Add(Cast<ITargeteable>(Ally));

			else Targets.Add(Cast<ITargeteable>(Ally));
		}
	}

	return Targets;
}

ITargeteable* UBotBrainComponent::GetClosestTarget(TArray<ITargeteable*> Targets)
{
	FVector Distance;
	ITargeteable* Closest = nullptr;
	for (int i = 0; i < Targets.Num(); i++)
	{
		if (!Targets[i]) continue;

		FVector ToTarget = Cast<AActor>(Targets[i])->GetActorLocation() - Bot->GetActorLocation();

		if (i == 0 || ToTarget.Size() < Distance.Size())
		{
			Distance = ToTarget;
			Closest = Targets[i];
		}
	}

	return Closest;
}

void UBotBrainComponent::AddKnownWaypoints(TArray<AAIWaypoint*> WPs, bool bOverride)
{
	if (!bOverride)
	{
		for (int i = 0; i < WPs.Num(); i++)
			if (KnownWaypoints.Num() <= MaxKnownWaypoints)
				KnownWaypoints.Add(WPs[i]);
	}

	else KnownWaypoints = WPs;
}

void UBotBrainComponent::ResetLoop()
{
	ClearBB();
	ClearBBTarget();
	SetupBB();
}

void UBotBrainComponent::PauseBrain(bool bPause, bool bResetLoop)
{
	switch (bPause)
	{
		case(true):
			BotBB->SetValueAsBool(BB_IS_PAUSED, bPause);
			BotController->StopMovement();
			break;
		case(false):
			BotBB->SetValueAsBool(BB_IS_PAUSED, bPause);
			break;
	}
	
	if (bResetLoop)
		ResetLoop();
}

AActor* UBotBrainComponent::GetPriorityTarget()
{
	return HasValidBBTarget(BB_PRIORITY_TARGET) ? Cast<AActor>(BotBB->GetValueAsObject(BB_PRIORITY_TARGET)) : nullptr;
}

void UBotBrainComponent::SetPriorityTarget(AActor* NewTarget)
{
	if (!NewTarget) return; 

	if (BotBB) 
		BotBB->SetValueAsObject(BB_PRIORITY_TARGET, NewTarget);

	if (Bot->IsTargeting())
	{
		if(Bot->GetLockedTarget()->GetUniqueID() != NewTarget->GetUniqueID())
			OnTargetUpdate.ExecuteIfBound(NewTarget, true);
	}
		
}

// Called every frame
void UBotBrainComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (BotBB->GetValueAsBool(BB_IS_PATH_BLOCKED))
		HandleBlockedPath(DeltaTime);
	// ...
}

void UBotBrainComponent::OnPerceptionCheck()
{
	TArray<AActor*> PercievedActors = Bot->GetSweepResult(Bot->GetActorLocation(), DefaultPerceptionRadius, true, ECollisionChannel::ECC_GameTraceChannel3, TArray<AActor*>{ Bot } );
	for (AActor* PercievedActor : PercievedActors)
	{
		if (PercievedActor->Implements<UTargeteable>() && !Cast<ITargeteable>(PercievedActor)->IsDead() && PercievedActor->GetVelocity().Size() >= 0.2f) {
			TryAddNewTarget(Cast<ITargeteable>(PercievedActor));
		}
	}
}

void UBotBrainComponent::OnTargetRecallCheck()
{
	ClearTargetsMap(KnownItems, Bot->GetCharStats<UBotStatsComponent>()->GetTargetRecallSeconds());
	ClearTargetsMap(KnownEnemies, Bot->GetCharStats<UBotStatsComponent>()->GetEnemyRecallSeconds());
	ClearTargetsMap(KnownAllies, Bot->GetCharStats<UBotStatsComponent>()->GetAllyRecallSeconds());

	AddRecallCheckTimer();
	
}

void UBotBrainComponent::ClearTargetsMap(TMap<AActor*, float>& Map,  float RecallSeconds)
{
	if (Map.Num() <= 0) return;

	if (!Bot)return;

	if(!ensure(Bot->GetWorld() != nullptr))return;

	AGameStateBase* GameState = Bot->GetWorld()->GetGameState();

	if (!GameState) return;

	float currentSecs = GameState->GetServerWorldTimeSeconds();

	TArray<AActor*>Keys;
	Map.GetKeys(Keys);
	TMap<AActor*, float> CleanMap;

	for (int i = Keys.Num() -1; i >= 0; i--) 
	{
		if (!Keys[i]) continue;
		
		if(Cast<ITargeteable>(Keys[i]) && Cast<ITargeteable>(Keys[i])->IsDead()) continue;

		if (Map[Keys[i]] + RecallSeconds <= currentSecs) 
		{
			if (!this->IsA<UBTBrainComponent>())
			{
				if (HasValidBBTarget(BB_LAST_SEEN_TARGET) && BotBB->GetValueAsObject(BB_LAST_SEEN_TARGET)->GetUniqueID() == Keys[i]->GetUniqueID())
				{
					if (GetPriorityTarget() && GetPriorityTarget()->GetUniqueID() == BotBB->GetValueAsObject(BB_LAST_SEEN_TARGET)->GetUniqueID())
					{
						BotBB->ClearValue(BB_PRIORITY_TARGET);
					}

					BotBB->ClearValue(BB_LAST_SEEN_TARGET);
				}
			}
			
			if (Keys[i]->IsA<ABasePraiseCharacter>())
				Cast<ABasePraiseCharacter>(Keys[i])->OnNotifyDead.RemoveDynamic(Bot, &ABaseBotCharacter::HandleCharDeadNotify);

			Map.Remove(Keys[i]);
		}

		else CleanMap.Add(Keys[i], Map[Keys[i]]);
	}

	Map = CleanMap;
}

void UBotBrainComponent::SetShouldTarget(bool bValue)
{
	bShouldTarget = bValue;

	if (bShouldTarget)
		LastTargetUpdateTimestamp = GetTimeSinceGameStart();
}

void UBotBrainComponent::SetShouldUnblockPath(bool bValue)
{
	bShouldUnblockPath = bValue;

	if (!bShouldUnblockPath) return;

	if (!EnvQuerier) return;

	PauseBrain(true);

	TimeBlocked = 0;
	
	BotBB->SetValueAsBool(BB_IS_PATH_BLOCKED, true);
	BotBB->SetValueAsBool(BB_HAS_REACHED_TARGET, false);
	BotBB->SetValueAsBool(BB_HAS_VALID_LOCATION, false);

	if (FUtilities::IsValidVector(BotBB->GetValueAsVector(BB_PRIORITY_LOCATION)))
		BotBB->SetValueAsVector(BB_LAST_PRIORITY_LOCATION, BotBB->GetValueAsVector(BB_PRIORITY_LOCATION));

	FEnvQueryConfig QueryConfig = FEnvQueryConfig(Bot, EEnvQueryMode::PLANE, 360, 45, 1500, 500, 10.f, 100.f, false);

	UEnvQueryDistanceCheck* DistanceCheck = Cast< UEnvQueryDistanceCheck>(EnvQuerier->GetQueryCheck(EEnvQueryCheck::DISTANCE));

	if (DistanceCheck)
	{
		DistanceCheck->SetPreferClosestLocs(false); 
		QueryConfig.QueryChecks.Add(DistanceCheck);
	}
	
	bool bHasTarget = GetPriorityTarget() && Bot->CanSeeTarget(GetPriorityTarget());

	if (bHasTarget || (BotBB->GetValueAsBool(BB_LOCATION_OVER_TARGET) && FUtilities::IsValidVector(BotBB->GetValueAsVector(BB_PRIORITY_LOCATION))))
	{
		UEnvQueryDistanceCheck* DistanceToTargetCheck = Cast< UEnvQueryDistanceCheck>(EnvQuerier->GetQueryCheck(EEnvQueryCheck::DISTANCE));

		if (DistanceToTargetCheck)
		{
			if (bHasTarget)
				DistanceToTargetCheck->SetQuerier(GetPriorityTarget());

			else DistanceToTargetCheck->SetQueriedLocation(BotBB->GetValueAsVector(BB_PRIORITY_LOCATION));

			DistanceToTargetCheck->SetPreferClosestLocs(true);  
			QueryConfig.QueryChecks.Add(DistanceToTargetCheck);
		}
	}
	
	UEnvQueryVisibilityCheck* VisibilityCheck = Cast<UEnvQueryVisibilityCheck>(EnvQuerier->GetQueryCheck(EEnvQueryCheck::VISIBILITY));

	if (VisibilityCheck)
	{
		VisibilityCheck->SetIsFromQuerierEyesLoc(true);
		VisibilityCheck->SetQuerierHeightOffset(FVector(0, 0, 20.f));
		VisibilityCheck->SetPointHeightOffset(FVector(0, 0, 150.f));
		VisibilityCheck->SetPreferNonVisible(false); 
		VisibilityCheck->bDebug = false;
		QueryConfig.QueryChecks.Add(VisibilityCheck);
	}
	
	EnvQuerier->bDrawPoints = false;
	EnvQuerier->bDrawBestResults = false;

	QueryConfig.QueryConditions.Add([this](FVector PointLoc) { FVector ToLoc = PointLoc - Bot->GetActorLocation(); return FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Bot->GetActorForwardVector(), ToLoc.GetSafeNormal()))) > 80.f; });
	QueryConfig.QueryConditions.Add([this](FVector PointLoc) { return !UnblockPathLocs.Contains(PointLoc); });

	TArray<FVector> BestLocations;
	if (BeginEnvQuery(QueryConfig, BestLocations, 2))
	{
		FVector NewLocation;
		if (BotController->TryReachLocation(BestLocations, NewLocation, 50.f, false))
		{
			++UnblockPathRetries;
			
			if(!UnblockPathLocs.Contains(NewLocation))
				UnblockPathLocs.Add(NewLocation, GetTimeSinceGameStart());

			BotBB->SetValueAsVector(BB_PRIORITY_LOCATION, NewLocation);
			BotBB->SetValueAsBool(BB_HAS_VALID_LOCATION, true);
		}
	}
	else
	{
		if (this->IsA<UStateMachineBrainComponent>())
		{
			PauseBrain(true, true);
			UnblockPathRetries = 0;
			UnblockPathLocs.Empty();
			EWanderMode WanderMode = Bot->GetBotClass() == EBotClass::FIGHTER ? EWanderMode::INSPECT : EWanderMode::RANDOM;
			SetWanderMode(WanderMode, 15.f);
			Cast<UStateMachineBrainComponent>(this)->SwitchAIState(EMachineState::WANDER);
			PauseBrain(false);
			return;
		}
	}
}

void UBotBrainComponent::HandleBlockedPath(float DeltaTime)
{
	FVector UnblockLoc = BotBB->GetValueAsVector(BB_PRIORITY_LOCATION);

	if (!FUtilities::IsValidVector(UnblockLoc))
	{
		SetShouldUnblockPath(true);
		return;
	}

	FVector  ToUnblockLoc = UnblockLoc - Bot->GetActorLocation();

	if (ToUnblockLoc.Size() <= 150 || TimeBlocked >= MaxTimeBlocked)
	{
		if (UnblockPathRetries < MaxUnblockPathRetries)
		{
			BotBB->ClearValue(BB_PRIORITY_LOCATION);

			if (FUtilities::IsValidVector(BotBB->GetValueAsVector(BB_LAST_PRIORITY_LOCATION)))
				BotBB->SetValueAsVector(BB_PRIORITY_LOCATION, BotBB->GetValueAsVector(BB_LAST_PRIORITY_LOCATION));

			BotBB->SetValueAsBool(BB_HAS_REACHED_TARGET, true);
			BotBB->SetValueAsBool(BB_IS_PATH_BLOCKED, false);

			if (GetPriorityTarget()) {
				BotBB->SetValueAsBool(BB_HAS_REACHED_TARGET, false);
				BotController->MoveToLocation(Cast<AActor>(BotBB->GetValueAsObject(BB_PRIORITY_TARGET))->GetActorLocation(), 100.f, true, true);
			}
			else
			{
				if (BotBB->GetValueAsBool(BB_LOCATION_OVER_TARGET))
				{
					if (FUtilities::IsValidVector(BotBB->GetValueAsVector(BB_PRIORITY_LOCATION)))
					{
						BotBB->SetValueAsBool(BB_HAS_REACHED_TARGET, false);
						BotController->MoveToLocation(BotBB->GetValueAsVector(BB_PRIORITY_LOCATION));
					}
				}
			}

			PauseBrain(false);
		}
		else
		{
			if (this->IsA<UStateMachineBrainComponent>())
			{
				PauseBrain(true);
				UnblockPathRetries = 0;
				UnblockPathLocs.Empty();
				EMachineState NewState = EMachineState::WANDER;

				if (!HasValidBBTarget(BB_PRIORITY_TARGET))
				{
					EWanderMode WanderMode = Bot->GetBotClass() == EBotClass::FIGHTER ? EWanderMode::INSPECT : EWanderMode::RANDOM;
					SetWanderMode(WanderMode, 15.f);
				}

				else
				{
					FVector ToTarg = GetPriorityTarget()->GetActorLocation() - Bot->GetActorLocation();

					if (ToTarg.Size() > Bot->GetCharStats()->GetSightRange())
					{
						BotBB->SetValueAsObject(BB_TRAVEL_DESTINATION, GetPriorityTarget());
						BotBB->SetValueAsBool(BB_WANTS_TO_TRAVEL, true);
						NewState = EMachineState::TRAVEL;
					}
					else
					{
						EWanderMode WanderMode = Bot->GetBotClass() == EBotClass::FIGHTER ? EWanderMode::INSPECT : EWanderMode::RANDOM;
						SetWanderMode(WanderMode, 15.f);
					}
				}

				BotBB->SetValueAsBool(BB_IS_PATH_BLOCKED, false);
				Cast<UStateMachineBrainComponent>(this)->SwitchAIState(NewState);
			}

			PauseBrain(false);
		}
	}
	else
	{
		if (Bot->GetVelocity().Size() <= 0.f)
			TimeBlocked += DeltaTime;
	}
}

bool UBotBrainComponent::HasValidBBTarget(FName BBKey)
{
	if (!BotBB) return false;

	if (BotBB->GetValueAsObject(BBKey)) {
		AActor* BBTarg = Cast<AActor>(BotBB->GetValueAsObject(BBKey));
		if (!BBTarg || (!BotBB->GetValueAsBool(BB_DEAD_TARGETS_ALLOWED) && Cast<ITargeteable>(BBTarg) && Cast<ITargeteable>(BBTarg)->IsDead()))
		{
			BotBB->ClearValue(BBKey);
			return false;
		}
		return BBTarg != nullptr;
	}
	
	return false;
}

bool UBotBrainComponent::TryAddKnownItem(ITargeteable* NewItem)
{
	if (Cast<AActor>(NewItem)->IsA<AWeapon>())
	{
		AWeapon* Weapon = Cast<AWeapon>(NewItem);

		if (Weapon->HasOwner()) return false;
		
		if (Weapon->GetWeaponState() <= 10.f) return false;
	}

	return TryAddTarget(NewItem, KnownItems);
}

bool UBotBrainComponent::TryAddNewTarget(ITargeteable* Target)
{
	if (!Bot)return false;

	if (!ensure(Bot->GetWorld() != nullptr))return false;

	AGameStateBase* GameState = Bot->GetWorld()->GetGameState();

	if (!GameState) return false;

	if (!ensure(GameState != nullptr)) return false;	

	switch (Target->IsTargetType())
	{
		case(ETargetType::PLAYER):
		case(ETargetType::NPC):
			ProcessCharacter(Cast<ABasePraiseCharacter>(Target));
			break;
		case(ETargetType::ITEM):
			ProcessItem(Target);
			break;
		case(ETargetType::VIP):
			break;
	}

	return true;
}

bool UBotBrainComponent::TryRemoveKnownTarget(ITargeteable* Target)
{
	try
	{
		switch (Target->IsTargetType()) {
			case(ETargetType::PLAYER):
			case(ETargetType::NPC):
				return (TryRemoveTarget(Target, KnownEnemies) || TryRemoveTarget(Target, KnownAllies));
			case(ETargetType::ITEM):
				return TryRemoveTarget(Target, KnownItems);
			default:
				return false;
		}
	}
	catch(std::exception ex)
	{
		FLogger::LogTrace(__FUNCTION__ + FString(ex.what()));
	}
	
	return false;
}

bool UBotBrainComponent::TryRemoveTarget(ITargeteable* Target, TMap<AActor*, float>& Map)
{
	if (!Cast<AActor>(Target)) return false;
	TArray<AActor*> Keys;
	Map.GetKeys(Keys);
	for (AActor* Key : Keys)
		if (Key && Key->GetUniqueID() == Cast<AActor>(Target)->GetUniqueID())
			return Map.Remove(Cast<AActor>(Target)) > 0;
	
	return false;
}

bool UBotBrainComponent::TryAddTarget(ITargeteable* Target, TMap<AActor*, float>& Map)
{
	AGameStateBase* GameState = Bot->GetWorld()->GetGameState();

	if (!GameState) return false;
	TArray<AActor*>MapKeys;
	Map.GetKeys(MapKeys);

	if (Cast<AActor>(Target) && Map.Contains(Cast<AActor>(Target))) {						
		for (AActor* key : MapKeys) {
			if (key) {
				Map[key] = GameState->GetGameTimeSinceCreation();
			}
		}
	}
	else Map.Add(Cast<AActor>(Target), GameState->GetGameTimeSinceCreation());

	Map.ValueSort([](const float& A, const float& B) { return A > B; }); 

	return true;
}

bool UBotBrainComponent::TryAddCharTarget(ABasePraiseCharacter* Character, bool bIsEnemy)
{
	FVector ToTarget = Character->GetActorLocation() - Bot->GetActorLocation();
	bool bUpdateLastSeenTargetLoc = false;
	switch (bIsEnemy) {
		case(true):
			
			TryRemoveTarget(Cast<ITargeteable>(Character), KnownAllies);

			if (!Bot->IsEnemyTarget(Character) && !Bot->IsFriendTarget(Character)) 
			{
				int ExtraHits = 0;
				
				if (Cast<UStateMachineBrainComponent>(this) && Cast<UStateMachineBrainComponent>(this)->GetCurrentState() == EMachineState::COMBAT) 
					ExtraHits = Bot->GetNeutralCombatHitsTolerance();																													
				
				if (Bot->GetNeutralHitsTolerance() + ExtraHits > 0)
				{
					AddNeutralDamager(Character, ExtraHits);																							

					return false;
				}
			}

			bUpdateLastSeenTargetLoc = ToTarget.Size() <= DefaultPerceptionRadius ? Bot->CanSeeTarget(Character) : Bot->IsTargetInsideFOV(Character, true);

			if (bUpdateLastSeenTargetLoc && TryAddTarget(Character, KnownEnemies))
			{
				BotBB->SetValueAsObject(BB_LAST_SEEN_TARGET, Character); 
				BotBB->SetValueAsVector(BB_LAST_KNOWN_TARGET_LOC, Character->GetActorLocation());

				if(!Character->OnNotifyDead.IsBound())
					Character->OnNotifyDead.AddDynamic(Bot, &ABasePraiseCharacter::HandleCharDeadNotify);
				
				if (Bot->GetCharStatus() != ECharStatus::ALERTED)
					Bot->UpdateCharStatus(ECharStatus::ALERTED);

				AddRecallCheckTimer();
				AddLastKnownEnemyLocCheckTimer();
				return true;
			}
			
			return false;

		case(false):

			if (KnownEnemies.Contains(Character))
				return false;
			
			if (TryAddTarget(Character, KnownAllies))
			{
				AddRecallCheckTimer();
				return true;
			}
	}

	return false;
}

bool UBotBrainComponent::IsValidTarget(APawn* Target) const
{
	return Target->GetClass()->ImplementsInterface(UTargeteable::StaticClass()) &&
		!Cast<ITargeteable>(Target)->IsDead();
}

ITargeteable* UBotBrainComponent::GetClosestFoE(bool bEnemy, bool bAllowDeadTargets)
{
	TArray<AActor*> FoEs = TArray<AActor*>();
	switch (bEnemy)
	{
		case(true):
			FoEs.Append(GetKnownEnemies());
			break;
		case(false):
			FoEs.Append(GetKnownAllies());
			break;
	}
	
	if (FoEs.Num() <= 0) return nullptr;

	AActor* FoE = GetClosestTarget(FoEs, ECharVector::NONE, bAllowDeadTargets);

	return FoE ? Cast<ITargeteable>(FoE) : nullptr;
}

ITargeteable* UBotBrainComponent::GetClosestItem()
{
	return Cast<ITargeteable>(GetClosestTarget(GetKnownItems()));
}

void UBotBrainComponent::RegisterReceivedDamage(float Damage, int32 DamagerID)
{
	LastDamageReceivedTimeStamp = DamageReceivedTimeStamp;

	DamageReceivedTimeStamp = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

	if (RecentlyReceivedDamageMap.Contains(DamagerID))
		RecentlyReceivedDamageMap[DamagerID] += Damage;
	else
		RecentlyReceivedDamageMap.Add(DamagerID, Damage);

	if (TotalReceivedDamageMap.Contains(DamagerID))
		TotalReceivedDamageMap[DamagerID] += Damage;
	else
		TotalReceivedDamageMap.Add(DamagerID, Damage);

	RecentlyReceivedDamageMap.ValueSort([](const float& A, const float& B) { return A > B; });

	TotalReceivedDamageMap.ValueSort([](const float& A, const float& B) { return A > B; });

	BotBB->SetValueAsBool(BB_HAS_BEEN_DAMAGED, true);

	ProcessRecentDamage();
	ProcessTotalDamage();
	TryResetRecentDamageMap();
	AddRecallCheckTimer();
}

void UBotBrainComponent::ProcessRecentDamage()
{
	TArray<int> RecentDamageKeys;
	RecentlyReceivedDamageMap.GetKeys(RecentDamageKeys);
	
	for (AActor* KnownEnemyKey : GetKnownEnemies())
		if (KnownEnemyKey && KnownEnemyKey->GetUniqueID() == RecentDamageKeys[0])
			BotBB->SetValueAsObject(BB_MAX_AGGRO_TARG, KnownEnemyKey);
}

void UBotBrainComponent::ProcessTotalDamage()
{
	TArray<AActor*> KnownEnemyKeys;
	TArray<int> TotalDamagerIDs;
	KnownEnemies.GetKeys(KnownEnemyKeys);
	TotalReceivedDamageMap.GetKeys(TotalDamagerIDs);

	for (AActor* Key : KnownEnemyKeys)
		if (Key && Key->GetUniqueID() == TotalDamagerIDs[0])
			BotBB->SetValueAsObject(BB_MAX_DMG_TARG, Key);
}
