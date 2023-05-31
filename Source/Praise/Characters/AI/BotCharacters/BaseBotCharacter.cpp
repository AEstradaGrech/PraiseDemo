// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBotCharacter.h"
#include "../../../Components/Actor/AI/BotBrainComponent.h"
#include "../../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../../../Components/Actor/AI/T200BrainComponent.h"
#include "../../../Components/Actor/AI/BTBrainComponent.h"
#include "../../../Components/Actor/AI/BTBrainComponent.h"
#include "../../../Components/Actor/CharStats/BotStatsComponent.h"
#include "../../../Interfaces/CombatSystem/Targeteable.h"
#include "../../../Networking/PraiseGameState.h"
#include "../../../Components/GameMode/MachineStatesFactoryComponent.h"
#include "../../../Components/Actor/AnimInstances/PraiseAnimInstance.h"
#include "../../../Weapons/Fist.h"
#include "../../../Weapons/MeleeWeapon.h"
#include "../../../Praise.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../../Environment/Buildings/BaseBuilding.h"
#include "../../../Weapons/Firearm.h"

ABaseBotCharacter::ABaseBotCharacter() : Super()
{
	BotSensor = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Bot Sensor"));
	bUseControllerRotationYaw = true;
	BotClass = EBotClass::CITIZEN;
	bIgnoreCreatures = false;
	bOnlySubZonePatrols = false;
	bCanEverTravel = true;
	bCanEverPatrol = true;
	PathFindingDistance = 30000.f;
	MaxTimeInIdle = BB_GLOBAL_MAX_IDLE_VAL;
	MaxTimeWandering = BB_GLOBAL_MAX_WANDERING_VAL;
	MaxTimeChasing = BB_GLOBAL_MAX_CHASING_VAL;
	MaxTimeRunningAway = BB_GLOBAL_MAX_RUNAWAY_VAL;
	bCanAttackFactionChars = false;

}


void ABaseBotCharacter::GetDamage(float Damage, AActor* Damager, EAttackType AttackType)
{
	Super::GetDamage(Damage, Damager, AttackType);

	if (!GetBrain()) return;

	if (Damager->IsA<ABasePraiseCharacter>())
		GetBrain()->TryAddCharTarget(Cast<ABasePraiseCharacter>(Damager), /*bIsEnemy*/ true);

	if (!GetBrain()->GetNeutralDamagers().Contains(Damager))
		HandleDamageReceived(Damage, Damager);
}

void ABaseBotCharacter::GetWeaponDamage(float Damage, EAttackType AttackType, EWeaponSlot AttackerHand, ABasePraiseCharacter* Damager, AWeapon* DamagerWeapon)
{
	Super::GetWeaponDamage(Damage, AttackType, AttackerHand, Damager, DamagerWeapon);

	if (!GetBrain()) return;

	GetBrain()->TryAddCharTarget(Damager, /*bIsEnemy*/ true);

	if(!GetBrain()->GetNeutralDamagers().Contains(Damager))
		HandleDamageReceived(Damage, Damager);
	
}

void ABaseBotCharacter::OverrideBrainComp(EBrainType NewBrainType)
{
	BotBrainType = NewBrainType;

	OverrideBrainComp();
}

void ABaseBotCharacter::OverrideBrainComp()
{
	switch (BotBrainType)
	{
		case(EBrainType::DEFAULT):
			SetNewBrain<UStateMachineBrainComponent>();
			break;
		case(EBrainType::T200):
			SetNewBrain<UT200BrainComponent>();
			break;
		case(EBrainType::BT_BRAIN):
			SetNewBrain<UBTBrainComponent>();
			break;
		default: break;
	}
}

bool ABaseBotCharacter::DidBrainInit()
{
	return GetBrain() && GetBrain()->DidInit();
}

void ABaseBotCharacter::SetOnlySubZonePatrols(bool bValue)
{
	bOnlySubZonePatrols = bValue;

	if (GetBrain() && GetBrain()->GetBotBB())
		GetBrain()->GetBotBB()->SetValueAsBool(BB_ONLY_SUBZONE_PATROL, bValue);
}

void ABaseBotCharacter::OnAIStateUpdate(EMachineState NewState)
{
	CurrentState = NewState;
}

void ABaseBotCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	TargetingComponent->SetIsAutoTarget(true);
	bShouldMove = false;
	bIsRunning = false;
	bHandleSpeedChange = false;

	if (GetCharacterMovement()) 
	{
		GetCharacterMovement()->SetAvoidanceEnabled(true);
		GetCharacterMovement()->bUseRVOAvoidance = true;
		GetCharacterMovement()->AvoidanceConsiderationRadius = 130.f;
	}

	if (bIgnoreCreatures)
		IgnoredFactions.Add(ECharFaction::CREATURE);

	if (bIgnoreNeutrals)
		IgnoredFactions.Add(ECharFaction::NONE);

	if (GetCharStats<UBotStatsComponent>())
		AlertedStateSeconds = GetCharStats<UBotStatsComponent>()->GetAlertedStateSeconds();

	if (!ensure(GetWorld() != nullptr)) return;

	AGameStateBase* GS = UGameplayStatics::GetGameState(GetWorld());
	
	if(GS->IsA<APraiseGameState>())
		GameState = Cast<APraiseGameState>(GS);
}

bool ABaseBotCharacter::TryInitBrain()
{
	if (!SetSensor()) return false;
	
	if (!GetBotBrain<UBotBrainComponent>()) return false; 

	UBotBrainComponent* BotBrain = GetBotBrain<UBotBrainComponent>();

	return BotBrain->InitBrain(GetBotController<ABaseBotController>(), this);
}

void ABaseBotCharacter::HandleMessage(FTelegram& Msg)
{
	Super::HandleMessage(Msg);
}

UBotBrainComponent* ABaseBotCharacter::GetBrain()
{
	return GetBotBrain<UBotBrainComponent>();
}

bool ABaseBotCharacter::HasBeenRecentlyDamaged()
{
	return GetBrain() ? GetBrain()->HasBeenRecentlyDamaged() : false;
}

void ABaseBotCharacter::TryLockTarget()
{
	if (!TargetingComponent) return;
	
	if (TargetingComponent->IsTargetLocked()) {
		TargetingComponent->ClearTarget();
		return;
	}

	bIsTargeting = TargetingComponent->TryLockTarget(true);
}

void ABaseBotCharacter::HandleCharDeadNotify(AActor* Victim, AActor* Killer)
{
	Super::HandleCharDeadNotify(Victim, Killer);

	if (Victim->GetUniqueID() == GetUniqueID()) return;

	if (GetBrain()) {
		GetBrain()->ClearBBTarget(Victim);
		GetBrain()->TryRemoveKnownTarget(Cast<ITargeteable>(Victim));
	}
	if (bIsTargeting)
	{
		if (TargetingComponent && GetLockedTarget() && GetLockedTarget()->GetUniqueID() == Victim->GetUniqueID())
		{
			TargetingComponent->ClearTarget();
			bIsTargeting = false;
		}
	}
}

bool ABaseBotCharacter::CanReachLocation(FVector Loc)
{
	if (!ensure(GetWorld() != nullptr)) return false;

	UNavigationSystemV1* NavMesh = UNavigationSystemV1::GetCurrent(GetWorld());

	if (!NavMesh) return false;
	FNavLocation Location;
	if (NavMesh->GetRandomReachablePointInRadius(Loc, 50.f, Location))
	{
		DrawDebugSphere(GetWorld(), Location.Location, 50.f, 8, FColor::Red, false, 5.f);
		return true;
	}

	return false;
}

void ABaseBotCharacter::AddDefaultUnarmedWeapon()
{
	UCharWeaponSlot* Slot = NewObject<UCharWeaponSlot>(this);

	Slot->SetupSlot(EWeaponSlot::PRIMARY_WEAPON);

	if (!DefaultUnarmedWeapon)
	{
		AFist* CharFist = SpawnDefaultUnarmedWeapon();

		if (!CharFist) return;

		CharFist->SetOwner(this);

		AttachWeapon(CharFist, FName("Hand_RSocket"), false);

		DefaultUnarmedWeapon = CharFist;
	}

	Slot->SetSlotWeapon(DefaultUnarmedWeapon, EHolsterPosition::UNARMED);

	if (!CharWeaponSlots.Contains(Slot->GetSlotID()))
	{
		CharWeaponSlots.Add(Slot->GetSlotID(), Slot);
	}
}

void ABaseBotCharacter::StartPrimaryAttack()
{
	
}

void ABaseBotCharacter::TriggerEvade()
{
	if (!CanExecuteAction(ECharAction::EVADE)) return;

	EvadeDirection = GetBrain()->GetBestEvadeDirection();
	
	bIsEvading = true;

	if (GetController())
	{
		GetController()->StopMovement();
	}

	PlayEvadeAnimation(EvadeDirection);
}


