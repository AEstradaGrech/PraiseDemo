// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachineBrainComponent.h"
#include "../../../AI/MachineStates/BaseMachineState.h"
#include "../../../AI/BotDecisions/LocoDecision.h"



UStateMachineBrainComponent::UStateMachineBrainComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.SetTickFunctionEnable(false);
	CurrentState = EMachineState::IDLE;
	CurrentMachineState = nullptr;

	StateUpdateCounter = 0.f;
}

void UStateMachineBrainComponent::OnPostWorldCreatedCallback(UWorld* World)
{
}

void UStateMachineBrainComponent::SetDefaults()
{
	Super::SetDefaults();

	StateUpdateCounter = 0.f;

	CurrentState = EMachineState::IDLE;

	if (!ensure(GetWorld() != nullptr)) return;

	GetWorld()->GetTimerManager().ClearTimer(StateUpdateTimerHandle);
}

bool UStateMachineBrainComponent::InitBrain(ABaseBotController* OwnerController, ABaseBotCharacter* BrainOwner)
{
	if(!Super::InitBrain(OwnerController, BrainOwner)) return false;

	if (!StatesFactory) return false;

	SetBrainEnabled(true);

	if (!CurrentMachineState) return false;

	bDidInit = true;

	return bDidInit;
}

void UStateMachineBrainComponent::SetStateMachineEnabled(bool bEnabled)
{
	bIsBrainEnabled = bEnabled;

	switch (bIsBrainEnabled)
	{
		case(true):

			CurrentState = EMachineState::IDLE;
			
			SetupDefaultDecisions();
			
			CurrentMachineState = GetFactoryState(EMachineState::IDLE);
			
			if (!CurrentMachineState) {
				SetBrainEnabled(false);
				CurrentMachineState = nullptr; 
				bDidInit = false;
				return;
			}
		
			if (!CurrentMachineState->OnEnterState(BotController))
			{
				SetBrainEnabled(false);
				CurrentMachineState = nullptr; 
				bDidInit = false;
				return;
			}

			
			AddLocoDecison(EBotDecision::SPRINT);
			AddLocoDecison(EBotDecision::EQUIP_WEAPON);
			AddLocoDecison(EBotDecision::EVADE);
			AddLocoDecison(EBotDecision::ATTACK);
			AddLocoDecison(EBotDecision::BLOCK);
			AddLocoDecison(EBotDecision::LOCK_TARGET);
			AddLocoDecison(EBotDecision::UNBLOCK_PATH); 
			AddLocoDecison(EBotDecision::STAY_IN_IDLE); 

			UpdateState(CurrentMachineState);

			GetWorld()->GetTimerManager().SetTimer(PerceptionCheckTimerHandle, this, &UStateMachineBrainComponent::OnPerceptionCheck, PerceptionCheckSeconds, true);

			break;
		case(false):

			GetWorld()->GetTimerManager().ClearTimer(StateUpdateTimerHandle);
			GetWorld()->GetTimerManager().ClearTimer(PerceptionCheckTimerHandle);
			CurrentMachineState = nullptr;
			
			CachedDefaultDecisions.Empty();

			ClearBB();

			break;
	}

	PrimaryComponentTick.SetTickFunctionEnable(bIsBrainEnabled);
}

void UStateMachineBrainComponent::SetStayInIdle(bool bValue)
{
	Super::SetStayInIdle(bValue);

	if(bShouldStayInIdle)
		SwitchAIState(EMachineState::IDLE);
}

void UStateMachineBrainComponent::ClearMachine()
{
	CurrentState = EMachineState::IDLE;

	StateUpdateCounter = 0.f;

	GetWorld()->GetTimerManager().ClearTimer(StateUpdateTimerHandle);

	CurrentMachineState = nullptr;
}

void UStateMachineBrainComponent::ResetLoop()
{
	Super::ResetLoop();

	SwitchAIState(EMachineState::IDLE);	
}

void UStateMachineBrainComponent::BeginPlay()
{
	Super::BeginPlay();
	bShouldStayInIdle = false;
}

void UStateMachineBrainComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Bot == nullptr)
	{
		return;
	}

	if (BotBB && BotBB->GetValueAsBool(BB_IS_PAUSED)) 
	{
		return;
	}

	if (CurrentMachineState == nullptr)
		ChangeState();

	if (CurrentMachineState && !Bot->IsDead()) 
	{
		if (CurrentMachineState->GetClass()->ImplementsInterface(UEvaluable::StaticClass()))
			HandleLocoDecisions(CurrentMachineState);
		
		HandleStateDecisions(CurrentMachineState);

		CurrentMachineState->RunState();
	}
}

