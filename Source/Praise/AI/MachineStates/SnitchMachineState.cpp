// Fill out your copyright notice in the Description page of Project Settings.


#include "SnitchMachineState.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../MsgCommands/BackupRequestMsgCommand.h"
#include "../MsgCommands/TargetSeenMsgCommand.h"
#include "../../Components/GameMode/MsgCommandsFactoryComponent.h"
#include "../../Characters/AI/BotCharacters/FightingBotCharacter.h"

USnitchMachineState::USnitchMachineState()
{
	BackupReqRange = 4000.f;
	BackupReqSeconds = 5.f;
	LastBackupReqTimestamp = 0;

	LastTargetCheckTimestamp = 0;
	TargetCheckSeconds = 2.5f;
}

bool USnitchMachineState::OnEnterState(ABaseBotController* NewBotController)
{
	if (!Super::OnEnterState(NewBotController)) return false;

	NavMesh = UNavigationSystemV1::GetCurrent(Bot->GetWorld());

	if (!NavMesh)
		return false;

	bIsTimeLimited = true;

	MaxTimeInCurrentState = StateMachine->GetBotBB()->GetValueAsFloat(BB_MAX_SNITCH);
	MinSnitchTime = StateMachine->GetBotBB()->GetValueAsFloat(BB_MIN_SNITCH);

	if (MaxTimeInCurrentState <= 0.f)
		MaxTimeInCurrentState = BB_GLOBAL_MAX_SNITCH_VAL;

	if (MinSnitchTime <= 0.f)
		MinSnitchTime = BB_GLOBAL_MIN_SNITCH_VAL;

	LastEnemyKnownLoc = StateMachine->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION);	 
	StateMachine->GetBotBB()->SetValueAsVector(BB_LAST_PRIORITY_LOCATION, LastEnemyKnownLoc);

	BackupReqRange = 2000.f;
	BackupReqSeconds = 10.f;
	LastBackupReqTimestamp = 0;
	SnitchingDistance = 400.f;
	LastTargetCheckTimestamp = 0;
	TargetCheckSeconds = 5.f;
	bHasSnitched = false;

	Bot->UpdateCharStatus(ECharStatus::ALERTED);

	bDidInit = true;

	return bDidInit;
}

void USnitchMachineState::OnExitState()
{
	if (bHasSnitched)
	{
		StateMachine->GetBotBB()->ClearValue(BB_LAST_KNOWN_TARGET_LOC);
		StateMachine->GetBotBB()->ClearValue(BB_PRIORITY_LOCATION);
		
	}
	StateMachine->GetBotBB()->ClearValue(BB_PRIORITY_TARGET);
	Super::OnExitState();
}

void USnitchMachineState::RunState()
{
	Super::RunState();
}

void USnitchMachineState::AddStateDecisions()
{
	UBooleanDecision* UpdateTargetDecision = Cast<UBooleanDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::BOOLEAN, this));

	if (UpdateTargetDecision && UpdateTargetDecision->HasValidBrain()) {
		UpdateTargetDecision->SetupDecision([this]() { return ShouldUpdateTarget(); }, [this](const bool bValue) { UpdateTarget(); }, /*bCallbackResult:*/ true);
		StateDecisions.Add(UpdateTargetDecision);
	}

	UBooleanDecision* SnitchDecision = Cast<UBooleanDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::BOOLEAN, this));

	if (SnitchDecision && SnitchDecision->HasValidBrain()) {
		SnitchDecision->SetupDecision([this]() { return ShouldSnitch(); }, [this](const bool bValue) { SendTargetSeenMsg(); }, /*bCallbackResult:*/ true);
		StateDecisions.Add(SnitchDecision);
	}

	UBooleanDecision* WanderDecision = Cast<UBooleanDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::BOOLEAN, this));

	if (WanderDecision && WanderDecision->HasValidBrain()) {
		WanderDecision->SetupDecision([this]() { return ShouldWander(); }, [this](const bool bValue) { GetRandomLocation(); }, /*bCallbackResult:*/ true);
		StateDecisions.Add(WanderDecision);
	}

	UBooleanDecision* BackupReqDecision = Cast<UBooleanDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::BOOLEAN, this));

	if (BackupReqDecision && BackupReqDecision->HasValidBrain()) {
		BackupReqDecision->SetupDecision([this]() { return ShouldRequestBackup(); }, [this](const bool bValue) { SendBackupRequest(); }, /*bCallbackResult:*/ true);
		StateDecisions.Add(BackupReqDecision);
	}

	UBooleanDecision* UpdatePathDecision = Cast<UBooleanDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::BOOLEAN, this));

	if (UpdatePathDecision && UpdatePathDecision->HasValidBrain()) {
		UpdatePathDecision->SetupDecision([this]() { return ShouldUpdatePath(); }, [this](const bool bValue) { UpdatePath(); }, /*bCallbackResult:*/ true);
		StateDecisions.Add(UpdatePathDecision);
	}

	TryAddStateDecision(EBotDecision::STAY_IN_IDLE);
}

