// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTService_BaseService.h"
#include "BTService_SetPriorityLocation.generated.h"

/**
 * PriorityTarget->Loc || CurrentTargetLastKnownLoc
 */
UCLASS()
class PRAISE_API UBTService_SetPriorityLocation : public UBTService_BaseService
{
	GENERATED_BODY()
	
public:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
