// Fill out your copyright notice in the Description page of Project Settings.


#include "PraiseGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "UI/CharStatsBarWidget.h"
#include "kismet/GameplayStatics.h"
#include "UI/MainMenuWidget.h"
#include "UI/InGameMenuWidget.h"
#include "UI/EndGameMenuWidget.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Characters/Player/PraisePlayerController.h"
#include "Characters/Player/PraisePlayerCharacter.h"

UPraiseGameInstance::UPraiseGameInstance() : Super()
{
	ConstructorHelpers::FClassFinder<UUserWidget>CrosshairBP(TEXT("/Game/Core/UI/PlayerHUD/WBP_SimpleCrosshair"));

	if (CrosshairBP.Succeeded())
		CrosshairClass = CrosshairBP.Class;

	ConstructorHelpers::FClassFinder<UUserWidget>StatsBarBP(TEXT("/Game/Core/UI/PlayerHUD/WBP_PlayerStatsBar"));

	if (StatsBarBP.Succeeded())
		StatsBarWidgetClass = StatsBarBP.Class;

	ConstructorHelpers::FClassFinder<UUserWidget>MainMenuBP(TEXT("/Game/Core/UI/Widgets/MENU_0/WBP_MainMenu"));

	if (MainMenuBP.Succeeded())
		MainMenuBPClass = MainMenuBP.Class;

	ConstructorHelpers::FClassFinder<UUserWidget>GameMenuBP(TEXT("/Game/Core/UI/Widgets/WBP_InGameMenu"));

	if (GameMenuBP.Succeeded())
		GameMenuBPClass = GameMenuBP.Class;
	
	ConstructorHelpers::FClassFinder<UUserWidget>EndGameMenuBP(TEXT("/Game/Core/UI/Widgets/WBP_EndGameMenu"));

	if (EndGameMenuBP.Succeeded())
		EndGameMenuBPClass = EndGameMenuBP.Class;

	static ConstructorHelpers::FObjectFinder<USoundCue> SoundtracksCue(TEXT("/Game/Core/Audio/Music/SoundtracksCue"));

	if (SoundtracksCue.Succeeded())
		SoundtracksQ = SoundtracksCue.Object;

}

void UPraiseGameInstance::Init()
{
	Super::Init();

}

void UPraiseGameInstance::Shutdown()
{
	Super::Shutdown();
}

void UPraiseGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
}

void UPraiseGameInstance::SwitchCrosshair(bool bIsEnabled)
{
	//if (!Crosshair)
	//{
	if (bIsEnabled)
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(this, CrosshairClass);

		if (!Widget) return;

		Crosshair = Widget;

		Crosshair->bIsFocusable = false;

		Crosshair->AddToViewport();
	}
	else
	{
		CharStatsWidget->ClearWidgetOwner();
		Crosshair->RemoveFromViewport();
	}

	if (!Crosshair) return;

	switch (bIsEnabled)
	{
		case(true):
			Crosshair->SetVisibility(ESlateVisibility::Visible);
			break;
		case(false):
			Crosshair->SetVisibility(ESlateVisibility::Hidden);
			break;
	}
}

void UPraiseGameInstance::EnablePlayerStatsBar(bool bIsEnabled, ICharStatsInterface* StatsBarOwner, bool bIsDestroy)
{
	if (bIsEnabled)
	{

		UUserWidget* Widget = CreateWidget<UUserWidget>(this, StatsBarWidgetClass);

		if (!Widget) return;

		CharStatsWidget = Cast<UCharStatsBarWidget>(Widget);

		CharStatsWidget->bIsFocusable = false;

		CharStatsWidget->AddToViewport();

		CharStatsWidget->SetWidgetOwner(StatsBarOwner);
	}
	else
	{
		CharStatsWidget->ClearWidgetOwner();
		
		CharStatsWidget->RemoveFromViewport();
	}
	
}

void UPraiseGameInstance::SetPlayerHUD()
{
	
}

