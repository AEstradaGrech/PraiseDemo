// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBotController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTBotController.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API ABTBotController : public ABaseBotController
{
	GENERATED_BODY()

public:
	ABTBotController();

	FORCEINLINE UBehaviorTreeComponent* GetBotBTComp() const { return BotBTComp; }
	FORCEINLINE UBlackboardComponent* GetBotBB() const { return BotBlackboard; }
	virtual void OnPossess(APawn* InPawn) override;
	bool TryInitBT();
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UBehaviorTreeComponent* BotBTComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UBlackboardComponent* BotBlackboard;
};
