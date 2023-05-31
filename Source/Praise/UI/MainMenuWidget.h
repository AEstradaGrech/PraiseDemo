// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMenuWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UMainMenuWidget : public UBaseMenuWidget
{
	GENERATED_BODY()

public:
	UMainMenuWidget(const FObjectInitializer& ObjectInitializer);
private:
	UPROPERTY(meta = (BindWidget))
		class UButton* PlayBtn;
	UPROPERTY(meta = (BindWidget))
		class UButton* OverviewBtn;
	UPROPERTY(meta = (BindWidget))
		class UButton* QuitBtn;
	UPROPERTY(meta = (BindWidget))
		class UButton* OverviewBackBtn;
	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* MainSwitcher;
	

protected:
	virtual bool Initialize();

	UFUNCTION()
		void OnPlayBtnClick();
	UFUNCTION()
		void OnExitBtnClick();
	UFUNCTION()
		void OnOverviewBtnClick();
	UFUNCTION()
		void OnOverviewBackBtnClick();
	UFUNCTION()
		void DoPlayButtonAction();

	FTimerHandle OnPlayBtnTimerHandle;

};
