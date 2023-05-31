// Fill out your copyright notice in the Description page of Project Settings.


#include "T200BrainComponent.h"
#include "../../../AI/BotDecisions/SeekDecision.h"
#include "../../../AI/BotDecisions/ChaseDecision.h"
#include "../../../AI/BotDecisions/EngageCombatDecision.h"
#include "../../../Characters/AI/BotCharacters/SeekerBotCharacter.h"

UT200BrainComponent::UT200BrainComponent() : Super()
{

}

void UT200BrainComponent::SetupDefaultDecisions()
{
	USeekDecision* SeekDecision = Cast<USeekDecision>(StatesFactory->GetStateDecision(EBotDecision::SEEK));

	if (SeekDecision) {
		SeekDecision->SetBotBrain(this);

		if (Bot->IsA<ASeekerBotCharacter>())
		{
			ASeekerBotCharacter* SeekerBot = Cast<ASeekerBotCharacter>(Bot);
			if (SeekerBot->IsSeekingOnlyPlayers())
			{
				SeekDecision->SetIsSeekingAnyTarget(false);
				SeekDecision->SetIsSeekingLivingTargets(false);
				SeekDecision->SetTargetType(ETargetType::PLAYER);
				SeekDecision->SetShouldDoFactionCheck(false);
				SeekDecision->SetTargetAsPersonalEnemy(true);
			}

			else SeekDecision->SetIsSeekingLivingTargets(true);
			
			SeekDecision->SetShouldSeekClosestTarget(SeekerBot->IsSeekingClosestTarget());

			if (SeekerBot->GetSearchDistance() <= 0)
			{
				SeekDecision->SetIsLimitedDistance(false);
			}
			else
			{
				SeekDecision->SetIsLimitedDistance(true);
				SeekDecision->SetSearchRadius(SeekerBot->GetSearchDistance());
			}
			SeekDecision->SetTargetType(ETargetType::PLAYER);

			BotBB->SetValueAsBool(BB_CAN_WANDER, SeekerBot->ShouldWanderIfNoTargets());
		}
		else
		{
			SeekDecision->SetIsSeekingLivingTargets(true);
			SeekDecision->SetShouldSeekClosestTarget(true);
			SeekDecision->SetIsLimitedDistance(false);
			SeekDecision->SetTargetType(ETargetType::PLAYER);
		}
		

		CachedDefaultDecisions.Add(SeekDecision);	
	}


	UEngageCombatDecision* EngageCombatDecision = Cast<UEngageCombatDecision>(StatesFactory->GetStateDecision(EBotDecision::ENGAGE_COMBAT));

	if (EngageCombatDecision) 
	{
		EngageCombatDecision->SetBotBrain(this);
		SeekDecision->ChainDecision(EngageCombatDecision);
	}

	UChaseDecision* ChaseDecision = Cast<UChaseDecision>(StatesFactory->GetStateDecision(EBotDecision::CHASE_TARGET));

	if (ChaseDecision)
	{
		ChaseDecision->SetBotBrain(this);
		EngageCombatDecision->ChainDecision(ChaseDecision);
	}
}
