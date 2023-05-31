// Fill out your copyright notice in the Description page of Project Settings.


#include "BackupRequestMsgCommand.h"
#include "../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../Characters/Player/PraisePlayerCharacter.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../../Components/GameMode/MsgCommandsFactoryComponent.h"
#include "../../Enums/AI/EChaseMode.h"
#include "../../Enums/AI/EMachineState.h"

UBackupRequestMsgCommand::UBackupRequestMsgCommand() : Super()
{

}

void UBackupRequestMsgCommand::Copy(UBaseMsgCommand* Original)
{
	Super::Copy(Original);

	if (!Original->IsA<UBackupRequestMsgCommand>()) return;
}

void UBackupRequestMsgCommand::ExecuteCommand()
{
	if (!CheckRange()) return;

	if (Receiver->IsA<ABaseBotCharacter>())
	{
		ABaseBotCharacter* Bot = Cast<ABaseBotCharacter>(Receiver);

		if (Bot->GetBotClass() == EBotClass::FIGHTER)
		{
			if (Bot->GetBotBrain<UStateMachineBrainComponent>())
			{
				UStateMachineBrainComponent* StateMachine = Bot->GetBotBrain<UStateMachineBrainComponent>();
				
				if(StateMachine->GetCurrentState() == EMachineState::COMBAT) return; 

				if (StateMachine->GetCurrentState() == EMachineState::CHASE && StateMachine->HasKnownEnemies()) return; 
			}

			HandleBackupRequest();
		}
		
	}
	if (Receiver->IsA<APraisePlayerCharacter>())
	{
		// TODO HUD mark
	}

	Super::ExecuteCommand();
}

void UBackupRequestMsgCommand::HandleBackupRequest()
{
	if (Receiver->IsA<ABaseBotCharacter>())
	{
		ABaseBotCharacter* Bot = Cast<ABaseBotCharacter>(Receiver);

		if (!Bot->GetBrain()) return;

		if (!Bot->GetBrain()->GetBotBB()) return;

		FVector ToTarget = Sender->GetActorLocation() - Receiver->GetActorLocation();
		
		if(!Receiver->IsTargetInsideFOV(Sender, false))
			Receiver->SetActorRotation(ToTarget.Rotation());
		
		if (Bot->GetBotBrain<UStateMachineBrainComponent>())
		{
			UStateMachineBrainComponent* StatesMachine = Bot->GetBotBrain<UStateMachineBrainComponent>();

			switch (StatesMachine->GetCurrentState())
			{
				case(EMachineState::CHASE):
					if(Bot->GetCharStatus() != ECharStatus::ALERTED)
						Bot->UpdateCharStatus(ECharStatus::ALERTED);
					break;
				case(EMachineState::WANDER):
					if (ToTarget.Size() > StatesMachine->GetBotBB()->GetValueAsFloat(BB_WANDERING_RADIUS) || !Bot->CanSeeTarget(Sender))
					{
						if (Bot->GetCharStatus() != ECharStatus::ALERTED)
							Bot->UpdateCharStatus(ECharStatus::ALERTED);

						StatesMachine->SetChaseMode(EChaseMode::PURSUE, 5.f);
						StatesMachine->SwitchAIState(EMachineState::CHASE);
					}
					break;
				default:

					UpdateTargetLocation();
					StatesMachine->SetChaseMode(EChaseMode::PURSUE, 5.f);
					StatesMachine->SwitchAIState(EMachineState::CHASE);
					break;
			}
		}
	}
}

void UBackupRequestMsgCommand::UpdateTargetLocation()
{
	if (!Receiver->IsA<ABaseBotCharacter>()) return;

	ABaseBotCharacter* Bot = Cast<ABaseBotCharacter>(Receiver);
	
	bool bCanSeeTarget = false;

	FVector ToTarget = Sender->GetActorLocation() - Receiver->GetActorLocation();

	bool bIsPingedMsg = false;

	if (Bot->GetBotBrain<UStateMachineBrainComponent>())
	{
		if (Bot->GetBotBrain<UStateMachineBrainComponent>()->GetCurrentState() != EMachineState::RUNAWAY)
			bIsPingedMsg = true;
	}

	bCanSeeTarget = Bot->CanSeeTarget(Sender);

	if (bCanSeeTarget && !bIsPingedMsg)
	{
		Bot->GetBrain()->GetBotBB()->SetValueAsVector(BB_PRIORITY_LOCATION, Sender->GetActorLocation());
	}
	else
	{
		Bot->GetBrain()->GetBotBB()->SetValueAsVector(BB_PRIORITY_LOCATION, MsgOriginLocation);
	}
}