void UStateMachineBrainComponent::SwitchAIState(EMachineState NewState)
{
	if (Bot->IsDead()) return;

	if (!StatesFactory) return;

	if (CurrentState == NewState) return;

	UBaseMachineState* NewMachineState = GetFactoryState(NewState);

	if (!NewMachineState) return;

	CurrentState = NewState;

	CurrentMachineState->OnExitState();
	
	CurrentMachineState = NewMachineState;

	if (!CurrentMachineState->OnEnterState(BotController))
		SwitchAIState(EMachineState::IDLE);
	
	OnStateUpdate.Broadcast();
}

void UStateMachineBrainComponent::SetBrainEnabled(bool bEnable)
{
	SetStateMachineEnabled(bEnable);

	Super::SetBrainEnabled(bEnable); 
}

void UStateMachineBrainComponent::BeginAutodestroy()
{
	Bot->GetDamage(-Bot->GetHealth(), Bot);
}

TArray<UBotDecision*> UStateMachineBrainComponent::GetDefaultBotDecisions() const
{
	if (Bot->IsDead()) return TArray<UBotDecision*>();

	return CachedDefaultDecisions;
}

void UStateMachineBrainComponent::TickRunState(float DeltaTime)
{
	StateUpdateCounter += DeltaTime;

	if (StateUpdateCounter >= StateUpdateInterval && CurrentMachineState)
	{
		CurrentMachineState->RunState();
		StateUpdateCounter = 0.f;
	}
}

TArray<UBotDecision*> UStateMachineBrainComponent::GetCharDefaultDecisions()
{
	if(!Bot) return TArray<UBotDecision*>();

	if(!StatesFactory) return TArray<UBotDecision*>();



	return TArray<UBotDecision*>();
}

UBaseMachineState* UStateMachineBrainComponent::GetFactoryState(EMachineState NewState)
{
	if (!StatesFactory) return nullptr;

	UBaseMachineState* MachineState = StatesFactory->GetMachineState(NewState);

	if (!MachineState) return nullptr;

	return MachineState;
}

void UStateMachineBrainComponent::UpdateState(UBaseMachineState* NewState)
{
	CurrentMachineState->OnExitState();
	CurrentMachineState = NewState;
	CurrentMachineState->OnEnterState(BotController);

}

void UStateMachineBrainComponent::CheckForUpdates()
{
	if (CurrentState != Bot->GetCurrentAIState())
	{
		ChangeState();

		CurrentState = Bot->GetCurrentAIState();
	}
}

void UStateMachineBrainComponent::HandleLocoDecisions(IEvaluable* EvaluableState)
{
	FDecisionOutput DecisionOutput;
	for (int i = 0; i < LocomotionDecisions.Num(); i++) 
		if (LocomotionDecisions[i]->HasDecided(DecisionOutput)) 
			DecisionOutput.DecisionCallback(DecisionOutput.bCallbackResult);
}

void UStateMachineBrainComponent::HandleStateDecisions(UBaseMachineState* State)
{
	FDecisionOutput Output;
	for (int i = 0; i < State->GetStateDecisions().Num(); i++) 
	{
		
		if (State->DidInit() && State->GetStateDecisions()[i]->HasDecided(Output)) 
		{
			
			switch (Output.bIsTransitionDecision) 
			{
				case(true):
					
					if(Output.DecisionCallback)
						Output.DecisionCallback(Output.bCallbackResult);
					
					if (CurrentState != Output.OutputState) 
					{
						SwitchAIState(Output.OutputState);	

						return;								
					}
															
				case(false):
					if(Output.DecisionCallback)
						Output.DecisionCallback(Output.bCallbackResult);
					break;
				}
		}
	}
}

bool UStateMachineBrainComponent::EvaluateCurrentState(IEvaluable* State, EBotDecision Decision)
{
	switch (Decision) 
	{
		case(EBotDecision::SPRINT):
			return State->ShouldRun();
		case(EBotDecision::EQUIP_WEAPON):
			return State->ShouldEquip();
		case(EBotDecision::ATTACK):
			return State->ShouldAttack();
		case(EBotDecision::BLOCK):
			return State->ShouldBlock();
		case(EBotDecision::EVADE):
			return State->ShouldEvade();
		case(EBotDecision::LOCK_TARGET):
			return State->ShouldTarget();
		case(EBotDecision::UNBLOCK_PATH):
			return State->ShouldUnblockPath();
		case(EBotDecision::STAY_IN_IDLE):
			return State->ShouldBackToIdle();
		default:
			return false;
	}
	return false;
}

