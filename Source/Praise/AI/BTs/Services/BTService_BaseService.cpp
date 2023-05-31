// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_BaseService.h"

UBTService_BaseService::UBTService_BaseService()
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
}

void UBTService_BaseService::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	CacheMembers(OwnerComp);
}

void UBTService_BaseService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}

void UBTService_BaseService::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	//ClearMembers();
}

void UBTService_BaseService::CacheMembers(UBehaviorTreeComponent& OwnerComp)
{
	if (!OwnerComp.GetOwner()->IsA<ABTBotController>()) return;

	BTController = Cast<ABTBotController>(OwnerComp.GetOwner());

	if (!BTController->GetPawn()->IsA<ABaseBotCharacter>()) return;

	BTPawn = Cast<ABaseBotCharacter>(BTController->GetPawn());

	if (!BTController->HasBrainOfType<UBTBrainComponent>()) return;

	BotBrain = BTController->GetBrainAs<UBTBrainComponent>();

	if (!BTPawn->GetCharStats<UBotStatsComponent>()) return;

	BotStats = BTPawn->GetCharStats<UBotStatsComponent>();
}

bool UBTService_BaseService::CanExecute()
{
	return BTController && BTPawn && BotBrain && BotStats;
}

void UBTService_BaseService::ClearMembers()
{
	BTController = nullptr;
	BTPawn = nullptr;
	BotBrain = nullptr;
	BotStats = nullptr;
}