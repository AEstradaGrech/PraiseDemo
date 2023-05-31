// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSeenMsgCommand.h"
#include "../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../Characters/Player/PraisePlayerCharacter.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../../Enums/AI/EChaseMode.h"
#include "../../Enums/AI/EMachineState.h"

UTargetSeenMsgCommand::UTargetSeenMsgCommand() : Super()
{

}

void UTargetSeenMsgCommand::Copy(UBaseMsgCommand* Original)
{
	Super::Copy(Original);

	if (!Original->IsA<UTargetSeenMsgCommand>()) return;

	TargetLocation = Cast<UTargetSeenMsgCommand>(Original)->GetTargetLocation();
}


void UTargetSeenMsgCommand::ExecuteCommand()
{
	Super::ExecuteCommand();

	if (!CheckRange()) return;

	FVector ToTarget = TargetLocation - Receiver->GetActorLocation();

	if (Receiver->IsA<ABaseBotCharacter>())
	{
		ABaseBotCharacter* Bot = Cast<ABaseBotCharacter>(Receiver);

		Bot->GetBrain()->GetBotBB()->SetValueAsVector(BB_PRIORITY_LOCATION, TargetLocation);

		if (Bot->GetBotBrain<UStateMachineBrainComponent>())
		{
			UStateMachineBrainComponent* StatesMachine = Bot->GetBotBrain<UStateMachineBrainComponent>();

			switch (StatesMachine->GetCurrentState())
			{
				case(EMachineState::COMBAT):
					return;
				case(EMachineState::CHASE):
					if(Bot->GetCharStatus() != ECharStatus::ALERTED)
						Bot->UpdateCharStatus(ECharStatus::ALERTED);
					break;
				case(EMachineState::WANDER):
					if (ToTarget.Size() > StatesMachine->GetBotBB()->GetValueAsFloat(BB_WANDERING_RADIUS))
					{
					
						if (Bot->GetCharStatus() != ECharStatus::ALERTED)
							Bot->UpdateCharStatus(ECharStatus::ALERTED);

						StatesMachine->GetBotBB()->SetValueAsInt(BB_CHASING_MODE, (int)EChaseMode::PURSUE);
						StatesMachine->SwitchAIState(EMachineState::CHASE);
					}
					break;
				default:
					StatesMachine->GetBotBB()->SetValueAsInt(BB_CHASING_MODE, (int)EChaseMode::PURSUE);
					StatesMachine->SwitchAIState(EMachineState::CHASE);
					break;
			}
		}
	}

	if (Receiver->IsA<ABasePraiseCharacter>())
	{

	}
}