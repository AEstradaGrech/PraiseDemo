// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BotBrainComponent.h"
#include "BTBrainComponent.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UBTBrainComponent : public UBotBrainComponent
{
	GENERATED_BODY()
	
public:
	UBTBrainComponent();
	virtual void SetBrainEnabled(bool bIsEnabled) override;
private:
	virtual void BeginPlay() override;
	virtual bool InitBrain(ABaseBotController* OwnerController, ABaseBotCharacter* BrainOwner);
	virtual void SetDefaults() override;
	virtual void SetupBB() override;
	virtual void ClearBB() override;
	
};
