// Fill out your copyright notice in the Description page of Project Settings.


#include "EndGameMenuWidget.h"
#include "InGameMenuWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Image.h"
#include "../Praise.h"

bool UEndGameMenuWidget::Initialize()
{
	if (!Super::Initialize()) return false;

	StatsBtn->OnClicked.AddDynamic(this, &UEndGameMenuWidget::OnGameStatsBtnClick);
	BackToMainMenuBtn->OnClicked.AddDynamic(this, &UEndGameMenuWidget::OnBackToMenuBtnClick);
	CloseGameBtn->OnClicked.AddDynamic(this, &UEndGameMenuWidget::OnCloseGameBtnClick);

	StatsBackBtn->OnClicked.AddDynamic(this, &UEndGameMenuWidget::OnStatsMenuBackBtnClick);

	return true;
}

void UEndGameMenuWidget::OnGameStatsBtnClick()
{
	if (!MainSwitcher) return;

	MainSwitcher->SetActiveWidgetIndex(1);
}


void UEndGameMenuWidget::OnBackToMenuBtnClick()
{
	if (!MenuInterface) return;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(BackToMenuTimerHandle, this, &UEndGameMenuWidget::GoBackToMainMenu, 1.f, false);
	}
}

void UEndGameMenuWidget::OnCloseGameBtnClick()
{
	if (!MenuInterface) return;

	MenuInterface->Exit(EGameMenu::MAIN); 
}

void UEndGameMenuWidget::OnStatsMenuBackBtnClick()
{
	if (!MainSwitcher) return;

	MainSwitcher->SetActiveWidgetIndex(0);
}


void UEndGameMenuWidget::GoBackToMainMenu()
{
	MenuInterface->Exit(EGameMenu::INGAME); 
}
