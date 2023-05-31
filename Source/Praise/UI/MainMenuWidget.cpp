// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "../Praise.h"
#include "Components/Image.h"


UMainMenuWidget::UMainMenuWidget(const FObjectInitializer& ObjectInitializer)
{

}

bool UMainMenuWidget::Initialize()
{
	
	if (!Super::Initialize()) return false;

	PlayBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::OnPlayBtnClick);

	OverviewBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::OnOverviewBtnClick);

	OverviewBackBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::OnOverviewBackBtnClick);

	QuitBtn->OnClicked.AddDynamic(this, &UMainMenuWidget::OnExitBtnClick);

	return true;
}

void UMainMenuWidget::OnPlayBtnClick()
{
	if (!MenuInterface) return;

	if (!ensure(GetWorld() != nullptr)) return;

	GetWorld()->GetTimerManager().SetTimer(OnPlayBtnTimerHandle, this, &UMainMenuWidget::DoPlayButtonAction, 1.f, false);
}

void UMainMenuWidget::OnExitBtnClick()
{
	if (!MenuInterface) return;

	MenuInterface->Exit(EGameMenu::MAIN);
}

void UMainMenuWidget::OnOverviewBtnClick()
{
	if (!MainSwitcher) return;

	MainSwitcher->SetActiveWidgetIndex(1);
}

void UMainMenuWidget::OnOverviewBackBtnClick()
{
	if (!MainSwitcher) return;

	MainSwitcher->SetActiveWidgetIndex(0);
}

void UMainMenuWidget::DoPlayButtonAction()
{
	if (!MenuInterface) return;

	MenuInterface->GoToMap(MAP_BOTVILLE);
}

