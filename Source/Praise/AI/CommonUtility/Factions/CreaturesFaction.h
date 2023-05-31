// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFaction.h"
#include "CreaturesFaction.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API ACreaturesFaction : public AGameFaction
{
	GENERATED_BODY()
	
public:
	ACreaturesFaction();

protected:
	virtual void CacheBotBPs() override;

};