void ABaseBotCharacter::OnInteractionBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void ABaseBotCharacter::OnInteractionEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}


void ABaseBotCharacter::OnPawnSeen(APawn* SeenPawn)
{
	if (IsDead()) return;

	if (!GetBotBrain<UBotBrainComponent>()) return;

	UBotBrainComponent* BotBrain = GetBotBrain<UBotBrainComponent>();

	if (!BotBrain->IsValidTarget(SeenPawn)) return;

	BotBrain->TryAddNewTarget(Cast<ITargeteable>(SeenPawn));
}

void ABaseBotCharacter::OnPawnHeard(APawn* HeardPawn, const FVector& Location, float Volume)
{
	
}

void ABaseBotCharacter::OnKnownEnemyDead(AActor* Dead, AActor* Killer)
{
	if (Dead->GetClass()->ImplementsInterface(UTargeteable::StaticClass()))
		if (GetBrain() && GetBrain()->TryRemoveKnownTarget(Cast<ITargeteable>(Dead)))
			if (Dead->IsA<ABasePraiseCharacter>())
				Cast<ABasePraiseCharacter>(Dead)->OnNotifyDead.RemoveDynamic(this, &ABaseBotCharacter::OnKnownEnemyDead);
}

FPlayerStats ABaseBotCharacter::GetDefaultBotStats()
{
	FPlayerStats Stats;
	Stats.Strenght = 5;
	Stats.Constitution = 5;
	Stats.Dextrity = 5;
	Stats.Intelligence = 5;
	Stats.Charisma = 5;
	Stats.Wisdom = 5;
	Stats.Luck = 5;
	Stats.Level = 1;
	Stats.Class = ECharClass::NONE;

	return Stats;
}

bool ABaseBotCharacter::SetSensor()
{
	
	if (!BotSensor) return false;
	if (!CharStats) return false;

	BotSensor->SetPeripheralVisionAngle(CharStats->GetFieldOfView());
	BotSensor->SightRadius = CharStats->GetSightRange();
	BotSensor->bOnlySensePlayers = false;
	BotSensor->SetSensingInterval(0.5f);
	BotSensor->bHearNoises = false;
	BotSensor->OnSeePawn.AddDynamic(this, &ABaseBotCharacter::OnPawnSeen);
	return true;
}

void ABaseBotCharacter::HandleShouldRun()
{
	if (!GetBrain()) return;

	switch (GetBrain()->ShouldRun()) 
	{
		case(true):
			if (!bIsRunning)
				StartSprint();
			break;

		case(false):
			if (bIsRunning)
				EndSprint();
			break;
	}
}

void ABaseBotCharacter::OnAnimStateUpdate(ECharAnimState State, bool bIsEnabled)
{
	if (IsDead()) return;
	Super::OnAnimStateUpdate(State, bIsEnabled);

	switch (State)
	{
	case(ECharAnimState::IDLE):
		//ResetAll();
		break;

	case(ECharAnimState::EQUIPING):
		
		
		break;
	case(ECharAnimState::ATTACKING):
		if (!(GetBrain() && GetWorld() && GetWorld()->GetGameState())) return;
		GetBrain()->LastAttackTimestamp = GetWorld()->GetGameState()->GetGameTimeSinceCreation();
		break;
	case(ECharAnimState::JUMPING):
		bIsJumping = bIsEnabled;
		break;
	case(ECharAnimState::ROLLING):
		bIsRolling = bIsEnabled;
		break;
	case(ECharAnimState::EVADING):
		bIsEvading = bIsEnabled;
		if (!(GetBrain() && GetWorld() && GetWorld()->GetGameState())) return;
		GetBrain()->LastEvadeTimestamp = GetWorld()->GetGameState()->GetGameTimeSinceCreation();
		break;
	}
}

