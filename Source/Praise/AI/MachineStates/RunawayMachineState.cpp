// Fill out your copyright notice in the Description page of Project Settings.


#include "RunawayMachineState.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../../Components/GameMode/MsgCommandsFactoryComponent.h"
#include "../../Enums/AI/EAI_Msg.h"
#include "../../Characters/AI/BotCharacters/CitizenBotCharacter.h"
#include "../MsgCommands/BackupRequestMsgCommand.h"
#include "../../AI/BotDecisions/LocoDecision.h"

URunawayMachineState::URunawayMachineState() : Super()
{
	MaxRunawayRange = 7500.f;
	BackupReqRange = 2500.f;
	BackupReqSeconds = 10.f;
	LastBackupReqTimestamp = 0;

	UpdatePathSeconds = 2.f;

	FindCoverSpotSeconds = 5.f;
	LastFindCoverSpotTimestamp = 0;
	bIsHiding = false;
	bCanHide = false;
	bIsMovingToCoverSpot = false;
}

void URunawayMachineState::AddStateDecisions()
{
	Super::AddStateDecisions();

	TryAddStateDecision(EBotDecision::SELECT_TARGET);

	UBooleanDecision* MoveToSpawnLocDecision = Cast<UBooleanDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::BOOLEAN, this));

	if (MoveToSpawnLocDecision && MoveToSpawnLocDecision->HasValidBrain()) {
		MoveToSpawnLocDecision->SetupDecision([this]() { return ShouldMoveToSpawnLoc(); }, [this](const bool bValue) { UpdatePath(bValue); }, /*bCallbackResult:*/ true);
		StateDecisions.Add(MoveToSpawnLocDecision);
	}

	UBooleanDecision* FindCoverSpotDecision = Cast<UBooleanDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::BOOLEAN, this));

	if (FindCoverSpotDecision && FindCoverSpotDecision->HasValidBrain()) {
		FindCoverSpotDecision->SetupDecision([this]() { return ShouldFindCoverSpot(); }, [this](const bool bValue) { UpdatePath(bValue); }, /*bCallbackResult:*/ false);
		StateDecisions.Add(FindCoverSpotDecision);
	}

	UBooleanDecision* UpdatePathDecision = Cast<UBooleanDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::BOOLEAN, this));

	if (UpdatePathDecision && UpdatePathDecision->HasValidBrain()) {
		UpdatePathDecision->SetupDecision([this]() { return ShouldUpdatePath(); }, [this](const bool bValue) { UpdatePath(bValue); }, /*bCallbackResult:*/ false);
		StateDecisions.Add(UpdatePathDecision);
	}

	if (Bot->IsA<ACitizenBotCharacter>())
	{
		UBooleanDecision* BackupReqDecision = Cast<UBooleanDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::BOOLEAN, this));

		if (BackupReqDecision && BackupReqDecision->HasValidBrain()) {
			BackupReqDecision->SetupDecision([this]() { return ShouldRequestBackup(); }, [this](const bool bValue) { SendBackupRequest(); }, /*bCallbackResult:*/ true);
			StateDecisions.Add(BackupReqDecision);
		}

		TryAddStateDecision(EBotDecision::SNITCH);
	}

	TryAddStateDecision(EBotDecision::STAY_IN_IDLE);
}

