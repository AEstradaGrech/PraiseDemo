// Fill out your copyright notice in the Description page of Project Settings.


#include "RunawayDecision.h"
#include "../../Characters/AI/BotCharacters/FightingBotCharacter.h"
#include "../../Enums/AI/EChaseMode.h"
#include "../../Components/Actor/AI/T200BrainComponent.h"

URunawayDecision::URunawayDecision() : Super()
{

}

bool URunawayDecision::HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision)
{
	Super::HasDecided(DecisionOutput, PreviousDecision);

	if (HasBlackboardTargets() || BotBrain->GetKnownEnemies().Num() > 0) 
	{
		if (BotBrain->GetBot<AFightingBotCharacter>() || BotBrain->IsA<UT200BrainComponent>())
		{
			if (BotBrain->GetBot<ABaseBotCharacter>()->HasWeapon()) 
			{
				if (GetCurrentThreat())
				{
					if (GetCurrentThreat()->IsDead()) return false;

					BotBrain->SetPriorityTarget(Cast<AActor>(GetCurrentThreat()));
					DecisionOutput.OutputState = EMachineState::CHASE; 
					BotBrain->SetChaseMode(BotBrain->IsA<UT200BrainComponent>() ? EChaseMode::SEEK : EChaseMode::PURSUE, BotBrain->GetBotBB()->GetValueAsFloat(BB_MAX_CHASING));
					BotBrain->GetBotBB()->SetValueAsInt(BB_MAX_CHASING, 60.f);
					DecisionOutput.bIsTransitionDecision = true;
				}
			}
			else
			{
				if (!BotBrain->GetBot<AFightingBotCharacter>()->ShouldFightToDeath()) 
				{
					DecisionOutput.OutputState = EMachineState::RUNAWAY; 
					DecisionOutput.bIsTransitionDecision = true;
				}
			}
		}
		else
		{
			DecisionOutput.OutputState = EMachineState::RUNAWAY;
			DecisionOutput.bIsTransitionDecision = true;
		}

		return IsChained() ? ChainResult(DecisionOutput) : true;
	}

	return false;
}

bool URunawayDecision::HasBlackboardTargets()
{
	return BotBrain->HasValidBBTarget(BB_LAST_DAMAGER) ||
		   BotBrain->HasValidBBTarget(BB_MAX_AGGRO_TARG) ||
		   BotBrain->HasValidBBTarget(BB_PRIORITY_TARGET) || 
		   FUtilities::IsValidVector(BotBrain->GetBotBB()->GetValueAsVector(BB_LAST_KNOWN_TARGET_LOC)) ||
		   FUtilities::IsValidVector(BotBrain->GetBotBB()->GetValueAsVector(BB_LAST_DAMAGER_TO_BOT_VECTOR));
}

/// <summary>
/// Devuelve BBTarg o el enemigo percibido mas cercano
/// </summary>
/// <returns></returns>
ITargeteable* URunawayDecision::GetCurrentThreat()
{
	ITargeteable* CurrentThreat = GetCurrentBBThreat();

	if (CurrentThreat && !CurrentThreat->IsDead())
		return CurrentThreat;

	if (BotBrain->GetKnownEnemyTargets(BotBrain->GetBotBB()->GetValueAsBool(BB_DEAD_TARGETS_ALLOWED)).Num() > 0)
		return BotBrain->GetClosestTarget(BotBrain->GetKnownEnemyTargets(BotBrain->GetBotBB()->GetValueAsBool(BB_DEAD_TARGETS_ALLOWED)));

	return nullptr;
}

ITargeteable* URunawayDecision::GetCurrentBBThreat()
{
	ITargeteable* Target = nullptr;

	if (HasBlackboardTargets())
	{
		Target = Cast<ITargeteable>(BotBrain->GetBotBB()->GetValueAsObject(BB_LAST_DAMAGER));

		if (Target)
			return Target;

		Target = Cast<ITargeteable>(BotBrain->GetBotBB()->GetValueAsObject(BB_MAX_AGGRO_TARG));

		if (Target)
			return Target;
		
		Target = Cast<ITargeteable>(BotBrain->GetBotBB()->GetValueAsObject(BB_PRIORITY_TARGET));

		if (Target)
			return Target;
	}

	return Target;
}

