// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaseMachineState.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../../AI/BotDecisions/LocoDecision.h"
#include "../../AI/BotDecisions/EngageCombatDecision.h"

UChaseMachineState::UChaseMachineState() :Super()
{
	bIsTimeLimited = true;
	TotalTimeInCurrentState = 0;
	TimeInCurrentState = 0;
	StoppingDistance = 150.f;
	TriggerDistance = 500.f;
}

void UChaseMachineState::AddStateDecisions()
{
	TryAddStateDecision(EBotDecision::SELECT_TARGET, EBotDecision::CHASE_TARGET);
	
	UEngageCombatDecision* EngageCombatDecision = Cast<UEngageCombatDecision>(TryAddStateDecision(EBotDecision::ENGAGE_COMBAT));

	if (EngageCombatDecision)
		EngageCombatDecision->SetOnlyInCombatRange(true);

	UBooleanDecision* UpdatePathDecision = Cast<UBooleanDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::BOOLEAN, this));

	if (UpdatePathDecision && UpdatePathDecision->HasValidBrain()) {
		UpdatePathDecision->SetupDecision([this]() { return ShouldUpdateLocation(); }, [this](const bool bValue) { UpdateTargetLocation(bValue); }, /*bCallbackResult:*/ true);
		StateDecisions.Add(UpdatePathDecision);
	}
}

bool UChaseMachineState::OnEnterState(ABaseBotController* Controller)
{	
	if (!Super::OnEnterState(Controller)) return false;

	NavMesh = UNavigationSystemV1::GetCurrent(Bot->GetWorld());

	if (!NavMesh)
		return false;


	bIsTimeLimited = true;
	bCheckBlockedPath = true;

	MaxTimeInCurrentState = StateMachine->GetBotBB()->GetValueAsFloat(BB_MAX_CHASING) == 0.f ? BB_GLOBAL_MAX_CHASING_VAL :
							StateMachine->GetBotBB()->GetValueAsFloat(BB_MAX_CHASING);
	
	StateMachine->GetBotBB()->SetValueAsBool(BB_ONLY_ENEMY_TARGETS_ALLOWED, true);
	
	ChaseMode = (EChaseMode)StateMachine->GetBotBB()->GetValueAsInt(BB_CHASING_MODE);

	if (ChaseMode == EChaseMode::PURSUE)
		StateMachine->GetBotBB()->SetValueAsBool(BB_LOCATION_OVER_TARGET, true);
	
	StoppingDistance = 150.f;
	TriggerDistance = 500.f;
	StateMachine->GetBotBB()->ClearValue(BB_HAS_REACHED_TARGET);
	StateMachine->GetBotBB()->ClearValue(BB_HAS_VALID_LOCATION);
	StateMachine->GetBotBB()->SetValueAsBool(BB_SELECT_CLOSEST_FOE, StateMachine->GetBotBB()->GetValueAsBool(BB_LOCATION_OVER_TARGET) ? false : true);

	bCanMoveToLocation = true;
	CanReachAttempts = 3;
	UpdatePathSeconds = 1.f;
	UpdatePathTimestamp = 0;

	if (Bot->GetTargetingComponent())
	{
		Bot->GetTargetingComponent()->SetIsAutoTarget(true);
		Bot->GetTargetingComponent()->SetCheckFOV(false);
		Bot->GetTargetingComponent()->SetCheckCollision(false);
	}

	bDidInit = true;

	return bDidInit;
}

void UChaseMachineState::OnExitState()
{
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, true);
	StateMachine->GetBotBB()->SetValueAsBool(BB_ONLY_ENEMY_TARGETS_ALLOWED, true); 
	StateMachine->GetBotBB()->SetValueAsBool(BB_SELECT_CLOSEST_FOE, false);

	if (ChaseMode == EChaseMode::PURSUE)
		StateMachine->GetBotBB()->SetValueAsBool(BB_LOCATION_OVER_TARGET, false);
	
	Bot->UpdateCharStatus(ECharStatus::NORMAL);

	Super::OnExitState();

	if (Bot->GetTargetingComponent())
	{
		Bot->GetTargetingComponent()->SetIsAutoTarget(false);
		Bot->GetTargetingComponent()->SetCheckFOV(true);
		Bot->GetTargetingComponent()->SetCheckCollision(true);
	}
}

