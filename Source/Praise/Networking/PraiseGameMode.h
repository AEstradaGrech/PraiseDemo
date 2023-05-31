// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PraiseGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API APraiseGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	APraiseGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	FORCEINLINE class UBotSpawnerComponent* GetBotSpawner() const { return BotSpawner; }
	FORCEINLINE class UFactionsManagerComponent* GetFactionsManager() const { return FactionsManager; }
	FORCEINLINE class UMapLocationsManagerComponent* GetMapLocationsManager() const { return MapLocationsManager; }
	FORCEINLINE class UMsgCommandsFactoryComponent* GetMsgCommandsFactory() const { return MsgCommandsFactory; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		class UBotSpawnerComponent* BotSpawner;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		class UFactionsManagerComponent* FactionsManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		class UMapLocationsManagerComponent* MapLocationsManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		class UMsgCommandsFactoryComponent* MsgCommandsFactory;
	UFUNCTION()
	virtual void HandleCharDeath(AActor* DeadChar, AActor* Killer) {};
};