bool URunawayMachineState::OnEnterState(ABaseBotController* NewBotController)
{	
	if(!Super::OnEnterState(NewBotController)) return false;

	NavMesh = UNavigationSystemV1::GetCurrent(BotController->GetWorld());

	if (!NavMesh) 
		return false;
	
	SafeDistance = 1000.f;

	bIsTimeLimited = true;

	TrySetPriorityLocation();
	
	MaxRunawayRange = 10000.f;
	MaxTimeInCurrentState = StateMachine->GetBotBB()->GetValueAsFloat(BB_MAX_RUNAWAY);
	MinRunawayTime = StateMachine->GetBotBB()->GetValueAsFloat(BB_MIN_RUNAWAY);

	if (MaxTimeInCurrentState <= 0.f)
		MaxTimeInCurrentState = BB_GLOBAL_MAX_RUNAWAY_VAL;

	if (MinRunawayTime <= 0.f)
		MinRunawayTime = BB_GLOBAL_MIN_RUNAWAY_VAL;

	BackupReqRange = 2500.f;
	BackupReqSeconds = 10.f;
	LastBackupReqTimestamp = 0;
	
	UpdatePathSeconds = 2.f;
	
	FindCoverSpotSeconds = 5.f;
	LastFindCoverSpotTimestamp = 0;
	bIsHiding = false;
	bCanHide = false;
	bIsMovingToCoverSpot = false;

	Bot->UpdateCharStatus(ECharStatus::ALERTED);

	StateMachine->GetBotBB()->SetValueAsBool(BB_LOCATION_OVER_TARGET, HasActorToAvoid() ? false : true);
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, false);
	StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_VALID_LOCATION, false);
	bDidInit = true;
	
	return bDidInit;
}


void URunawayMachineState::OnExitState()
{
	Super::OnExitState();

	StateMachine->SetShouldRun(false);
	StateMachine->GetBotBB()->SetValueAsBool(BB_LOCATION_OVER_TARGET, false);
	StateMachine->GetBotBB()->ClearValue(BB_LAST_KNOWN_TARGET_LOC);
	StateMachine->GetBotBB()->ClearValue(BB_COVER_SPOT);
	Bot->UpdateCharStatus(ECharStatus::NORMAL);
}

void URunawayMachineState::RunState()
{
	StateMachine->GetBotBB()->SetValueAsBool(BB_LOCATION_OVER_TARGET, HasActorToAvoid() ? false : true);

	if (HasReachedTarget())
	{
		StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, true);

		if (bIsMovingToCoverSpot)
		{
			bIsMovingToCoverSpot = false;
			bIsHiding = true;

			if (Bot->HasTargetsAtSight(Bot->GetActorLocation(), true, 2000.f))
			{
				bIsHiding = false;

				if (TryFindCoverSpot())
				{
					bIsHiding = true;

					UpdatePath();
					return;
				}
			}

		}

	}

	Super::RunState();
}



bool URunawayMachineState::ShouldRun()
{
	if (!Bot->HasMinActionStam(ECharAction::RUN)) return false;

	if (bIsMovingToCoverSpot) return true;

	AActor* Avoided = GetAvoidedActor();

	if (!Avoided) return false;

	FVector ToTarget = Avoided->GetActorLocation() - Bot->GetActorLocation();

	return ToTarget.Size() <= GetSafeDistance()  || StateMachine->GetBotBB()->GetValueAsBool(BB_HAS_BEEN_DAMAGED) || FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_LAST_KNOWN_TARGET_LOC));
}

bool URunawayMachineState::ShouldBackToIdle()
{
	return StateMachine->GetKnownEnemies().Num() <= 0 && TimeInCurrentState > MinRunawayTime;
}


