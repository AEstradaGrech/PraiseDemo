// Fill out your copyright notice in the Description page of Project Settings.


#include "EngageCombatDecision.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"

UEngageCombatDecision::UEngageCombatDecision() : Super()
{
	bOnlyInCombatRange = false;
}

bool UEngageCombatDecision::HasDecided(FDecisionOutput& Output, UBotDecision* PreviousDecision) 
{
	Super::HasDecided(Output, PreviousDecision);

	if (IsValidTarget() && BotBrain->GetBot<ABaseBotCharacter>()->GetBotController<ABaseBotController>()->GetPathFollowingComponent()->HasValidPath())
	{
			ICombatible* Target = GetTargetAsCombatible();

			if (!Target) return false;

			if (Target->GetHealth() <= 0) return false;

			ABasePraiseCharacter* CharTarget = BotBrain->GetBot<ABasePraiseCharacter>();

			if (BotBrain->GetBot<ABasePraiseCharacter>()->CanSeeTarget(Cast<AActor>(Target), true))
			{
				if (CharTarget->IsInCombatRange(Cast<AActor>(Target), CharTarget->GetMeleeDistanceOffset()))
				{
					Output.OutputState = EMachineState::COMBAT;
					Output.bIsTransitionDecision = Cast<UStateMachineBrainComponent>(BotBrain)->GetCurrentState() != EMachineState::COMBAT;

					return IsChained() ? ChainResult(Output) : true;
				}
				else
				{
					if (bOnlyInCombatRange) return false;

					Output.OutputState = EMachineState::CHASE;
					Output.bIsTransitionDecision = Cast<UStateMachineBrainComponent>(BotBrain)->GetCurrentState() != EMachineState::CHASE;

					return IsChained() ? ChainResult(Output) : true;
				}	
			}
			
			return IsChained() ? ChainResult(Output) : false;
	}
	
	return IsChained() ? ChainResult(Output) : false;
}

AActor* UEngageCombatDecision::GetPriorityTarget()
{
	return Cast<AActor>(BotBrain->GetPriorityTarget());
}

ICombatible* UEngageCombatDecision::GetTargetAsCombatible()
{
	if (!GetPriorityTarget()) return nullptr;

	return GetPriorityTarget()->Implements<UCombatible>() ? Cast<ICombatible>(GetPriorityTarget()) : nullptr;
}

bool UEngageCombatDecision::IsValidTarget()
{
	AActor* Target = GetPriorityTarget();

	if (!Target) return false;

	return Target->Implements<UCombatible>() && Target->Implements<UDamageable>() && !Cast<IDamageable>(Target)->IsDead() && BotBrain->GetBot<ABasePraiseCharacter>()->CanSeeTarget(Target);
}

