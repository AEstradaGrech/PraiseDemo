// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaseDecision.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../../Components/Actor/AI/T200BrainComponent.h"
#include "../../Enums/AI/EChaseMode.h"

UChaseDecision::UChaseDecision() : Super()
{

}

bool UChaseDecision::HasDecided(FDecisionOutput& Output, UBotDecision* PreviousDecision) {

	Super::HasDecided(Output, PreviousDecision);

	if (BotBrain->GetBotBB()->GetValueAsBool(BB_HAS_FOLLOW_REQ)) 
	{
		if (BotBrain->GetPriorityTarget()) {
			Output.OutputState = EMachineState::CHASE;
			Output.bIsTransitionDecision = true;
			return IsChained() ? ChainResult(Output) : true;;
		}
	}
	else
	{

		switch (Cast<UStateMachineBrainComponent>(BotBrain)->GetCurrentState())
		{
			case(EMachineState::PATROL): 
			case(EMachineState::WANDER): 
			case(EMachineState::RUNAWAY):
			default:
				return HandleDefaultCase(Output, PreviousDecision);
			case(EMachineState::CHASE):
				return HandleChaseCase(Output, PreviousDecision);
			case(EMachineState::COMBAT):
				return HandleCombatCase(Output, PreviousDecision);
		}
	}

	return false;
}

bool UChaseDecision::HandleDefaultCase(FDecisionOutput& Output, UBotDecision* PreviousDecision)
{
	if (!CanChase(true)) return false;

	EChaseMode Mode = BotBrain->IsA<UT200BrainComponent>() ? EChaseMode::SEEK : EChaseMode::PURSUE;
	
	float TriggerDistance = BotBrain->GetBot<ABasePraiseCharacter>()->GetMaxCombatRange();

	FVector ToTarget = BotBrain->GetPriorityTarget()->GetActorLocation() - BotBrain->GetBot<AActor>()->GetActorLocation();

	if (ToTarget.Size() >= BotBrain->GetBot<ABaseBotCharacter>()->GetPathFindingDistance() && BotBrain->HasValidBBTarget(BB_PRIORITY_TARGET) && BotBrain->GetBotBB()->GetValueAsBool(BB_CAN_TRAVEL))
	{
		BotBrain->GetBotBB()->SetValueAsBool(BB_WANTS_TO_TRAVEL, true);
		BotBrain->GetBotBB()->SetValueAsObject(BB_TRAVEL_DESTINATION, BotBrain->GetPriorityTarget());

		Output.OutputState = EMachineState::TRAVEL;
		Output.bIsTransitionDecision = true;
		return true;
	}

	if (ToTarget.Size() > TriggerDistance)
	{
		BotBrain->SetChaseMode(Mode, BotBrain->GetBotBB()->GetValueAsFloat(BB_MAX_CHASING));
		Output.OutputState = EMachineState::CHASE;
		Output.bIsTransitionDecision = true;
		
		return true;
	}

	else
	{
		if (BotBrain->GetPriorityTarget()->Implements<UFactioneable>() && 
			BotBrain->GetBot<ABaseBotCharacter>()->IsEnemyTarget(Cast<IFactioneable>(BotBrain->GetPriorityTarget())) &&
			BotBrain->GetBot<ABaseBotCharacter>()->CanSeeTarget(BotBrain->GetPriorityTarget(), true))
		{
			Output.OutputState = EMachineState::COMBAT;
			Output.bIsTransitionDecision = true;
			return true;
		}
	}

	return false;
}

bool UChaseDecision::HandleCombatCase(FDecisionOutput& Output, UBotDecision* PreviousDecision)
{
	if (BotBrain->GetBotBB()->GetValueAsObject(BB_LOCATION_OVER_TARGET)) 
	{
		if (BotBrain->GetBotBB()->GetValueAsObject(BB_PRIORITY_LOCATION))
		{

		}
	}
	else
	{
		if (!CanChase(true)) return false;

		if (BotBrain->GetPriorityTarget()->GetClass()->ImplementsInterface(UCombatible::StaticClass()))
		{
			ABaseBotCharacter* Bot = BotBrain->GetBot<ABaseBotCharacter>();

			ICombatible* Combatible = Cast<ICombatible>(BotBrain->GetPriorityTarget());

			FVector ToTarg = BotBrain->GetPriorityTarget()->GetActorLocation() - BotBrain->GetBot<AActor>()->GetActorLocation();

			if (ToTarg.Size() <= BotBrain->GetBot<ABaseBotCharacter>()->GetMinMeleeDistance()) return false;

			if (BotBrain->GetBot<ABaseBotCharacter>()->GetCurrentMainWeapon() && BotBrain->GetBot<ABaseBotCharacter>()->GetCurrentMainWeapon()->GetSlotWeapon())
			{
				EChaseMode Mode = BotBrain->IsA<UT200BrainComponent>() ? EChaseMode::SEEK : EChaseMode::PURSUE;

				
				if (!Bot->IsInCombatRange(BotBrain->GetPriorityTarget(), Bot->GetMeleeDistanceOffset() + 120.f) && !Bot->IsEvading() && !Bot->IsRolling())
				{
					Output.OutputState = EMachineState::CHASE; 
					Output.bIsTransitionDecision = true;
					BotBrain->SetChaseMode(Mode, BotBrain->GetBotBB()->GetValueAsFloat(BB_MAX_CHASING));
					
					return  true;
				}
				
				else return false;
			}
		}
	}

	return false;
}

