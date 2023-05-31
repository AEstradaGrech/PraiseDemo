// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectTargetDecision.h"
#include "../../Enums/AI/EChaseMode.h"
USelectTargetDecision::USelectTargetDecision() : Super()
{

}

bool USelectTargetDecision::HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision)
{
	Super::HasDecided(DecisionOutput, PreviousDecision);

	if (BotBrain->GetPriorityTarget()) 
		if (Cast<ITargeteable>(BotBrain->GetPriorityTarget())->IsDead()) 
			BotBrain->GetBotBB()->ClearValue(BB_PRIORITY_TARGET);
	
	if (!BotBrain->GetPriorityTarget() || (BotBrain->LastTargetUpdateTimestamp == 0 || BotBrain->LastTargetUpdateTimestamp + BotBrain->GetTargetUpdateSecs() >= BotBrain->GetTimeSinceGameStart()))
	{
		if (!BotBrain->GetBotBB()->GetValueAsBool(BB_SELECT_CLOSEST_FOE) && HasBlackboardValues())
		{
			if (BotBrain->GetBotBB()->GetValueAsBool(BB_LOCATION_OVER_TARGET))
			{
				if (FUtilities::IsValidVector(BotBrain->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION)))
					return IsChained() ? ChainResult(DecisionOutput) : false;
				else
				{
					if (FUtilities::IsValidVector(BotBrain->GetBotBB()->GetValueAsVector(BB_LAST_KNOWN_TARGET_LOC))) {
						return SetPriorityLocationAndReturn(DecisionOutput, BotBrain->GetBotBB()->GetValueAsVector(BB_LAST_KNOWN_TARGET_LOC));
					}

					if (FUtilities::IsValidVector(BotBrain->GetBotBB()->GetValueAsVector(BB_LAST_DAMAGER_TO_BOT_VECTOR))) {
						FVector AvoidedDir = BotBrain->GetBotBB()->GetValueAsVector(BB_LAST_DAMAGER_TO_BOT_VECTOR);

						FVector AvoidedLoc = BotBrain->GetBot<AActor>()->GetActorLocation() + AvoidedDir * 100.f;

						return SetPriorityLocationAndReturn(DecisionOutput, AvoidedLoc);
					}
				}
			}

			if (GetBlackboardTarget(BB_MAX_AGGRO_TARG))
			{
				return SetPriorityTargetAndReturn(DecisionOutput, GetBlackboardTarget(BB_MAX_AGGRO_TARG));
			}
			if (GetBlackboardTarget(BB_LAST_DAMAGER))
			{
				return SetPriorityTargetAndReturn(DecisionOutput, GetBlackboardTarget(BB_LAST_DAMAGER));
			}
			if (GetBlackboardTarget(BB_LAST_SEEN_TARGET))
			{
				return SetPriorityTargetAndReturn(DecisionOutput, GetBlackboardTarget(BB_LAST_SEEN_TARGET));
			}
		}

		else
		{
			AActor* ClosestTarget = nullptr;
			if (BotBrain->GetKnownEnemies().Num() > 0)
			{
				ClosestTarget = Cast<AActor>(BotBrain->GetClosestFoE(true, BotBrain->GetBotBB()->GetValueAsBool(BB_DEAD_TARGETS_ALLOWED)));

				return SetPriorityTargetAndReturn(DecisionOutput, ClosestTarget);
			}
			else
			{
				if (BotBrain->GetBotBB()->GetValueAsBool(BB_HAS_BEEN_DAMAGED))
				{
					DecisionOutput.OutputState = EMachineState::WANDER;
					BotBrain->SetWanderMode(EWanderMode::INSPECT, 30.f);
					DecisionOutput.bIsTransitionDecision = true;

					return true;
				}
			}
		}

		BotBrain->LastTargetUpdateTimestamp = BotBrain->GetTimeSinceGameStart();
	}

	return IsChained() ? ChainResult(DecisionOutput) : false;
}

AActor* USelectTargetDecision::GetBlackboardTarget(FName Key)
{
	return Cast<AActor>(BotBrain->GetBotBB()->GetValueAsObject(Key));
}

bool USelectTargetDecision::HasBlackboardValues() const
{
	return BotBrain->HasValidBBTarget(BB_MAX_AGGRO_TARG) || 
		   BotBrain->HasValidBBTarget(BB_LAST_DAMAGER) || 
		   BotBrain->HasValidBBTarget(BB_LAST_SEEN_TARGET) ||
		   FUtilities::IsValidVector(BotBrain->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION));
}

bool USelectTargetDecision::SetPriorityTargetAndReturn(FDecisionOutput& DecisionOutput, AActor* Target)
{
	BotBrain->SetPriorityTarget(Target);

	if (!IsChained())
	{
		DecisionOutput.bIsTransitionDecision = false;
		return true;
	}

	else return ChainResult(DecisionOutput);
}

bool USelectTargetDecision::SetPriorityLocationAndReturn(FDecisionOutput& DecisionOutput, FVector Location)
{
	BotBrain->GetBotBB()->SetValueAsVector(BB_PRIORITY_LOCATION, Location);

	if (!IsChained())
	{
		DecisionOutput.bIsTransitionDecision = false;

		return true;
	}

	else return ChainResult(DecisionOutput);;
}
