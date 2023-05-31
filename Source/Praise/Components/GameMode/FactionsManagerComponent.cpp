// Fill out your copyright notice in the Description page of Project Settings.


#include "FactionsManagerComponent.h"
#include "../../Networking/PraiseGameMode.h"
#include "BotSpawnerComponent.h"
#include "MapLocationsManagerComponent.h"
#include "MsgCommandsFactoryComponent.h"
#include "Engine/DataTable.h"

// Sets default values for this component's properties
UFactionsManagerComponent::UFactionsManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	static const ConstructorHelpers::FObjectFinder<UDataTable> FactionsDTFile(TEXT("DataTable'/Game/Core/DataTables/Factions/DT_Factions.DT_Factions'"));

	if (FactionsDTFile.Succeeded())
		FactionsDT = FactionsDTFile.Object;

	// ...
}


// Called when the game starts
void UFactionsManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

TArray<ANodeWaypoint*> UFactionsManagerComponent::GetFactionNodes(ECharFaction Faction, bool bOnlyVipNodes)
{
	if (GameFactions.Contains(Faction))
		return bOnlyVipNodes ? GameFactions[Faction]->GetLocations() : GameFactions[Faction]->GetPathNodes();
	
	else return  TArray<ANodeWaypoint*>();
}

bool UFactionsManagerComponent::TryAddDefaulGameFactions()
{
	if (!ensure(GetWorld() != nullptr)) return false;

	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();

	if (!GameMode) return false;

	if (!GameMode->IsA<APraiseGameMode>()) return false;

	APraiseGameMode* PraiseGameMode = Cast<APraiseGameMode>(GameMode);

	if (!(PraiseGameMode->GetBotSpawner() && PraiseGameMode->GetMapLocationsManager())) return false;

	return TryAddGameFaction(ECharFaction::NONE, PraiseGameMode->GetBotSpawner(), PraiseGameMode->GetMapLocationsManager(), PraiseGameMode->GetFactionsManager(), PraiseGameMode->GetMsgCommandsFactory()) && 
		   TryAddGameFaction(ECharFaction::PROTECTOR, PraiseGameMode->GetBotSpawner(), PraiseGameMode->GetMapLocationsManager(), PraiseGameMode->GetFactionsManager(), PraiseGameMode->GetMsgCommandsFactory()) &&
		   TryAddGameFaction(ECharFaction::PUBLIC_ENEMY, PraiseGameMode->GetBotSpawner(), PraiseGameMode->GetMapLocationsManager(), PraiseGameMode->GetFactionsManager(), PraiseGameMode->GetMsgCommandsFactory()) &&
		   TryAddGameFaction(ECharFaction::CREATURE, PraiseGameMode->GetBotSpawner(), PraiseGameMode->GetMapLocationsManager(), PraiseGameMode->GetFactionsManager(), PraiseGameMode->GetMsgCommandsFactory());
}

bool UFactionsManagerComponent::TryAddGameFaction(ECharFaction Faction, UBotSpawnerComponent* BotSpawner, UMapLocationsManagerComponent* LocationsManager, UFactionsManagerComponent* FactionsMan, UMsgCommandsFactoryComponent* MsgsFactory)
{
	if (GameFactions.Contains(Faction)) return false;

	if (!FactionsDT) return false;

	FGameFactionDTStruct* FactionConfig = GetFactionData(Faction);

	if (!FactionConfig) return false;

	AGameFaction* NewFaction = NewObject<AGameFaction>(this);

	if (!NewFaction) return false;

	if (!NewFaction->InitFaction(FactionConfig, BotSpawner, LocationsManager, FactionsMan, MsgsFactory)) return false;

	GameFactions.Add(Faction, NewFaction);

	return true;
}

FGameFactionDTStruct* UFactionsManagerComponent::GetFactionData(ECharFaction Faction)
{
	if (!FactionsDT) return nullptr;

	const FString ContextString = FString(" :: GETTING DATA FOR FACTION --> ") + FUtilities::EnumToString(*FString("ECharFaction"), (int)Faction);

	TArray<FGameFactionDTStruct*> Rows;

	FactionsDT->GetAllRows(ContextString, Rows);
	
	for (FGameFactionDTStruct* Row : Rows)
		if (Row->Faction == Faction)
			return Row;

	return nullptr;
}

