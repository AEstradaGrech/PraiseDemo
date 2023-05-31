// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMenuWidget.h"
#include "InGameMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UInGameMenuWidget : public UBaseMenuWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
		class UButton* CloseMenuBtn;

	UPROPERTY(meta = (BindWidget))
		class UButton* GeneralHelpBtn;
	UPROPERTY(meta = (BindWidget))
		class UButton* GeneralHelpBackBtn;
	UPROPERTY(meta = (BindWidget))
		class UButton* CombatHelpBtn;
	UPROPERTY(meta = (BindWidget))
		class UButton* CombatHelpBackBtn;
	UPROPERTY(meta = (BindWidget))
		class UButton* InventoryHelpBtn;
	UPROPERTY(meta = (BindWidget))
		class UButton* InventoryHelpBackBtn;
	UPROPERTY(meta = (BindWidget))
		class UButton* ControlsHelpBtn;
	UPROPERTY(meta = (BindWidget))
		class UButton* ControlsHelpBackBtn;
	UPROPERTY(meta = (BindWidget))
		class UButton* HelpBtn;

	//MainSwitch
	UPROPERTY(meta = (BindWidget))
		class UButton* HelpBackBtn;
	UPROPERTY(meta = (BindWidget))
		class UButton* BackToMainMenuBtn;
	UPROPERTY(meta = (BindWidget))
		class UButton* CloseGameBtn;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* MainSwitcher;
	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* HelpMenuSwitcher;

	FTimerHandle BackToMenuTimerHandle;

	virtual bool Initialize();

	UFUNCTION()
		void OnHelpBtnClick();
	UFUNCTION()
		void OnCloseGameBtnClick();
	
	UFUNCTION()
		void OnCloseMenuBtnClick();
	
	UFUNCTION()
		void OnHelpSubMenuBackBtnClick();

	UFUNCTION()
		void OnBackToMenuBtnClick();
	UFUNCTION()
		void OnHelpMenuBackBtnClick();

	UFUNCTION()
		void OnGeneralHelpBtnClick();
	UFUNCTION()
		void OnCombatHelpBtnClick();
	UFUNCTION()
		void OnInventoryHelpBtnClick();
	UFUNCTION()
		void OnControlsHelpBtnClick();

	UFUNCTION()
		void GoBackToMainMenu();
};
