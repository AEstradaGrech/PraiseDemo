// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SetPriorityLocation.h"

#include "../../../Interfaces/CombatSystem/Targeteable.h"

void UBTService_SetPriorityLocation::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (!CanExecute()) return;

	if (!BotBrain->GetBotBB()) return;
}

void UBTService_SetPriorityLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!BotBrain->HasValidBBTarget(BB_PRIORITY_TARGET)) return;

	AActor* Target = BotBrain->GetPriorityTarget();

	if (BTPawn->CanSeeTarget(Target, true, true))
	{
		BotBrain->SetPriorityTarget(Target);
		BotBrain->GetBotBB()->SetValueAsVector(BB_PRIORITY_LOCATION, Target->GetActorLocation());
		DrawDebugSphere(BTPawn->GetWorld(), Target->GetActorLocation(), 50.f, 8, FColor::Magenta, false, 5.f);
	}
}