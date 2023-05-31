// Fill out your copyright notice in the Description page of Project Settings.


#include "BTBrainComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "../../../Characters/AI/BTBotController.h"

UBTBrainComponent::UBTBrainComponent() : Super()
{
	
}

void UBTBrainComponent::SetBrainEnabled(bool bIsEnabled)
{
	Super::SetBrainEnabled(bIsEnabled);

	if (!bIsEnabled)
	{
		if (!BotController) return;
			
		if (!BotController->IsA<ABTBotController>()) return;

		ABTBotController* BTController = Cast<ABTBotController>(BotController);

		BTController->GetBotBTComp()->StopTree();
	}
}

void UBTBrainComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UBTBrainComponent::InitBrain(ABaseBotController* OwnerController, ABaseBotCharacter* BrainOwner)
{
	if (!OwnerController->IsA<ABTBotController>()) return false;

	if (!Super::InitBrain(OwnerController, BrainOwner)) return false;

	ABTBotController* BTController = Cast<ABTBotController>(OwnerController);

	if (!BTController->TryInitBT()) return false;

	BotBB = BTController->GetBotBB();

	return true;
}

void UBTBrainComponent::SetDefaults()
{
	
}

void UBTBrainComponent::SetupBB()
{
	if (!BotBB) return;

	BotBB->SetValueAsVector(BB_SPAWN_LOC, Bot->GetSpawnLocation());
	BotBB->SetValueAsBool(BB_IS_ROARING, false);
	BotBB->SetValueAsBool(BB_IS_PAUSED, false);
	BotBB->SetValueAsBool(BB_CAN_ATTACK, true);
}

void UBTBrainComponent::ClearBB()
{
	if (!BotBB) return;

	BotBB->ClearValue(BB_PRIORITY_TARGET);
	BotBB->ClearValue(BB_PRIORITY_LOCATION);
	BotBB->ClearValue(BB_LAST_KNOWN_TARGET_LOC);
	BotBB->ClearValue(BB_IS_ROARING);
}

