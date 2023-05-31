// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../../AI/CommonUtility/Factions/GameFaction.h"
#include "../../Enums/Characters/ECharFaction.h"
#include "Factioneable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFactioneable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PRAISE_API IFactioneable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual ECharFaction FactionID() const = 0;
	virtual AGameFaction* GetCharFaction() const = 0;
	virtual void SetFaction(AGameFaction* NewFaction) = 0;
	virtual bool IsFriendTarget(IFactioneable* Target) const = 0;
	virtual bool IsEnemyTarget(IFactioneable* Target) const = 0;
	virtual TArray<ECharFaction> GetIgnoredFactions() const = 0;
	virtual TArray<AActor*> GetPersonalFriends() const = 0;
	virtual TArray<AActor*> GetPersonalEnemies() const = 0;
	virtual void AddPersonalFoE(AActor* FoE, bool bIsFriend) = 0;

};
