// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatMachineState.h"
#include "../../AI/BotDecisions/ChaseDecision.h"
#include "../../AI/BotDecisions/BooleanDecision.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../../Components/GameMode/MsgCommandsFactoryComponent.h"
#include "../../Characters/AI/BotCharacters/FightingBotCharacter.h"
#include "../MsgCommands/BackupRequestMsgCommand.h"
#include "../../Structs/CommonUtility/FLogger.h"

UCombatMachineState::UCombatMachineState() : Super()
{

}

void UCombatMachineState::AddStateDecisions()
{
                         
    TryAddStateDecision(EBotDecision::SELECT_TARGET, EBotDecision::CHASE_TARGET); 
    TryAddStateDecision(EBotDecision::ENGAGE_COMBAT);

    UBooleanDecision* BackupReqDecision = Cast<UBooleanDecision>(StateMachine->GetStatesFactory()->GetStateDecision(EBotDecision::BOOLEAN, this));
    
    if (BackupReqDecision && BackupReqDecision->HasValidBrain()) {
        BackupReqDecision->SetupDecision([this]() { return ShouldRequestBackup(); }, [this](const bool bValue) { SendBackupRequest(); }, /*bCallbackResult:*/ true);
        StateDecisions.Add(BackupReqDecision);
    }

    TryAddStateDecision(EBotDecision::STAY_IN_IDLE);
}

void UCombatMachineState::LocateFightingAllies()
{
    for (AActor* Ally : StateMachine->GetKnownAllies())
    {
        if (Ally && Ally->IsA<AFightingBotCharacter>() && Bot->IsInCombatRange(Ally, 0))
        {
            if (!NearbyAllies.Contains(Ally))
                NearbyAllies.Add(Ally);
        }
    }
}

void UCombatMachineState::LocateEnemies()
{
    NearbyEnemies.Empty();
    TArray<ECharVector> LocationKeys;
    NearbyEnemies.GetKeys(LocationKeys);
    int NearbyEnemiesCount = 0;
    for (AActor* Target : StateMachine->GetKnownEnemies())
    {
        if (Target && Target->Implements<UCombatible>())
        {
            ICombatible* CombatibleTarg = Cast<ICombatible>(Target);

            if (Bot->IsInCombatRange(Target, Bot->GetMeleeDistanceOffset()))
            {
                ++NearbyEnemiesCount;
                ECharVector Location = Bot->GetTargetLocationVector(Target);

                if (!NearbyEnemies.Contains(Location))
                    NearbyEnemies.Add(Location);

                for (ECharVector Vector : LocationKeys)
                {
                    if (NearbyEnemies[Vector].Contains(Target))
                    {
                        if (Location != Vector)
                            NearbyEnemies[Vector].Remove(Target);
                    }
                    else
                    {
                        if (Location == Vector)
                            NearbyEnemies[Vector].Add(Target);
                    }
                }
            }
        }
    }

    StateMachine->GetBotBB()->SetValueAsBool(BB_SELECT_CLOSEST_FOE, NearbyEnemiesCount > 2 && !StateMachine->GetBotBB()->GetValueAsBool(BB_LOCATION_OVER_TARGET));
}

void UCombatMachineState::UpdateEnemyStatus()
{
    LocateEnemies();

    ECharVector MostDangerousVector = GetMostDangerousVector();

    TArray<ECharVector> SafeDirections = GetSafeDirections();

    ECharVector EvadeDirection = SafeDirections.Contains(FUtilities::GetOpposedCharVector(MostDangerousVector)) ?
        FUtilities::GetOpposedCharVector(MostDangerousVector) :
        SafeDirections.Num() > 0 ?
        SafeDirections[FMath::RandRange(1, SafeDirections.Num() - 1)] :
        ECharVector::BWD;

    SafestEvadeDirection = EvadeDirection;
}

