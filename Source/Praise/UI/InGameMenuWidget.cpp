// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenuWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Image.h"
#include "../Praise.h"


bool UInGameMenuWidget::Initialize()
{

	if (!Super::Initialize()) return false;

	CloseMenuBtn->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnCloseMenuBtnClick);

	//main switch
	HelpBtn->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnHelpBtnClick);
	BackToMainMenuBtn->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnBackToMenuBtnClick);
	CloseGameBtn->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnCloseGameBtnClick);

	
	// helpmenu
	GeneralHelpBtn->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnGeneralHelpBtnClick);
	CombatHelpBtn->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnCombatHelpBtnClick);
	InventoryHelpBtn->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnInventoryHelpBtnClick);
	ControlsHelpBtn->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnControlsHelpBtnClick);
	HelpBackBtn->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnHelpMenuBackBtnClick);

	//Submenus
	GeneralHelpBackBtn->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnHelpSubMenuBackBtnClick);
	CombatHelpBackBtn->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnHelpSubMenuBackBtnClick);
	InventoryHelpBackBtn->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnHelpSubMenuBackBtnClick);
	ControlsHelpBackBtn->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnHelpSubMenuBackBtnClick);

	return true;
}

// cerrar InGameMenu
void UInGameMenuWidget::OnCloseMenuBtnClick()
{
	if (!MenuInterface) return;

	MenuInterface->TeardownMenu(EGameMenu::INGAME);
}

//MainSwitch btns
void UInGameMenuWidget::OnHelpBtnClick()
{
	if (!MainSwitcher) return;

	MainSwitcher->SetActiveWidgetIndex(1);
}


void UInGameMenuWidget::OnBackToMenuBtnClick()
{
	if (!MenuInterface) return;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(BackToMenuTimerHandle, this, &UInGameMenuWidget::GoBackToMainMenu, 1.f, false);
	}
}

void UInGameMenuWidget::OnCloseGameBtnClick()
{
	if (!MenuInterface) return;

	MenuInterface->Exit(EGameMenu::MAIN); 
}



//SubMenu / HelpMenu
void UInGameMenuWidget::OnGeneralHelpBtnClick()
{
	if (!HelpMenuSwitcher) return;

	HelpMenuSwitcher->SetActiveWidgetIndex(1);
}

void UInGameMenuWidget::OnCombatHelpBtnClick()
{
	if (!HelpMenuSwitcher) return;

	HelpMenuSwitcher->SetActiveWidgetIndex(2);
}

void UInGameMenuWidget::OnInventoryHelpBtnClick()
{
	if (!HelpMenuSwitcher) return;

	HelpMenuSwitcher->SetActiveWidgetIndex(3);
}

void UInGameMenuWidget::OnControlsHelpBtnClick()
{
	if (!HelpMenuSwitcher) return;

	HelpMenuSwitcher->SetActiveWidgetIndex(4);
}

void UInGameMenuWidget::GoBackToMainMenu()
{
	MenuInterface->Exit(EGameMenu::INGAME); 
}

void UInGameMenuWidget::OnHelpMenuBackBtnClick()
{
	if (!MainSwitcher) return;

	MainSwitcher->SetActiveWidgetIndex(0);
}

void UInGameMenuWidget::OnHelpSubMenuBackBtnClick()
{
	if (!HelpMenuSwitcher) return;

	HelpMenuSwitcher->SetActiveWidgetIndex(0);
}





