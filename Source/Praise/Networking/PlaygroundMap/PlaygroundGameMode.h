// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PraiseGameMode.h"
#include "../../Enums/AI/EBrainType.h"
#include "../../Enums/AI/EBotClass.h"
#include "NavigationSystem.h"
#include "PlaygroundGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API APlaygroundGameMode : public APraiseGameMode
{
	GENERATED_BODY()

public:
	APlaygroundGameMode();


	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
private:
	
	UFUNCTION()
		void OnPostWorldCreationCallback(UWorld* World);

protected:
	virtual void PostInitializeComponents() override;
	virtual void HandleCharDeath(AActor* DeadChar, AActor* Killer) override;

	UPROPERTY(EditAnywhere, BlueprintReadonly)
		FVector DefaultMapSpawnPostion;
};
