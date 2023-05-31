// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMenuWidget.h"

void UBaseMenuWidget::RenderMenu()
{
	if (!ensure(GetWorld() != nullptr)) return; 

	AddToViewport();

	bIsFocusable = true;

	UWorld* World = GetWorld();

	FInputModeGameAndUI InputMode;

	InputMode.SetWidgetToFocus(TakeWidget());

	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	APlayerController* Controller = World->GetFirstPlayerController();

	Controller->SetInputMode(InputMode);

	Controller->bShowMouseCursor = true;
}

void UBaseMenuWidget::TeardownMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("TEARING DOWN MENU"));

	RemoveFromViewport();

	UWorld* World = GetWorld();

	if (!ensure(World != nullptr)) return;
	
	APlayerController* Controller = World->GetFirstPlayerController();

	if (!ensure(Controller != nullptr)) return;
	
	FInputModeGameOnly InputMode;

	Controller->SetInputMode(InputMode);

	Controller->bShowMouseCursor = false;
}