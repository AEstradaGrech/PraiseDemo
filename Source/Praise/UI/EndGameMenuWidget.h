// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMenuWidget.h"
#include "EndGameMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UEndGameMenuWidget : public UBaseMenuWidget
{
	GENERATED_BODY()
	
private:

	UPROPERTY(meta = (BindWidget))
		class UButton* StatsBtn;
	UPROPERTY(meta = (BindWidget))
		class UButton* BackToMainMenuBtn;
	UPROPERTY(meta = (BindWidget))
		class UButton* CloseGameBtn;
	UPROPERTY(meta = (BindWidget))
		class UButton* StatsBackBtn;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* MainSwitcher;

	FTimerHandle BackToMenuTimerHandle;

	virtual bool Initialize();

	UFUNCTION()
		void OnGameStatsBtnClick();
	UFUNCTION()
		void OnBackToMenuBtnClick();
	UFUNCTION()
		void OnCloseGameBtnClick();

	UFUNCTION()
		void OnStatsMenuBackBtnClick();

	UFUNCTION()
		void GoBackToMainMenu();
};
