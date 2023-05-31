// Fill out your copyright notice in the Description page of Project Settings.


#include "WanderMachineState.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../../Characters/AI/BotCharacters/FightingBotCharacter.h"
#include "../../Characters/AI/BotCharacters/SeekerBotCharacter.h"
#include "../../Components/Actor/AI/T200BrainComponent.h"
#include "Gameframework/GameStateBase.h"

UWanderMachineState::UWanderMachineState() : Super()
{
	bIsTimeLimited = true;

	MaxTimeInCurrentState = 60.f;
	LocationWaitingTime = 5.f;
	TimeInCurrentState = 0;
	TotalTimeInCurrentState = 0;
}

void UWanderMachineState::AddStateDecisions()
{
	TryAddStateDecision(EBotDecision::STAY_IN_IDLE);
	TryAddStateDecision(EBotDecision::RUN_AWAY);

	if (StateMachine->IsA<UT200BrainComponent>())
		TryAddStateDecision(EBotDecision::SEEK);

	if (Bot->IsA<AFightingBotCharacter>())
		TryAddStateDecision(EBotDecision::SELECT_TARGET, EBotDecision::ENGAGE_COMBAT);
}

bool UWanderMachineState::OnEnterState(ABaseBotController* NewBotController)
{
	if(!Super::OnEnterState(NewBotController)) return false;
	
	NavMesh = UNavigationSystemV1::GetCurrent(BotController->GetWorld());

	if (!NavMesh) 
		return false;
		
	LocationsToEndPatrol = 3;
	VisitedLocationsIdx = 0;
	LocationWaitingTime = 5.f;
	StoppingDistance = 150.f;
	Bot->SetTurn(false);

	bIsTimeLimited = true;
	MaxTimeInCurrentState = StateMachine->GetBotBB()->GetValueAsFloat(BB_MAX_WANDERING);
	WanderMode = (EWanderMode)StateMachine->GetBotBB()->GetValueAsInt(BB_WANDERING_MODE);
	MaxRadius = StateMachine->GetBotBB()->GetValueAsFloat(BB_WANDERING_RADIUS);

	if (WanderMode == EWanderMode::INSPECT)
	{
		FVector Loc = FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION)) ?
			StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION) :
			Bot->GetActorLocation();

		StateMachine->GetBotBB()->SetValueAsVector(BB_LAST_PRIORITY_LOCATION, Loc);
	}

	StateMachine->GetBotBB()->SetValueAsVector(BB_LAST_PRIORITY_LOCATION, Bot->GetActorLocation());

	SetNewRandomLocation();

	if (FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION)))
		MoveToNextLocation();

	bDidInit = true;

	return bDidInit;
}

void UWanderMachineState::OnExitState()
{

	if (bIsTimeLimited) {
		StateMachine->GetBotBB()->SetValueAsFloat(BB_TOTAL_WANDERING, TotalTimeInCurrentState);
		if (TimeInCurrentState >= MaxTimeInCurrentState)
			StateMachine->GetBotBB()->SetValueAsBool(BB_CAN_WANDER, false);
	}

	BotController->MoveTo(Bot->GetActorLocation());
	Bot->UpdateCharStatus(ECharStatus::NORMAL);

	Super::OnExitState();
}

void UWanderMachineState::RunState()
{
	if (!StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_REACHED_TARGET))
		if (HasReachedTarget())
			OnTargetReached();
	
	Super::RunState();
}

void UWanderMachineState::SetNewRandomLocation()
{
	FVector Origin = WanderMode == EWanderMode::INSPECT && FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_LAST_PRIORITY_LOCATION)) ? 
		             StateMachine->GetBotBB()->GetValueAsVector(BB_LAST_PRIORITY_LOCATION) : Bot->GetActorLocation();
	
	FNavLocation NewLocation;
	
	if (NavMesh->GetRandomReachablePointInRadius(Origin, MaxRadius, NewLocation) && NewLocation.Location != FVector::ZeroVector) {

		StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, false);
		StateMachine->GetBotBB()->SetValueAsVector(BB_PRIORITY_LOCATION, NewLocation.Location);
		VisitedLocationsIdx = ++VisitedLocationsIdx;
		
		FVector ToTarget = NewLocation.Location - Bot->GetActorLocation();
		CurrentPathSize =  ToTarget.Size();
		
		if (CurrentPathSize != 0.f) {
			StateMachine->GetBotBB()->SetValueAsVector(BB_ORIGINAL_DIST_TO, ToTarget);
		}
	}
}

void UWanderMachineState::OnTargetReached()
{
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, true);
	StateMachine->GetBotBB()->ClearValue(BB_PRIORITY_LOCATION);
	
	switch (WanderMode)
	{
		case(EWanderMode::INSPECT):
			
			Bot->GetWorld()->GetTimerManager().SetTimer(OnTargetReachedWaitingTimer, this, &UWanderMachineState::MoveToNextLocation, GetWaitingTime(), false);
			break;
		case(EWanderMode::RANDOM):

			if (WanderMode == EWanderMode::RANDOM)
				StateMachine->GetBotBB()->SetValueAsVector(BB_LAST_PRIORITY_LOCATION, Bot->GetActorLocation());

			if (FMath::RandRange(0, 100) >= 80)
				Bot->GetWorld()->GetTimerManager().SetTimer(OnTargetReachedWaitingTimer, this, &UWanderMachineState::MoveToNextLocation, GetWaitingTime(), false);

			else MoveToNextLocation();
			
			break;
		default:
			break;
		
	}	
}


FVector UWanderMachineState::GetTargetLocation(bool bCurrentTarget)
{
	return bCurrentTarget ? StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION) : StateMachine->GetBotBB()->GetValueAsVector(BB_LAST_PRIORITY_LOCATION);
}

bool UWanderMachineState::HasReachedTarget()
{
	if (GetTargetLocation() == FVector::ZeroVector) return true;

	FVector ToTarget = GetTargetLocation() - Bot->GetActorLocation();

	float Distance = ToTarget.Size();

	if (Distance >= MaxRadius) return true; 

	return Distance <= StoppingDistance;
}


bool UWanderMachineState::ShouldBackToIdle()
{
	if (Bot->IsA<ASeekerBotCharacter>() && !Cast<ASeekerBotCharacter>(Bot)->ShouldWanderIfNoTargets()) return true;

	return bIsTimeLimited && TimeInCurrentState >= MaxTimeInCurrentState;
}

void UWanderMachineState::MoveToNextLocation()
{
	if(StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_REACHED_TARGET) && !FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION)))
		SetNewRandomLocation();

	if(!FUtilities::IsValidVector(GetTargetLocation())) return;

	BotController->MoveToLocation(GetTargetLocation());

	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, false);
}

float UWanderMachineState::GetWaitingTime(bool bExactTime)
{
	if (!bExactTime) {
		int rnd = FMath::FRandRange(-1, 1);
		return LocationWaitingTime + (LocationWaitingTime * 25 / 100) * (rnd == 0 ? 1 : rnd);
	}

	return LocationWaitingTime;
}