bool USnitchMachineState::ShouldRun()
{
	if (Bot->GetCharStatus() == ECharStatus::ALERTED) return true;

	return GetPriorityTarget() && Bot->IsTargetInsideFOV(Cast<AActor>(StateMachine->GetPriorityTarget()), true);
}

bool USnitchMachineState::ShouldBackToIdle()
{
	if (StateMachine->GetKnownEnemies().Num() <= 0 && 
		!FUtilities::IsValidVector(StateMachine->GetBotBB()->GetValueAsVector(BB_LAST_KNOWN_TARGET_LOC)) && 
		TimeInCurrentState > MinSnitchTime)
			return true;

	bool bHasPriorityTarg = GetPriorityTarget() != nullptr;
	bool bHasAllies = StateMachine->GetKnownAllies().Num() <= 0;
	return (!bHasPriorityTarg && bHasAllies && TimeInCurrentState > MinSnitchTime) || bHasSnitched;
}

bool USnitchMachineState::ShouldUpdateTarget()
{
	if (!GetPriorityTarget())
		return true;

	if (GetPriorityTarget() && StateMachine->GetTimeSinceGameStart() - LastTargetCheckTimestamp < TargetCheckSeconds) return false;

	ITargeteable* Target = GetPriorityTarget();

	if (!Target) return true;

	if (!IsValidSnitchingTarget(Target)) return true;

	ITargeteable* Closest = StateMachine->GetClosestFoE(/*bEnemies:*/ false);

	if (!Closest) return false;

	if (Closest->GetTargetID() == Bot->GetUniqueID()) return false;

	if (IsValidSnitchingTarget(Closest))
	{
		if (Closest && Closest->GetTargetID() != Target->GetTargetID())
			if(Cast<ABasePraiseCharacter>(Closest)->GetHealth() >= Cast<ABasePraiseCharacter>(Target)->GetHealth())
			return true;
	}
	
	return false;
}

bool USnitchMachineState::ShouldWander()
{
	if (Bot->GetVelocity().Size() <= 0) return true;

	return !GetPriorityTarget() && StateMachine->GetKnownAllies().Num() <= 0 && MaxTimeInCurrentState < MinSnitchTime && Bot->GetVelocity().Size() <= 0.f;
}

bool USnitchMachineState::ShouldRequestBackup()
{
	return StateMachine->GetKnownEnemies().Num() > 0 && !GetPriorityTarget() && !bHasSnitched &&(StateMachine->GetTimeSinceGameStart() - LastBackupReqTimestamp > BackupReqSeconds);
}

bool USnitchMachineState::ShouldSnitch()
{
	if (!GetPriorityTarget()) return false;

	FVector ToTarget = Cast<AActor>(StateMachine->GetPriorityTarget())->GetActorLocation() - Bot->GetActorLocation();

	return ToTarget.Size() <= SnitchingDistance;
}

bool USnitchMachineState::ShouldUpdatePath()
{
	if (!GetPriorityTarget()) return false;

	FVector ToTarget = Cast<AActor>(GetPriorityTarget())->GetActorLocation() - Bot->GetActorLocation();

	float AngleToTarget = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Bot->GetActorForwardVector(), ToTarget.GetSafeNormal())));

	bool bIsMoving = Bot->GetVelocity().Size() > 0;

	if (!bIsMoving && AngleToTarget >= 30.f) { 
		return true;
	}

	if (Bot->IsTargetInsideFOV(Cast<AActor>(GetPriorityTarget()), true) && !bIsMoving && ToTarget.Size() > SnitchingDistance)
		return true;

	return !Bot->IsTargetInsideFOV(Cast<AActor>(GetPriorityTarget()), true);
}

