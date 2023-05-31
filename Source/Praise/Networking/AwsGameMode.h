// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../Structs/GameLift/FStartGameSessionState.h"
#include "../Structs/GameLift/FUpdateGameSessionState.h"
#include "../Structs/GameLift/FProcessTerminateState.h"
#include "../Structs/GameLift/FHealthCheckState.h"
#include "AwsGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API AAwsGameMode : public AGameModeBase
{
	GENERATED_BODY()
	

public:
	AAwsGameMode();

protected:

	virtual void BeginPlay() override;

	// Los callback de AWS estan fuera del scope del GameMode. Los struts hacen de puente entre GameMode y SDK.Callbacks
	UPROPERTY()
		FStartGameSessionState StartGameSessionState;
	UPROPERTY()
		FUpdateGameSessionState UpdateGameSessionState;
	UPROPERTY()
		FProcessTerminateState ProcessTerminateState;
	UPROPERTY()
		FHealthCheckState HealthCheckState;
};
