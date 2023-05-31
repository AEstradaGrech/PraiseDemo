// Fill out your copyright notice in the Description page of Project Settings.


#include "../Player/PraisePlayerController.h"
#include "Components/AudioComponent.h"

APraisePlayerController::APraisePlayerController()
{
	ControllerAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Controller Audio Component"));
	
}

void APraisePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}