void UStateMachineBrainComponent::SetupDefaultDecisions()
{
	if (!StatesFactory) return;

	if (!DefaultBotDecisions.Contains(EBotDecision::START_PATROL))
		DefaultBotDecisions.Add(EBotDecision::START_PATROL);

	for (int i = 0; i < DefaultBotDecisions.Num(); i++) 
	{
		UBotDecision* Decision = StatesFactory->GetStateDecision(DefaultBotDecisions[i]);
		
		if (!Decision) continue;
		
		Decision->SetBotBrain(this);
		CachedDefaultDecisions.Add(Decision);
	}

	BotBB->SetValueAsFloat(BB_GLOBAL_MAX_IDLE, BB_GLOBAL_MAX_IDLE_VAL);
	BotBB->SetValueAsFloat(BB_GLOBAL_MAX_IDLE, Bot->GetMaxTimeWandering() <= 0 ? BB_GLOBAL_MAX_WANDERING_VAL : Bot->GetMaxTimeWandering());
}

void UStateMachineBrainComponent::AddLocoDecison(EBotDecision DecisionType)
{
	ULocoDecision* LocoDecision = Cast<ULocoDecision>(StatesFactory->GetStateDecision(EBotDecision::LOCOMOTION));
	
	if (!LocoDecision) return; 
		
	LocoDecision->SetDecisionType(DecisionType);
	LocoDecision->SetBotBrain(this);

	switch (DecisionType) {
		case(EBotDecision::SPRINT):
			LocoDecision->SetupDecision([this]() { return CurrentMachineState ? EvaluateCurrentState(CurrentMachineState, EBotDecision::SPRINT) : false; }, [this](const bool bValue) { SetShouldRun(bValue); }, true);
			break;
		case(EBotDecision::EQUIP_WEAPON):
			LocoDecision->SetupDecision([this]() { return CurrentMachineState ? EvaluateCurrentState(CurrentMachineState, EBotDecision::EQUIP_WEAPON) : false; }, [this](const bool bValue) { SetShouldEquip(bValue); }, true);
			break;
		case(EBotDecision::EVADE):
			LocoDecision->SetupDecision([this]() { return CurrentMachineState ? EvaluateCurrentState(CurrentMachineState, EBotDecision::EVADE) : false; }, [this](const bool bValue) { SetShouldEvade(bValue); }, true);
			break;
		case(EBotDecision::ATTACK):
			LocoDecision->SetupDecision([this]() { return CurrentMachineState ? EvaluateCurrentState(CurrentMachineState, EBotDecision::ATTACK) : false; }, [this](const bool bValue) { SetShouldAttack(bValue); }, true);
			break;
		case(EBotDecision::BLOCK):
			LocoDecision->SetupDecision([this]() { return CurrentMachineState ? EvaluateCurrentState(CurrentMachineState, EBotDecision::BLOCK) : false; }, [this](const bool bValue) { SetShouldBlock(bValue); }, true);
			break;
		case(EBotDecision::LOCK_TARGET):
			LocoDecision->SetupDecision([this]() { return CurrentMachineState ? EvaluateCurrentState(CurrentMachineState, EBotDecision::LOCK_TARGET) : false; }, [this](const bool bValue) { SetShouldTarget(bValue); }, true);
			break;
		case(EBotDecision::UNBLOCK_PATH):
			LocoDecision->SetupDecision([this]() { return CurrentMachineState ? EvaluateCurrentState(CurrentMachineState, EBotDecision::UNBLOCK_PATH) : false; }, [this](const bool bValue) { SetShouldUnblockPath(bValue); }, bShouldUnblockPath);
			break;
		case(EBotDecision::STAY_IN_IDLE):
			LocoDecision->SetupDecision([this]() { return CurrentMachineState ? EvaluateCurrentState(CurrentMachineState, EBotDecision::STAY_IN_IDLE) : false; }, [this](const bool bValue) { SetStayInIdle(bValue); }, true);
			break;
		default:
			break;
	}

	LocomotionDecisions.Add(LocoDecision);
}

void UStateMachineBrainComponent::ChangeState()
{
	
	if (StatesFactory) return;

	UBaseMachineState* NewState = StatesFactory->GetMachineState(Bot->GetCurrentAIState());

	if (NewState == nullptr) { FLogger::LogTrace(this->GetOwner()->GetName() + FString(" :: StateMachine.ChageState() :: NewState == nullptr")); return; }

	UpdateState(NewState);
}

void UStateMachineBrainComponent::ForceChangeState(EMachineState NewState)
{
	UBaseMachineState* State = StatesFactory->GetMachineState(NewState);

	if (State == nullptr) { FLogger::LogTrace(this->GetOwner()->GetName() + FString(" :: StateMachine.ForceChageState() :: NewState == nullptr")); return; }

	UpdateState(State);
}
