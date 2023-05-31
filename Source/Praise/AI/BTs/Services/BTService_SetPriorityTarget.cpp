// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetPriorityTarget.h"
#include "../../../Interfaces/CombatSystem/Targeteable.h"

void UBTService_SetPriorityTarget::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (!CanExecute()) return;

	if (!BotBrain->GetBotBB()) return;

	TargetLostSeconds = 0;
}

void UBTService_SetPriorityTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AActor* CurrentTarget = BotBrain->GetPriorityTarget();

	if (CurrentTarget)
	{
		if (!BTPawn->CanSeeTarget(CurrentTarget, true))
			TargetLostSeconds += DeltaSeconds;

		if (TargetLostSeconds >= ClearTargetSeconds || Cast<ITargeteable>(CurrentTarget)->IsDead())
		{
			BotBrain->GetBotBB()->ClearValue(BB_PRIORITY_TARGET);
			TargetLostSeconds = 0;
		}
	}
		

	ITargeteable* Target = BotBrain->GetClosestFoE();

	if (!Target) return;
	/*{
		BotBrain->GetBotBB()->ClearValue(BB_PRIORITY_TARGET);

		if(BotBrain->GetBot<ABaseBotCharacter>()->IsWeaponEquiped())
			BotBrain->GetBot<ABaseBotCharacter>()->EquipWeapons();

		if (BTPawn->GetTargetingComponent() && BTPawn->GetTargetingComponent()->IsTargetLocked())
			BTPawn->GetTargetingComponent()->ClearTarget();
	}*/
	//else
	//{
		if (!CurrentTarget || Target->GetTargetID() != CurrentTarget->GetUniqueID())
		{
			BotBrain->SetPriorityTarget(Cast<AActor>(Target));

			BTPawn->UpdateCharStatus(ECharStatus::ALERTED);

			if (!BotBrain->GetBot<ABaseBotCharacter>()->IsWeaponEquiped())
				BotBrain->GetBot<ABaseBotCharacter>()->EquipWeapons();

			if (BTPawn->GetTargetingComponent())
			{
				if (BTPawn->GetTargetingComponent()->IsTargetLocked() && BTPawn->GetTargetingComponent()->GetCurrentTarget() == Cast<AActor>(Target)) return;

				BTPawn->GetTargetingComponent()->ForceLockTarget(Cast<AActor>(Target), false);
			}
		}		
	//}
}