bool URunawayMachineState::TryFindCoverSpot()
{
	StateMachine->GetBotBB()->ClearValue(BB_COVER_SPOT);
	bCanHide = false;
	
	FEnvQueryConfig QueryConfig = FEnvQueryConfig(Bot, EEnvQueryMode::CYLINDRIC, 360, 45, 2000, 400, 10.f, 100.f, false);

	QueryConfig.CylinderRingPairs = 1;
	QueryConfig.CylinderRingsHeight = 200.f;

	UEnvQueryDistanceCheck* DistanceCheck = Cast< UEnvQueryDistanceCheck>(StateMachine->GetEnvQuerier()->GetQueryCheck(EEnvQueryCheck::DISTANCE));

	if (DistanceCheck)
	{
		DistanceCheck->SetPreferClosestLocs(true); 
		QueryConfig.QueryChecks.Add(DistanceCheck);
	}

	UEnvQueryVisibilityCheck* VisibilityCheck = Cast<UEnvQueryVisibilityCheck>(StateMachine->GetEnvQuerier()->GetQueryCheck(EEnvQueryCheck::VISIBILITY));

	if (VisibilityCheck)
	{
		VisibilityCheck->SetPointHeightOffset(FVector(0, 0, 150.f));
		VisibilityCheck->SetIsFromQuerierEyesLoc(true);
		VisibilityCheck->SetIsExclusiveCheck(true);
		VisibilityCheck->SetIsObjectQuery(false);

		if(HasActorToAvoid())
			VisibilityCheck->SetCheckedActor(GetAvoidedActor());
		
		VisibilityCheck->SetPreferNonVisible(true);

		QueryConfig.QueryChecks.Add(VisibilityCheck);
	}

	StateMachine->GetEnvQuerier()->bDrawBestResults = false;

	TArray<FVector> BestLocations;
	if (StateMachine->BeginEnvQuery(QueryConfig, BestLocations, 2))
	{
		FVector NewLocation;
		if (BotController->TryReachLocation(BestLocations, NewLocation, 50.f, /*bWorstLocFirst: */ true))
		{
			StateMachine->GetBotBB()->SetValueAsVector(BB_COVER_SPOT, NewLocation);

			if (!IsSafeCoverSpot()) {
				StateMachine->GetBotBB()->ClearValue(BB_COVER_SPOT);
				return false;
			}

			bIsMovingToCoverSpot = true;
			
			return true;
		}
	}

	return false;
}

AActor* URunawayMachineState::GetAvoidedActor()
{
	ITargeteable* Targeteable = StateMachine->GetClosestFoE(true);

	if (!Targeteable) return nullptr;

	if (!Cast<AActor>(Targeteable)) return nullptr;

	return Cast<AActor>(Targeteable); 
}

bool URunawayMachineState::HasActorToAvoid()
{
	return GetAvoidedActor() != nullptr;
}

FVector URunawayMachineState::GetAvoidedLocation()
{
	return FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION)) ?
		StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION) :
		StateMachine->GetBotBB()->GetValueAsVector(BB_SPAWN_LOC);
}

bool URunawayMachineState::TrySetTargetLocation(bool bMoveToSpawnLoc)
{
	if (bMoveToSpawnLoc)
	{
		TargetLocation = StateMachine->GetBotBB()->GetValueAsVector(BB_SPAWN_LOC);
		return FUtilities::IsValidVector(TargetLocation);
	}

	if (bCanHide)
	{
		TargetLocation = StateMachine->GetBotBB()->GetValueAsVector(BB_COVER_SPOT);
		return FUtilities::IsValidVector(TargetLocation);
	}

	TargetLocation = HasActorToAvoid() ? GetRandomOpposedLocation(GetAvoidedActor()->GetActorLocation()) : GetRandomOpposedLocation(GetAvoidedLocation());
	
	return FUtilities::IsValidVector(TargetLocation);
}

bool URunawayMachineState::TrySetPriorityLocation()
{
	if (FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION))) return true;	
	
	if (FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_LAST_KNOWN_TARGET_LOC)))			
		StateMachine->GetBotBB()->SetValueAsVector(BB_PRIORITY_LOCATION, StateMachine->GetBotBB()->GetValueAsVector(BB_LAST_KNOWN_TARGET_LOC));

	if (!FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION)))				
		if (FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_LAST_DAMAGER_TO_BOT_VECTOR)))
			StateMachine->GetBotBB()->SetValueAsVector(BB_PRIORITY_LOCATION, StateMachine->GetBotBB()->GetValueAsVector(BB_LAST_DAMAGER_TO_BOT_VECTOR));
	
	return FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION));
}

bool URunawayMachineState::TargetCanSeePoint(AActor* Target, const FVector& PointLocation)
{
	bool bHasHit = false;

	FCollisionQueryParams ColParams;
	ColParams.AddIgnoredActor(Bot);

	FVector OriginLoc = Target->IsA<APawn>() ? Cast<APawn>(Target)->GetPawnViewLocation() : Target->GetActorLocation();

	FVector FinalLocation = PointLocation + FVector(0, 0, 100);
	FVector TraceOrigin = OriginLoc + Target->GetActorForwardVector() * 50.f;
	FVector QuerierToPoint = FinalLocation - TraceOrigin;
	FVector TraceEnd = TraceOrigin + (QuerierToPoint.GetSafeNormal() * QuerierToPoint.Size());
	FHitResult HitResult;

	bool bResult = Bot->GetWorld()->LineTraceSingleByChannel(HitResult, TraceOrigin, TraceEnd, ECC_GameTraceChannel4, ColParams) ? false : true;

	return bResult;
}