void ABaseBotCharacter::Tick(float DeltaTime)
{
	if (bHandleSpeedChange && CharStats) 
	{
		float NewSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, CharStats->GetWalkSpeed(), DeltaTime, GetCharStats<UBotStatsComponent>()->GetAccelerationLerpSpeed());

		GetCharacterMovement()->MaxWalkSpeed = NewSpeed; 

		if (GetCharacterMovement()->MaxWalkSpeed == CharStats->GetWalkSpeed())
			bHandleSpeedChange = false;
	}
	
	if (GetBrain() && !GetBrain()->IsA<UBTBrainComponent>())
	{	
		switch (GetBrain()->ShouldRun())
		{
			case(true):
				if (!bIsRunning && GetBrain()->GetBotBB()->GetValueAsBool(BB_CAN_RUN))
					StartSprint();
				break;
			case(false):
				if (bIsRunning)
					EndSprint();
				break;
		}

		switch (GetBrain()->ShouldEquip())
		{
			case(true):
				if (!bIsEquiping && !IsWeaponEquiped())
					TriggerEquipWeapons();
				break;
			case(false):
				if (!bIsEquiping && IsWeaponEquiped())
					TriggerEquipWeapons();
				break;
		}

		switch (GetBrain()->ShouldTarget())
		{
			if (!TargetingComponent) return;

			case(true):
				
				if (!bIsTargeting)
				{
					TargetingComponent->SetOnlyEnemyTargets(GetBrain()->IsOnlyEnemyTargets());	

					TryLockTarget();
				}

				break;
			case(false):

				if (bIsTargeting) 
				{
					TargetingComponent->ClearTarget();
					TargetingComponent->SetOnlyEnemyTargets(false);
					bIsTargeting = false;
				}

				break;
		}

		if (GetBrain()->ShouldEvade()) {
			TriggerEvade();
			return;
		}

		if (GetBrain()->ShouldAttack())
			StartPrimaryAttack();

		switch (GetBrain()->ShouldBlock())
		{
			case(true):
				if (!bIsBlocking)
					StartBlocking();
				break;
			case(false):
				if (bIsBlocking)
					EndBlocking();
				break;
		}
	}
	
	Super::Tick(DeltaTime);
}

void ABaseBotCharacter::OverrideCharStatsComp()
{
	UBotStatsComponent* BotStats = NewObject<UBotStatsComponent>(this);

	if (!BotStats) return;

	CharStats = BotStats;

	if (CharStats) 
	{
		CharStats->RegisterComponent();
		CharStats->InitializeComponent();
	}

}

void ABaseBotCharacter::HandleSpeedChange(float DeltaTime)
{
	float NewSpeed = FMath::FInterpTo(GetCharacterMovement()->MaxWalkSpeed, CharStats->GetWalkSpeed(), DeltaTime, 2.f);

	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;

	float TargetSpeed = bIsRunning ? CharStats->GetMaxWalkSpeed() : CharStats->GetDefaultWalkSpeed();
	if (GetCharacterMovement()->MaxWalkSpeed == TargetSpeed)
		bHandleSpeedChange = false;
}

void ABaseBotCharacter::TrySetHandColliders()
{
	
}

void ABaseBotCharacter::AddCharStatsBar()
{
	SetupCharStatsBar();
}

void ABaseBotCharacter::SetCharacterDead(AActor* Killer)
{
	if (BotSensor)
	{
		BotSensor->OnSeePawn.RemoveDynamic(this, &ABaseBotCharacter::OnPawnSeen);
		BotSensor->SetActive(false);
	}

	Super::SetCharacterDead(Killer);
}

void ABaseBotCharacter::HandleDamageReceived(float Damage, AActor* Damager)
{
	if (!GetBrain()) return;

	if (!GetBrain()->GetBotBB()) return;

	FVector ToTarget = Damager->GetActorLocation() - GetActorLocation();
	
	GetBrain()->RegisterReceivedDamage(Damage, Damager->GetUniqueID());

	GetBrain()->GetBotBB()->SetValueAsVector(BB_LAST_DAMAGER_TO_BOT_VECTOR, ToTarget.GetSafeNormal());

	bool bUpdateLastDamager = ToTarget.Size() < GetBrain()->GetPerceptionRadius() ? CanSeeTarget(Damager) : IsTargetInsideFOV(Damager, true);
	
	if(bUpdateLastDamager)
		GetBrain()->GetBotBB()->SetValueAsObject(BB_LAST_DAMAGER, Damager);
	
	if (GetBotBrain<UStateMachineBrainComponent>() && GetBotBrain<UStateMachineBrainComponent>()->GetCurrentState() != EMachineState::COMBAT)
	{
		if (CharStatus != ECharStatus::ALERTED)		
			UpdateCharStatus(ECharStatus::ALERTED);
	}

	else UpdateCharStatus(ECharStatus::NORMAL); 
}

void ABaseBotCharacter::ResetEquipWeapons()
{
}

void ABaseBotCharacter::SetupCharWeaponSlots()
{
	
}

void ABaseBotCharacter::TriggerEquipWeapons()
{
	if (!HasWeapon()) return;

	EquipWeapons();
}

void ABaseBotCharacter::TriggerWeaponSlot(EWeaponSlot Slot)
{
	TrySwitchWeapon(Slot);
}
