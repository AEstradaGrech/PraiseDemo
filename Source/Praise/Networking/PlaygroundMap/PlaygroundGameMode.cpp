// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaygroundGameMode.h"
#include "../../Components/GameMode/BotSpawnerComponent.h"
#include "../../Components/GameMode/FactionsManagerComponent.h"
#include "../../Components/GameMode/MapLocationsManagerComponent.h"
#include "../../Components/GameMode/MsgCommandsFactoryComponent.h"
#include "../../AI/CommonUtility/Factions/NeutralsFaction.h"
#include "../../AI/CommonUtility/Factions/CreaturesFaction.h"
#include "../../AI/CommonUtility/Factions/PublicEnemiesFaction.h"
#include "../../Characters/AI/BotCharacters/BaseBotCharacter.h"

APlaygroundGameMode::APlaygroundGameMode() : Super()
{
	DefaultMapSpawnPostion = FVector(-2333.0f, -18870.0f, 165.0f);

	BotSpawner = CreateDefaultSubobject<UBotSpawnerComponent>(TEXT("Bot Spawner"));
	BotSpawner->SetDefaultMapSpawnLocation(DefaultMapSpawnPostion);
	
	FactionsManager = CreateDefaultSubobject<UFactionsManagerComponent>(TEXT("Factions Manager"));
	MapLocationsManager = CreateDefaultSubobject<UMapLocationsManagerComponent>(TEXT("Map Locations Manager"));
	MsgCommandsFactory = CreateDefaultSubobject<UMsgCommandsFactoryComponent>(TEXT("Msg Commands Factory"));
	
}

void APlaygroundGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!ensure(GetWorld() != nullptr)) return;

	if (BotSpawner)
		BotSpawner->EnsureSetup();

	if (!MapLocationsManager) return;

	if (MapLocationsManager->GetFactionNodesMap().Num() <= 0)
		MapLocationsManager->CacheCommonMapLocations();
	
	if (MapLocationsManager->GetLevelPathsMap().Num() <= 0)
		MapLocationsManager->SetupLevelPaths();

	FactionsManager->TryAddGameFaction(ECharFaction::BOTVILLE, BotSpawner, MapLocationsManager, FactionsManager, MsgCommandsFactory);
	FactionsManager->TryAddGenericFaction<ANeutralsFaction>(ECharFaction::NONE, BotSpawner, MapLocationsManager, FactionsManager, MsgCommandsFactory);
	FactionsManager->TryAddGenericFaction<ACreaturesFaction>(ECharFaction::CREATURE, BotSpawner, MapLocationsManager, FactionsManager, MsgCommandsFactory);
	FactionsManager->TryAddGenericFaction<APublicEnemiesFaction>(ECharFaction::PUBLIC_ENEMY, BotSpawner, MapLocationsManager, FactionsManager, MsgCommandsFactory);
}

void APlaygroundGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

}


void APlaygroundGameMode::OnPostWorldCreationCallback(UWorld* World)
{
	FLogger::LogTrace(__FUNCTION__);
}

void APlaygroundGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!MapLocationsManager) {
		MapLocationsManager = NewObject<UMapLocationsManagerComponent>(this);
		MapLocationsManager->RegisterComponent();
		MapLocationsManager->InitializeComponent();
	}

	MapLocationsManager->CacheCommonMapLocations();
}

void APlaygroundGameMode::HandleCharDeath(AActor* DeadChar, AActor* Killer)
{
	FLogger::LogTrace(__FUNCTION__ + FString(" :: DEAD CHAR NAME --> ") + DeadChar->GetName());

	bool bHasBeenKilled = Killer != nullptr;

	if(bHasBeenKilled)
		FLogger::LogTrace(__FUNCTION__ + FString(" :: KILLER CHAR NAME --> ") + Killer->GetName());
		
}