void USnitchMachineState::UpdateTarget()
{
	ITargeteable* Target = TryGetSnitchingTarget();

	if (!Target) return;

	StateMachine->SetPriorityTarget(Cast<AActor>(Target));

	LastTargetCheckTimestamp = StateMachine->GetTimeSinceGameStart();
}

void USnitchMachineState::GetRandomLocation()
{
	FNavLocation NavLocation;
	if (NavMesh->GetRandomReachablePointInRadius(Bot->GetActorLocation(), StateMachine->GetBotBB()->GetValueAsFloat(BB_WANDERING_RADIUS), NavLocation))
	{
		BotController->MoveTo(NavLocation.Location);
		bHasValidLocation = true;
		bHasReachedLocation = false;
	}
}

void USnitchMachineState::SendBackupRequest()
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

void USnitchMachineState::SendTargetSeenMsg()
{
	if (!GetPriorityTarget()) return;

	if (!Bot->GetMsgsFactory()) return;

	UTargetSeenMsgCommand* Cmd = Cast<UTargetSeenMsgCommand> (Bot->GetMsgsFactory()->GetCmd(EAI_Msg::TARGET_SEEN_AT));

	if (!Cmd) return;

	Cmd->SetSender(Bot);
	Cmd->SetReceiver(Cast<ABasePraiseCharacter>(GetPriorityTarget()));
	Cmd->SetOriginLocation(Bot->GetActorLocation());
	Cmd->SetTargetLocation(LastEnemyKnownLoc);

	FTelegram Msg = FTelegram(EAI_Msg::TARGET_SEEN_AT, Bot->GetUniqueID());

	Msg.SetMsgCmd(Cmd, EAI_MsgChannel::ZONE);

	Bot->SendMessage(EAI_MsgChannel::ZONE, Msg);

	LastBackupReqTimestamp = StateMachine->GetTimeSinceGameStart();
	
	bHasSnitched = true;
}

void USnitchMachineState::UpdatePath()
{
	if (!GetPriorityTarget()) return;

	if (!FUtilities::IsValidVector(Cast<AActor>(GetPriorityTarget())->GetActorLocation())) return;

	bHasValidLocation = false;
	FVector CurrentTargetLocation = Cast<AActor>(GetPriorityTarget())->GetActorLocation();
	FNavLocation NavLocation;
	if (NavMesh->ProjectPointToNavigation(CurrentTargetLocation, NavLocation))		
	{
		CurrentTargetLocation = NavLocation.Location;
		bHasValidLocation = true;
		
	}

	if (bHasValidLocation)
	{
		bHasReachedLocation = false;
		BotController->MoveTo(CurrentTargetLocation);
	}

}

ITargeteable* USnitchMachineState::GetPriorityTarget()
{
	if (!StateMachine->HasValidBBTarget(BB_PRIORITY_TARGET)) return nullptr;

	if (!StateMachine->GetPriorityTarget()->Implements<UTargeteable>()) return nullptr;

	if (!StateMachine->GetPriorityTarget()->Implements<UFactioneable>()) return nullptr;

	if (Bot->IsEnemyTarget(Cast<IFactioneable>(StateMachine->GetPriorityTarget()))) return nullptr;
	
	return Cast<ITargeteable>(StateMachine->GetPriorityTarget());
}

bool USnitchMachineState::IsValidSnitchingTarget(ITargeteable* Target)
{
	if (!Target) return false;

	if (Target->IsDead()) return false;

	if (Target->IsTargetType() == ETargetType::NPC && Cast<AActor>(Target)->IsA<AFightingBotCharacter>())
	{
		AFightingBotCharacter* BotTarget = Cast<AFightingBotCharacter>(Target);

		if (BotTarget->GetBrain()->IsA<UStateMachineBrainComponent>())
		{
			UStateMachineBrainComponent* TargetBrain = Cast<UStateMachineBrainComponent>(BotTarget->GetBrain());

			return TargetBrain->GetCurrentState() != EMachineState::COMBAT && TargetBrain->GetKnownEnemies().Num() <= 0;
		}
	}

	return false;
}

ITargeteable* USnitchMachineState::TryGetSnitchingTarget()
{
	ITargeteable* Closest = StateMachine->GetClosestFoE(false);

	if (IsValidSnitchingTarget(Closest))
		return Closest;
	
	++CurrentGetTargetTries;

	if (CurrentGetTargetTries <= MaxGetTargetTries)
		return TryGetSnitchingTarget();

	return nullptr;
}
