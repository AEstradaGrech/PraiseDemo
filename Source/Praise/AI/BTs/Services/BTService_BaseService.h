// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "../../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../../Characters/AI/BTBotController.h"
#include "../../../Components/Actor/CharStats/BotStatsComponent.h"
#include "../../../Components/Actor/AI/BTBrainComponent.h"
#include "BTService_BaseService.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UBTService_BaseService : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_BaseService();

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:

	void CacheMembers(UBehaviorTreeComponent& OwnerComp);
	bool CanExecute();
	void ClearMembers();
	ABTBotController* BTController;
	ABaseBotCharacter* BTPawn;
	UBotStatsComponent* BotStats;
	UBTBrainComponent* BotBrain;
};