bool URunawayMachineState::ShouldMoveToSpawnLoc()
{
	FVector ToSpawnLoc = Bot->GetSpawnLocation() - Bot->GetActorLocation();

	return ToSpawnLoc.Size() > MaxRunawayRange;
}

bool URunawayMachineState::ShouldUpdatePath()
{
	if (bIsMovingToSpawnLoc) return false; 

	if (bCanHide && !bIsMovingToCoverSpot) return true; 

	if (StateMachine->GetTimeSinceGameStart() - LastPathUpdateTimestamp < UpdatePathSeconds) return false;

	if (bIsHiding)
	{
		return HasActorToAvoid();
	}

	if (Bot->GetVelocity().Size() <= 0) return true; 

	if (!FUtilities::IsValidVector(TargetLocation)) return true; 
	
	bool bHasActorToAvoid = HasActorToAvoid();

	FVector ToNewLoc = TargetLocation - Bot->GetActorLocation();
	FVector LocationToAvoid = bHasActorToAvoid ? GetAvoidedActor()->GetActorLocation() : GetAvoidedLocation();
	FVector ToAvoided = LocationToAvoid - Bot->GetActorLocation();

	if (bHasActorToAvoid && ToAvoided.Size() < SafeDistance / 2.f) return true; 

	if (bHasActorToAvoid && (TargetLocation - GetAvoidedActor()->GetActorLocation()).Size() <= SafeDistance) return true; 

	return false;
}

bool URunawayMachineState::ShouldFindCoverSpot()
{
	if (LastTickSeconds > 0 && StateMachine->GetTimeSinceGameStart() - LastFindCoverSpotTimestamp <= FindCoverSpotSeconds) return false;
	
	LastFindCoverSpotTimestamp = StateMachine->GetTimeSinceGameStart();
	
	if (bIsHiding)
	{
		if (HasActorToAvoid() || Bot->HasTargetsAtSight(StateMachine->GetBotBB()->GetValueAsVector(BB_COVER_SPOT), true, 2000.f))
		{
			bIsHiding = false;
				
			if (TryFindCoverSpot())
			{
				bCanHide = true;
				return true;
			}
		}
	}
	else
	{
		if (bIsMovingToCoverSpot && IsSafeCoverSpot()) return false;
	
		if (TryFindCoverSpot())
		{
			bCanHide = true;
			return true;
		}
	}

	return false;
}

bool URunawayMachineState::IsSafeCoverSpot()
{
	FVector PointLoc = StateMachine->GetBotBB()->GetValueAsVector(BB_COVER_SPOT);
	
	if (!FUtilities::IsValidVector(PointLoc)) return false;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Bot);
	FCollisionShape ColShape = FCollisionShape::MakeSphere(1500.f);
	TArray<FHitResult> OutHits;
	if (Bot->GetWorld()->SweepMultiByChannel(OutHits, PointLoc, PointLoc, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel3, ColShape, QueryParams))
	{
		for (int i = 0; i < OutHits.Num(); i++)
		{
			if (!OutHits[i].Actor.IsValid()) continue;

			if (!OutHits[i].Actor->IsA<ABasePraiseCharacter>()) continue;

			if (!Bot->IsEnemyTarget(Cast<IFactioneable>(OutHits[i].Actor.Get()))) continue;

			return !TargetCanSeePoint(OutHits[i].Actor.Get(), PointLoc);
		}
	}

	return true;
}