void UChaseMachineState::RunState()
{
	if (!FUtilities::IsValidVector(GetTargetLocation())) return;

	FVector TargetLocation = GetTargetLocation();

	ToTarget = TargetLocation - Bot->GetActorLocation();

	bUsePathFinding = ToTarget.Size() <= PathFindingDistance;

	CurrentTargetLocation = TargetLocation;

	bCanReachTarget = !BotController->GetPathFollowingComponent()->HasPartialPath();

	if (FUtilities::IsValidVector(CurrentTargetLocation))
	{
		FVector ToNavLoc = CurrentTargetLocation - Bot->GetActorLocation();

		StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, ToNavLoc.Size() > 0 && ToNavLoc.Size() <= StoppingDistance); 
	}
	
	Super::RunState();
}

bool UChaseMachineState::ShouldRun()
{
	if(!Bot->HasMinActionStam(ECharAction::RUN)) return false;
	
	switch (ChaseMode)
	{
		case(EChaseMode::FOLLOW):
			return FUtilities::IsValidVector(ToTarget) && !StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_REACHED_TARGET) && ((ToTarget.Size() - StoppingDistance * 2.f) > (TriggerDistance * 60 / 100));
		case(EChaseMode::SEEK):
			if (FUtilities::IsValidVector(ToTarget) && ToTarget.Size() <= 4000) return true;

			return GetChasedTarget() && Bot->IsTargetInsideFOV(GetChasedTarget(), true);
		case(EChaseMode::PURSUE):
			if (Bot->GetCharStatus() == ECharStatus::ALERTED) return true;

			return GetChasedTarget() && Bot->IsTargetInsideFOV(GetChasedTarget(), true); 
		default:
			break;
	}
	return false;
}

bool UChaseMachineState::ShouldEquip()
{
	if (!Bot) return false;

	if (!Bot->HasWeapon()) return false;

	if (GetChasedTarget() && FUtilities::IsValidVector(ToTarget))
	{
		return ToTarget.Size() <= 1500.f ? true : StateMachine->GetKnownEnemies().Num() > 0;
	}
	
	return Bot->IsWeaponEquiped() && StateMachine->GetKnownEnemies().Num() > 0;
}

bool UChaseMachineState::ShouldTarget()
{
	if (StateMachine->GetPriorityTarget())
	{
		AActor* CurrentTarget = StateMachine->GetPriorityTarget();

		if (CurrentTarget && !StateMachine->GetBot<ABaseBotCharacter>()->CanSeeTarget(CurrentTarget, true)) return false;

		if (CurrentTarget && CurrentTarget != StateMachine->GetPriorityTarget() && Bot->IsWeaponEquiped() && StateMachine->GetPriorityTarget() && ToTarget.Size() <= StoppingDistance * 5.f)
			return true;
	
		return Bot->IsWeaponEquiped() && CurrentTarget && ToTarget.Size() <= StoppingDistance * 5.f;
	}

	else return false;
}


bool UChaseMachineState::ShouldBackToIdle()
{
	if (Super::ShouldBackToIdle()) return true;

	if (!StateMachine->GetBotBB()->GetValueAsBool(BB_LOCATION_OVER_TARGET))
	{
		if (ChaseMode != EChaseMode::PURSUE && !GetChasedTarget()) return true;

		if (GetChasedTarget() && GetChasedTarget()->Implements<UTargeteable>() && Cast<ITargeteable>(GetChasedTarget())->IsDead())
		{
			return true;
		}
	}
	else
	{
		if (ChaseMode == EChaseMode::PURSUE)
			if (!FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION)))
				if (!FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_LAST_KNOWN_TARGET_LOC)))
					return true;
	}

	if (bIsTimeLimited && TimeInCurrentState >= MaxTimeInCurrentState)
	{
		return true;
	}

	return false;
}



AActor* UChaseMachineState::GetChasedTarget()
{
	return StateMachine->GetPriorityTarget();
}

FVector UChaseMachineState::GetTargetLocation()
{
	FVector Loc;

	if (StateMachine->GetBotBB()->GetValueAsBool(BB_LOCATION_OVER_TARGET))
		Loc = StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION);

	else Loc = GetChasedTarget() ? GetChasedTarget()->GetActorLocation() : FVector::ZeroVector;

	FVector ToLoc = ToLoc - Bot->GetActorLocation();
	
	return Loc;
}

float UChaseMachineState::GetRandomizedStoppingDistance()
{
	float Range = (StoppingDistance * 25 / 100);

	return FMath::RandRange(StoppingDistance - Range, StoppingDistance + Range );
}

