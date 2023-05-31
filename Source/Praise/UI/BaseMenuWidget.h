// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Interfaces/UI/MenuInterface.h"
#include "BaseMenuWidget.generated.h"

/**
 * 
 */

UCLASS()
class PRAISE_API UBaseMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FORCEINLINE void SetMenuInterface(IMenuInterface* Interface) { MenuInterface = Interface; }

	void RenderMenu();
	void TeardownMenu();
	
protected:
	IMenuInterface* MenuInterface;

};
