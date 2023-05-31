// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../../Enums/CommonUtility/EGameMenu.h"
#include "MenuInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMenuInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PRAISE_API IMenuInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void LoadMenu(EGameMenu Menu) = 0;
	virtual void TeardownMenu(EGameMenu Menu) = 0;
	virtual void Exit(EGameMenu Menu) = 0;
	virtual void GoToMap(FName LevelName, bool IsServerTravel = false) = 0;
};
