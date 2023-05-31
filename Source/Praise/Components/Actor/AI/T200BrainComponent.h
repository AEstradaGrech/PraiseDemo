// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateMachineBrainComponent.h"
#include "T200BrainComponent.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UT200BrainComponent : public UStateMachineBrainComponent
{
	GENERATED_BODY()
public:
	UT200BrainComponent();

protected:
	virtual void SetupDefaultDecisions() override;
};
