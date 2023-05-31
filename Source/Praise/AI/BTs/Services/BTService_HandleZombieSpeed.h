// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BTService_BaseService.h"
#include "BTService_HandleZombieSpeed.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UBTService_HandleZombieSpeed : public UBTService_BaseService
{
	GENERATED_BODY()
	
public:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", MinClamp="0"))
		float SprintAwaitSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", MinClamp="0", MaxClamp="0.9"))
		float SprintSecondsDeviation;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		float CurrentAwaitSeconds;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		float LastSprintTimeStamp;

	void HandleWanderingSpeed();
	void HandleChaseTargetSpeed();
};
