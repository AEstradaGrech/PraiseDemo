// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PraisePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API APraisePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	APraisePlayerController();
	FORCEINLINE class UAudioComponent* GetControllerAudioComp() { return ControllerAudioComp; }
private:

	virtual void OnPossess(APawn* InPawn) override;
	class UAudioComponent* ControllerAudioComp;
};

