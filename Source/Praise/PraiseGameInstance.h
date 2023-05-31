// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/UI/CharStatsInterface.h"
#include "Interfaces/UI/MenuInterface.h"
#include "Interfaces/UI/PlayerHUDInterface.h"
#include "Enums/CommonUtility/EGameMenu.h"
#include "PraiseGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UPraiseGameInstance : public UGameInstance, public IMenuInterface, public IPlayerHUDInterface
{
	GENERATED_BODY()
	
public:
	UPraiseGameInstance();
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void StartGameInstance() override;
	virtual void SwitchCrosshair(bool bIsEnabled) override;
	virtual void EnablePlayerStatsBar(bool bIsEnabled, ICharStatsInterface* StatsBarOwner, bool bIsDestroy = false) override;

	//IMenuInterface
	UFUNCTION(BlueprintCallable)
	virtual void LoadMenu(EGameMenu Menu) override;
	UFUNCTION(BlueprintCallable)
	virtual void TeardownMenu(EGameMenu Menu) override;
	virtual void Exit(EGameMenu Menu) override;
	virtual void GoToMap(FName LevelName, bool bIsServerTravel = false) override;

private:
	void SetPlayerHUD();
	TSubclassOf<class UUserWidget> CrosshairClass;
	TSubclassOf<class UUserWidget> StatsBarWidgetClass;
	TSubclassOf<class UBaseMenuWidget> MainMenuBPClass;
	TSubclassOf<class UBaseMenuWidget> GameMenuBPClass;
	TSubclassOf<class UBaseMenuWidget> EndGameMenuBPClass;

	class UMainMenuWidget* MainMenuWidget;
	class UBaseMenuWidget* GameMenuWidget;
	class UBaseMenuWidget* EndGameMenuWidget;
	class UCharStatsBarWidget* CharStatsWidget;
	class UUserWidget* Crosshair;

	class USoundCue* SoundtracksQ;
};
