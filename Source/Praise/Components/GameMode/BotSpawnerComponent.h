// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MachineStatesFactoryComponent.h"
#include "../../AI/CommonUtility/Navigation/AIWaypoint.h"
#include "../../Enums/AI/EBrainType.h"
#include "../../Enums/AI/EBotClass.h"
#include "../../Structs/Characters/FBotStats.h"
#include "BotSpawnerComponent.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRAISE_API UBotSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBotSpawnerComponent();
	typedef ABaseBotCharacter* (UBotSpawnerComponent::* pCreateBotFn)();
	FORCEINLINE UMachineStatesFactoryComponent* GetMachineStatesFactory() const { return MachineStatesFactory; }
	FORCEINLINE TArray<AAIWaypoint*> GetCommonWaypoints() const { return CommonWaypoints; } 
	FORCEINLINE TMap<FString, TSubclassOf<ABaseBotCharacter>> GetBotBPs() const { return BotBPs; }
	FORCEINLINE FVector GetDefaultMapSpawnLocation() const { return DefaultMapSpawnPostion; }
	FORCEINLINE void SetDefaultMapSpawnLocation(FVector Location) { DefaultMapSpawnPostion = Location; }

	class ABaseBotCharacter* GetDummyBot();
	bool EnsureSetup();

	TSubclassOf<ABaseBotCharacter> GetBotBP(FString BPName);
	ABaseBotCharacter* GetBot(EBotClass BotClass);
	ABaseBotCharacter* GetBot(EBotClass BotClass, EBrainType BotType);
	ABaseBotCharacter* GetBot(FString BotBP,  FVector SpawnPosition = FVector::ZeroVector);
	ABaseBotCharacter* GetBot(TSubclassOf<class ABasePraiseCharacter> BotBP, FVector SpawnPosition = FVector::ZeroVector);
	ABaseBotCharacter* GetBot(TSubclassOf<class ABasePraiseCharacter> BotBP, AGameFaction* BotFaction, AFactionZone* SpawnZone, FVector SpawnPosition = FVector::ZeroVector);
	ABaseBotCharacter* GetBot(FString BotBPName, AGameFaction* BotFaction, AFactionZone* SpawnZone, FVector SpawnPosition = FVector::ZeroVector);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UMachineStatesFactoryComponent* MachineStatesFactory;

	FVector GetDefaultSpawnLocation(bool bRandomWaypoint = false);
	
	class UDataTable* BotStatsConfigDT;

	UPROPERTY(EditAnywhere, BlueprintReadonly)
		TMap<FString, TSubclassOf<ABaseBotCharacter>> BotBPs;
	UPROPERTY(EditAnywhere, BlueprintReadonly)
		FVector DefaultMapSpawnPostion;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		TArray<AAIWaypoint*> CommonWaypoints;

	TMap<EBotClass, pCreateBotFn> CreateBotFunctionsMap;


	template<class T>
	ABaseBotCharacter* CreateBot() { return NewObject<T>(); }
	template<class T>
	void RegisterBot(EBotClass BotClass) { CreateBotFunctionsMap.Add(BotClass, &UBotSpawnerComponent::CreateBot<T>); }

private:
	FBotStats* GetStatsConfig(FString ConfigName);
	void RegisterBots();
	void AddBotConfig(ABaseBotCharacter* Bot);
};