bool UChaseMachineState::ShouldUpdateLocation()
{
	if (StateMachine->GetBotBB()->GetValueAsBool(BB_IS_PATH_BLOCKED)) return false;
	
	if (LastTickSeconds > 0 && StateMachine->GetTimeSinceGameStart() - UpdatePathTimestamp <= UpdatePathSeconds) return false;

	if (!StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_VALID_LOCATION)) return true;

	FVector ToTargetProjected = ToTarget;
	
	ToTargetProjected.Z = 0;

	float AngleToTarget = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Bot->GetActorForwardVector(), ToTargetProjected.GetSafeNormal())));
	
	if (!StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_REACHED_TARGET) && AngleToTarget >= 20.f) {
		return true;
	}
		
	switch (ChaseMode)
	{
		case(EChaseMode::FOLLOW):
			if (ToTarget.Size() > TriggerDistance && StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_REACHED_TARGET) && bCanMoveToLocation)
				return true;

			if (ToTarget.Size() > (TriggerDistance * 2))
				if (Bot->GetVelocity().Size() <= 0.f)
					return true;
			break;
		case(EChaseMode::SEEK):
			return true;
		case(EChaseMode::PURSUE):

			StoppingDistance = 200.f;
			if (StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_REACHED_TARGET))
			{
				StateMachine->GetBotBB()->SetValueAsInt(BB_WANDERING_MODE, (int)EWanderMode::INSPECT);
				StateMachine->GetBotBB()->SetValueAsFloat(BB_MAX_WANDERING, 60.f);
				StateMachine->GetBotBB()->SetValueAsFloat(BB_WANDERING_RADIUS, 1500.f);
				StateMachine->SwitchAIState(EMachineState::WANDER);
				return false;
			}

			if (GetChasedTarget()) return !Bot->IsBeingDamaged();

			break; 
		default:
			break;
	}
	

	return false;
}

void UChaseMachineState::UpdateTargetLocation(bool bAbortIfPartial)
{
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_VALID_LOCATION,  false);
	
	if (!FUtilities::IsValidVector(CurrentTargetLocation)) return;
	bool bHasValidLocation = false;
	FNavLocation NavLocation;
	if (NavMesh->ProjectPointToNavigation(CurrentTargetLocation, NavLocation))			
	{
		bHasValidLocation = true;
	}
	else 
	{
		bHasValidLocation = NavMesh->GetRandomReachablePointInRadius(CurrentTargetLocation, 500.f, NavLocation);
	}
	
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_VALID_LOCATION, bHasValidLocation);

	if (bHasValidLocation)
	{
		CurrentNavLoc = NavLocation.Location;

		StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, false);
		
		bCanMoveToLocation = false;
		
		BotController->MoveToLocation(CurrentNavLoc, 100.f, true, bUsePathFinding);
		
		if (BotController->GetPathFollowingComponent()->HasPartialPath() && bAbortIfPartial)
		{
			StateMachine->GetBotBB()->SetValueAsBool(BB_CURRENT_PATH_IS_PARTIAL, true);
		}

		else StateMachine->GetBotBB()->SetValueAsBool(BB_CURRENT_PATH_IS_PARTIAL, false);
		
		UpdatePathTimestamp = StateMachine->GetTimeSinceGameStart();
	}
	
}

void UChaseMachineState::HandleChaseModeMovement()
{
	FVector ZeroedTargetLoc = CurrentTargetLocation - Bot->GetActorLocation();
	ZeroedTargetLoc.Z = 0;
	switch (ChaseMode)
	{
		case(EChaseMode::FOLLOW):
			if (ZeroedTargetLoc.Size() <= StoppingDistance * 1.5f)
				StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, true);
			if (ToTarget.Size() > TriggerDistance)
				bCanMoveToLocation = true;
			break;
		case(EChaseMode::SEEK):
			if (ZeroedTargetLoc.Size() <= StoppingDistance * 1.5f)
				StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, true);
			break;
		default:
			break;
	}
}

bool UChaseMachineState::CanReachTarget(int Attempts)
{
	bool bHasValidPath = false;
	FNavLocation NavLocation;
	for(int i = 0; i < Attempts; i++)
	{
		if (NavMesh->ProjectPointToNavigation(CurrentTargetLocation, NavLocation) || GetTargetLocation() != FVector::ZeroVector && NavMesh->ProjectPointToNavigation(GetTargetLocation(), NavLocation))	
			return true;
		
	}
	return false;
}

bool UChaseMachineState::CheckQueryPointMaxDistance(FVector Point, float MaxDistance)
{
	FVector TargetLoc = StateMachine->GetBotBB()->GetValueAsBool(BB_LOCATION_OVER_TARGET) ? 
						StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION) : GetChasedTarget() ?
						GetChasedTarget()->GetActorLocation() : FVector::ZeroVector;

	if (!FUtilities::IsValidVector(TargetLoc)) return false;

	FVector ToTarg = Point - TargetLoc;

	return FUtilities::IsValidVector(TargetLoc) ? (Point - TargetLoc).Size() <= MaxDistance : false;
}

