// Fill out your copyright notice in the Description page of Project Settings.


#include "SnitchDecision.h"
#include "../../Characters/AI/BotCharacters/CitizenBotCharacter.h"
#include "../../Characters/AI/BotCharacters/FightingBotCharacter.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"

USnitchDecision::USnitchDecision()
{

}

bool USnitchDecision::HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision)
{
	if (BotBrain->GetKnownAllies().Num() > 0 && CanSnitch() && FUtilities::IsValidVector(BotBrain->GetBotBB()->GetValueAsVector(BB_LAST_KNOWN_TARGET_LOC)))
	{
		
		DecisionOutput.OutputState = EMachineState::SNITCH;
		DecisionOutput.bIsTransitionDecision = true;
		DecisionOutput.DecisionCallback = [this](const bool bValue) { SetSnitchLocation(bValue); };

		return IsChained() ? ChainResult(DecisionOutput) : true;
	}

	return false;
}

void USnitchDecision::SetSnitchLocation(const bool bValue)
{
	FVector Location = FUtilities::IsValidVector(BotBrain->GetBotBB()->GetValueAsVector(BB_LAST_KNOWN_TARGET_LOC)) ?
					   BotBrain->GetBotBB()->GetValueAsVector(BB_LAST_KNOWN_TARGET_LOC) :
					   BotBrain->GetBot<AActor>()->GetActorLocation();

	BotBrain->GetBotBB()->SetValueAsVector(BB_PRIORITY_LOCATION, Location);
}

bool USnitchDecision::CanSnitch()
{
	bool bCanSnitch = false;

	for (AActor* Ally : BotBrain->GetKnownAllies())
	{
		if (Ally && Ally->IsA<AFightingBotCharacter>() && Cast<AFightingBotCharacter>(Ally)->GetBrainType() == EBrainType::DEFAULT)
		{	
			AFightingBotCharacter* FightingBot = Cast<AFightingBotCharacter>(Ally);

			UStateMachineBrainComponent* Brain = FightingBot->GetBotBrain<UStateMachineBrainComponent>();

			if (!Brain) continue;

			if (Brain->GetCurrentState() == EMachineState::COMBAT) continue;

			if (Brain->GetKnownEnemies().Num() > 0) continue;

			bCanSnitch = true;
		}
	}
	return bCanSnitch;
}