void URunawayMachineState::UpdatePath(bool bMoveToSpawnLoc)
{
	if (TrySetTargetLocation(bMoveToSpawnLoc))
	{
		LastPathUpdateTimestamp = StateMachine->GetTimeSinceGameStart();

		BotController->MoveToLocation(TargetLocation, 0.1f, true, true, true, false);
		
		StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_REACHED_TARGET, false);
		StateMachine->GetBotBB()->SetValueAsBool(BB_HAS_VALID_LOCATION, true);

		bIsMovingToTarget = true;

		bIsMovingToSpawnLoc = bMoveToSpawnLoc;

		if (bCanHide) {
			bIsMovingToCoverSpot = true;
		}

		else StateMachine->GetBotBB()->ClearValue(BB_COVER_SPOT);
	}
}

bool URunawayMachineState::HasReachedTarget()
{
	if (!Bot) return false;

	if (!FUtilities::IsValidVector(TargetLocation)) return false;

	FVector ToTarget = TargetLocation - Bot->GetActorLocation();

	return ToTarget.Size() <= StoppingDistance;
}

bool URunawayMachineState::HasBlackboardValues()
{
	if (StateMachine->GetBotBB()->GetValueAsBool(BB_LOCATION_OVER_TARGET) && FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION)))
		return true;

	return  FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_LAST_KNOWN_TARGET_LOC)) ||
		    FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_LAST_DAMAGER_TO_BOT_VECTOR)) || 
		    StateMachine->GetBotBB()->GetValueAsObject(BB_LAST_DAMAGER) || 
		    StateMachine->GetBotBB()->GetValueAsObject(BB_MAX_AGGRO_TARG);
}

FVector URunawayMachineState::GetRandomOpposedLocation(FVector LocationToAvoid)
{
	FNavLocation NewLocation;

	FVector ToAvoided = LocationToAvoid - Bot->GetActorLocation();

	FVector RunDir = Bot->GetActorLocation() + (ToAvoided.GetSafeNormal() * -1) * 100.f;

	float ExtraDegrees = 0;

	if (HasActorToAvoid() && ToAvoided.Size() <= SafeDistance)
	{
		FVector SafeVector = ToAvoided.GetSafeNormal() * SafeDistance;

		ExtraDegrees = (1 - (ToAvoided.GetSafeNormal().Size() / SafeDistance * 0.75)) * 120.f;

	}
	

	float RandAngle = FMath::RandRange(-25, 25);

	FVector TargetLoc = Bot->GetActorLocation() + ((ToAvoided.GetSafeNormal() * -1).RotateAngleAxis(RandAngle + ExtraDegrees * FMath::Sign(RandAngle), Bot->GetActorUpVector()) * (SafeDistance * 1.5));
	
	float Radius = TargetLocation == Bot->GetSpawnLocation() ? SearchDistance : StoppingDistance;
	
	if (NavMesh->GetRandomReachablePointInRadius(TargetLoc, Radius, NewLocation)) {
		return NewLocation.Location;
	}

	return FVector::ZeroVector;
}

bool URunawayMachineState::ShouldRequestBackup()
{
	return (HasBlackboardValues() || StateMachine->GetKnownEnemies().Num() > 0) && StateMachine->GetTimeSinceGameStart() - LastBackupReqTimestamp > BackupReqSeconds;
}

void URunawayMachineState::SendBackupRequest()
{
	if (!Bot->GetMsgsFactory()) return;

	UBackupRequestMsgCommand* Cmd = Cast<UBackupRequestMsgCommand>(Bot->GetMsgsFactory()->GetCmd(EAI_Msg::BACKUP_REQ));

	if (!Cmd) return;

	Cmd->SetSender(Bot);
	Cmd->SetMsgRange(BackupReqRange);
	Cmd->SetHasVisionCheck(true);
	Cmd->SetOriginLocation(Bot->GetActorLocation());

	FTelegram Msg = FTelegram(EAI_Msg::BACKUP_REQ, Bot->GetUniqueID());

	Msg.SetMsgCmd(Cmd, EAI_MsgChannel::ZONE);

	Bot->SendMessage(EAI_MsgChannel::ZONE, Msg);

	LastBackupReqTimestamp = StateMachine->GetTimeSinceGameStart();
}