bool UChaseDecision::HandleChaseCase(FDecisionOutput& Output, UBotDecision* PreviousDecision)
{
	EChaseMode CurrentChaseMode = (EChaseMode)BotBrain->GetBotBB()->GetValueAsInt(BB_CHASING_MODE);

	switch (CurrentChaseMode)
	{
		case(EChaseMode::SEEK):
			return CanChase(true) ? HandleDefaultCase(Output, PrevDecision) : false;

		case(EChaseMode::PURSUE):
			if (CanChase(true))
			{
				BotBrain->GetBotBB()->SetValueAsBool(BB_LOCATION_OVER_TARGET, false);
				BotBrain->GetBotBB()->ClearValue(BB_PRIORITY_LOCATION);
			
				return HandleDefaultCase(Output, PreviousDecision);
			}
			else
			{	
				if (!CanChase(false)) return false;

				if (BotBrain->GetKnownEnemies().Num() > 0)
					BotBrain->GetBotBB()->ClearValue(BB_PRIORITY_LOCATION);

				if(!FUtilities::IsValidVector(BotBrain->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION)))
					BotBrain->GetBotBB()->SetValueAsVector(BB_PRIORITY_LOCATION, BotBrain->GetBotBB()->GetValueAsVector(BB_LAST_KNOWN_TARGET_LOC));
					
				FVector ToTarget = BotBrain->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION) - BotBrain->GetBot<AActor>()->GetActorLocation();

				BotBrain->GetBotBB()->SetValueAsBool(BB_LOCATION_OVER_TARGET, !BotBrain->HasValidBBTarget(BB_PRIORITY_TARGET) && BotBrain->GetKnownEnemies().Num() <= 0);

				BotBrain->SetChaseMode(EChaseMode::PURSUE, BotBrain->GetBotBB()->GetValueAsFloat(BB_MAX_CHASING));

				Output.OutputState = EMachineState::CHASE;
				Output.bIsTransitionDecision = false; 

				return true;
				
			}			
			
			break;
		case(EChaseMode::FOLLOW):
		default:
			break;
	}

	return false;
}

bool UChaseDecision::CanChase(bool bCheckTarget)
{
	if(!BotBrain->GetPriorityTarget() && 
	   !FUtilities::IsValidVector(BotBrain->GetBotBB()->GetValueAsVector(BB_LAST_KNOWN_TARGET_LOC)) && 
	   !FUtilities::IsValidVector(BotBrain->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION)))
		return false;

	if (bCheckTarget)
	{
		if (BotBrain->HasValidBBTarget(BB_PRIORITY_TARGET))
		{
			if ((EChaseMode)BotBrain->GetBotBB()->GetValueAsInt(BB_CHASING_MODE) == EChaseMode::SEEK) return true;

			if (BotBrain->GetBot<ABasePraiseCharacter>()->IsTargetInsideFOV(BotBrain->GetPriorityTarget(), true))
			{
				return (BotBrain->GetPriorityTarget()->GetActorLocation() - BotBrain->GetBot<AActor>()->GetActorLocation()).Size() <= BotBrain->GetBot<ABasePraiseCharacter>()->GetCharStats()->GetSightRange();		
			}

			else return false;
		}
			

		return BotBrain->HasValidBBTarget(BB_PRIORITY_TARGET) ? true : false;
	}
	else
	{
		if (!FUtilities::IsValidVector(BotBrain->GetBotBB()->GetValueAsVector(BB_LAST_KNOWN_TARGET_LOC)) &&
			!FUtilities::IsValidVector(BotBrain->GetBotBB()->GetValueAsVector(BB_PRIORITY_LOCATION)))
			return false;
	}
	
	return true;
}