void UPraiseGameInstance::LoadMenu(EGameMenu Menu)
{
	UBaseMenuWidget* MenuWidget = nullptr;

	switch (Menu)
	{
	case(EGameMenu::MAIN):

		if (!MainMenuBPClass) return;

		MainMenuWidget = CreateWidget<UMainMenuWidget>(this, MainMenuBPClass);
	
		if (!MainMenuWidget) return;

		MainMenuWidget->RenderMenu();

		MainMenuWidget->SetMenuInterface(this);
		

		break;

	case(EGameMenu::INGAME):

		if (!GameMenuBPClass) return;

		GameMenuWidget = CreateWidget<UInGameMenuWidget>(this, GameMenuBPClass);

		if (!GameMenuWidget) return;

		if (GetFirstLocalPlayerController())
		{
			APlayerController* PlayerController = GetFirstLocalPlayerController();

			if (PlayerController->GetPawn()->IsA<APraisePlayerCharacter>())
			{
				APraisePlayerCharacter* Player = Cast<APraisePlayerCharacter>(PlayerController->GetPawn());

				SwitchCrosshair(false);

				EnablePlayerStatsBar(false, Player);

				Player->SetGameMenuEnabled(true);

				UAudioComponent* CharAudioComp = Player->GetCharAudioComp();

				if (CharAudioComp && SoundtracksQ)
				{
					CharAudioComp->SetIntParameter(FName("SoundtrackSample"), 2);
					CharAudioComp->SetSound(SoundtracksQ);
					CharAudioComp->Play();
				}
			}
		}

		GameMenuWidget->RenderMenu();

		GameMenuWidget->SetMenuInterface(this);

		break;
	case(EGameMenu::ENDGAME):

		if (!EndGameMenuBPClass) return;

		EndGameMenuWidget = CreateWidget<UEndGameMenuWidget>(this, EndGameMenuBPClass);

		if (!EndGameMenuWidget) return;

		if (GetFirstLocalPlayerController())
		{
			APlayerController* PlayerController = GetFirstLocalPlayerController();

			if (PlayerController->IsA<APraisePlayerController>())
			{
				UAudioComponent* CharAudioComp = Cast<APraisePlayerController>(PlayerController)->GetControllerAudioComp();

				if (CharAudioComp && SoundtracksQ)
				{
					CharAudioComp->SetIntParameter(FName("SoundtrackSample"), 2);
					CharAudioComp->SetSound(SoundtracksQ);
					CharAudioComp->Play();
				}
			}
		}

		EndGameMenuWidget->RenderMenu();

		EndGameMenuWidget->SetMenuInterface(this);

		break;
	}

	
	
}

void UPraiseGameInstance::TeardownMenu(EGameMenu Menu)
{
	UBaseMenuWidget* MenuWidget = nullptr;

	switch (Menu)
	{
		case(EGameMenu::MAIN):

			if (!MainMenuWidget) return;

			MenuWidget = MainMenuWidget;

			break;
		case(EGameMenu::INGAME):

			if (!GameMenuWidget) return;

			MenuWidget = GameMenuWidget;

			if (GetFirstLocalPlayerController())
			{
				APlayerController* PlayerController = GetFirstLocalPlayerController();

				if (PlayerController->GetPawn() && PlayerController->GetPawn()->IsA<APraisePlayerCharacter>())
				{
					APraisePlayerCharacter* Player = Cast<APraisePlayerCharacter>(PlayerController->GetPawn());

					SwitchCrosshair(true);

					EnablePlayerStatsBar(true, Player);

					Player->SetGameMenuEnabled(false);

					UAudioComponent* CharAudioComp = Player->GetCharAudioComp();

					if (CharAudioComp && SoundtracksQ)
					{
						CharAudioComp->SetSound(SoundtracksQ);
						CharAudioComp->Stop();
					}
				}
			}
			break;

		case(EGameMenu::ENDGAME):

			if (!EndGameMenuWidget) return;

			MenuWidget = EndGameMenuWidget;

			break;
	}

	if(MenuWidget)
		MenuWidget->TeardownMenu();

}

void UPraiseGameInstance::Exit(EGameMenu Menu)
{
	if (!GetFirstLocalPlayerController()) return;

	APlayerController* Controller = GetFirstLocalPlayerController();

	switch (Menu)
	{
		case(EGameMenu::MAIN):
			
			Controller->ConsoleCommand("Quit");

			break;
		case(EGameMenu::INGAME):

			TeardownMenu(EGameMenu::INGAME);

			if (Controller->GetPawn() && Controller->GetPawn()->IsA<APraisePlayerCharacter>()) 
			{
				SwitchCrosshair(false);
				EnablePlayerStatsBar(false, Cast<APraisePlayerCharacter>(Controller->GetPawn()), true);
			}
			
			GoToMap(MAP_MAIN_MENU);
			break;
	}
}

void UPraiseGameInstance::GoToMap(FName LevelName, bool bIsServerTravel)
{
	if (!ensure(GetWorld() != nullptr)) return;

	if (LevelName.IsNone()) return;


	if (bIsServerTravel)
	{
		//SeamlessTravel
	}

	else UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}
