// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBuilding.h"

#include "PrivateBuilding.generated.h"

UCLASS()
class PRAISE_API APrivateBuilding : public ABaseBuilding
{
	GENERATED_BODY()
	
public:
	APrivateBuilding();

	FORCEINLINE void SetAllowNonOwnersIn(bool bValue) { bAllowNonOwnersIn = bValue; }
	FORCEINLINE void SetIsOpen(bool bValue) { bIsOpen = bValue; }

protected:
	virtual void BeginPlay() override;
};
