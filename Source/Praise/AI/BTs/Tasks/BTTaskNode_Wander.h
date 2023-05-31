// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_Wander.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UBTTaskNode_Wander : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "100", ClampMax = "2000"))
		float MaxDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "50", ClampMax = "100"))
		float MinDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "10", ClampMax = "60"))
		float MaxWaitingTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "10"))
		float MinWaitingTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bShouldRoar;
	

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory) override;

private:
	class UNavigationSystemV1* NavMesh;
};