ECharVector UCombatMachineState::GetMostDangerousVector()
{
    if (NearbyEnemies.Num() <= 0) return ECharVector::NONE;

    TArray<ECharVector> LocationKeys;
    NearbyEnemies.GetKeys(LocationKeys);
    ECharVector WorstLocation = ECharVector::NONE;

    for (int i = 0; i < LocationKeys.Num(); i++) 
    {
        if (i != 0) 
        {
            if (NearbyEnemies[WorstLocation].Num() < NearbyEnemies[LocationKeys[i]].Num())
                WorstLocation = LocationKeys[i];
        }

        else  WorstLocation = LocationKeys[0];
    }

    return WorstLocation;
}

TArray<ECharVector> UCombatMachineState::GetSafeDirections()
{
    TArray<ECharVector> SafeDirs;
    for (int i = 0; i < FUtilities::GetEnumCount(*FString("ECharVector")); i++)
    {
        if (!NearbyEnemies.Contains((ECharVector)i))
            SafeDirs.Add((ECharVector)i);
    }

    return SafeDirs;
}

bool UCombatMachineState::ShouldRequestBackup()
{
    if (StateMachine->GetTimeSinceGameStart() - LastBackupReqTimestamp < BackupReqSeconds) return false;
    
    if (Bot->GetCharStatus() == ECharStatus::DAMAGED)
    {
        if (NearbyEnemies.Num() > 1 && NearbyEnemies.Num() > NearbyAllies.Num())
            return true;
    }

    if (Bot->GetCharStatus() == ECharStatus::VERY_DAMAGED)
    {
        return true;
    }

    return false;
}

void UCombatMachineState::SendBackupRequest()
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

    Bot->PlayCharSFX(ECharSFX::CHAR_BACKUP_REQ);
}

bool UCombatMachineState::OnEnterState(ABaseBotController* Controller)
{
    if (!Super::OnEnterState(Controller)) return false;

    StateMachine->GetBotBB()->SetValueAsBool(BB_ONLY_ENEMY_TARGETS_ALLOWED, true);
   
    StateMachine->LastAttackTimestamp = 0;

    CurrentTarget = StateMachine->GetPriorityTarget();

    SecondsToUpdateLocations = 0;

    UpdateEnemyLocationSeconds = 1.f;

    BackupReqRange = 2500.f;
    BackupReqSeconds = 10.f;
    LastBackupReqTimestamp = 0;

    if (Bot->GetTargetingComponent())
    {
        Bot->GetTargetingComponent()->SetIsAutoTarget(true);
        Bot->GetTargetingComponent()->SetCheckFOV(false);
        Bot->GetTargetingComponent()->SetCheckCollision(false);
    }

    bDidInit = true;

    return true;
}

void UCombatMachineState::OnExitState()
{
    Super::OnExitState();

    StateMachine->GetBotBB()->SetValueAsBool(BB_ONLY_ENEMY_TARGETS_ALLOWED, false);

    if (Bot->GetTargetingComponent())
    {
        Bot->GetTargetingComponent()->SetIsAutoTarget(false);
        Bot->GetTargetingComponent()->SetCheckFOV(true);
        Bot->GetTargetingComponent()->SetCheckCollision(true);
    }
}

void UCombatMachineState::RunState()
{
  
    if (Bot->IsTargeting() && Bot->GetLockedTarget())
    {
        ICombatible* Targ = GetTargetAsCombatible();
        
        if (!Targ) return;

        if (Targ->IsEvading() || Targ->IsRolling()) return;

        FVector ToTarg = Bot->GetLockedTarget()->GetActorLocation() - Bot->GetActorLocation();
       
        bUsePathFinding = ToTarg.Size() <= PathFindingDistance;

        if (!Bot->IsInMeleeRange(Bot->GetLockedTarget(), Bot->GetMeleeDistanceOffset()) && ToTarg.Size() > Bot->GetMinMeleeDistance())
        {   
            if (!Bot->IsAttacking() && Bot->CanMove() && ToTarg.Size() > Bot->GetMinMeleeDistance())
            {
                BotController->MoveToLocation(Bot->GetLockedTarget()->GetActorLocation(), Bot->GetMinMeleeDistance(), true, bUsePathFinding, true);
            }
        }

        if (ToTarg.Size() < Bot->GetMinMeleeDistance() && !Bot->IsAttacking())
        {
            FVector Vector = (Bot->GetActorLocation() - Bot->GetLockedTarget()->GetActorLocation()).GetSafeNormal() * 175.f;

            Bot->AddMovementInput(Vector);
            
            return;
        }
    }

    if (LastTickSeconds <= 0.f || (SecondsToUpdateLocations >= UpdateEnemyLocationSeconds))
    {
        SecondsToUpdateLocations = 0;

        UpdateEnemyStatus();

        LocateFightingAllies();
    }

    else SecondsToUpdateLocations += StateMachine->GetTimeSinceGameStart() - LastTickSeconds;

    Super::RunState();
}

