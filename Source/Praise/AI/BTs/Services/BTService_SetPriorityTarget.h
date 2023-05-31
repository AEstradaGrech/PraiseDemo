// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTService_BaseService.h"
#include "BTService_SetPriorityTarget.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UBTService_SetPriorityTarget : public UBTService_BaseService
{
	GENERATED_BODY()
	
public:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float ClearTargetSeconds;

	float TargetLostSeconds;
};