bool UCombatMachineState::ShouldRun()
{
    return false;
}

bool UCombatMachineState::ShouldEquip()
{
    return Bot->HasWeapon();
}

bool UCombatMachineState::ShouldAttack()
{
    float Timestamp = StateMachine->GetTimeSinceGameStart();
    bool bCanAttack = Timestamp - StateMachine->LastAttackTimestamp > StateMachine->GetAttackAwaitSecs();
    return Bot->HasWeapon() && !Bot->IsAttacking() && Bot->CanAttack()  && CanAttackTarget() && bCanAttack;
}

bool UCombatMachineState::ShouldBlock()
{
   
    ICombatible* Opponent = GetTargetAsCombatible();

    if (!Opponent) return false;

    if (Opponent->IsInMeleeRange(Bot, Opponent->GetMeleeDistanceOffset()))
    {
        if (Bot->IsWeaponEquiped())
        {
            if (!Bot->IsAttacking() && Bot->HasMinActionStam(ECharAction::BLOCK) && Opponent->IsAttacking())
                return true;
        }
    }

    return false;
}

bool UCombatMachineState::ShouldTarget()
{
    return Bot->IsWeaponEquiped() && StateMachine->GetPriorityTarget();
}

bool UCombatMachineState::ShouldEvade()
{
    bool bShouldEvade = false;
    
    bool bCanEvade = StateMachine->GetTimeSinceGameStart() - StateMachine->LastEvadeTimestamp > StateMachine->GetEvadeAwaitSecs();

    ICombatible* Target = GetTargetAsCombatible();

    if (!Target) return false;

    if (Target->IsAttacking() && Target->IsTargetInsideFOV(Bot, true, 90) && Target->IsInMeleeRange(Bot, Target->GetMeleeDistanceOffset() + 50.f) && !Bot->IsEvading() && !Bot->IsEquiping() && bCanEvade)
    {
        StateMachine->SetBestEvadeDirection(SafestEvadeDirection);

        return true;
    }
        
    
    return bShouldEvade;
}

bool UCombatMachineState::ShouldBackToIdle()
{
    if (!StateMachine->GetPriorityTarget() && NearbyEnemies.Num() <= 0 && StateMachine->GetKnownEnemies().Num() <= 0)
    {
        return true;
    }

    return !StateMachine->GetPriorityTarget() && NearbyEnemies.Num() <= 0 && StateMachine->GetKnownEnemies().Num() <= 0; 
}

bool UCombatMachineState::CanAttackTarget()
{
    return Bot->GetLockedTarget() && Bot->IsInMeleeRange(Bot->GetLockedTarget(), Bot->GetMeleeDistanceOffset()) && Bot->CanSeeTarget(Bot->GetLockedTarget(), true) && !Bot->IsAttacking();
}

ITargeteable* UCombatMachineState::GetTargetAsTargeteable() const
{
    return Cast<ITargeteable>(StateMachine->GetPriorityTarget());
}

ICombatible* UCombatMachineState::GetTargetAsCombatible() const
{
    if (StateMachine->GetPriorityTarget() && StateMachine->GetPriorityTarget()->Implements<UCombatible>())
    {
        return Cast<ICombatible>(StateMachine->GetPriorityTarget());
    }

    return nullptr;
}
 

