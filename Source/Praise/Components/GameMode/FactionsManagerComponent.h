// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Enums/Characters/ECharFaction.h"
#include "../../AI/CommonUtility/Factions/GameFaction.h"
#include "../../Structs/AI/Factions/FGameFactionDTStruct.h"
#include "FactionsManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRAISE_API UFactionsManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFactionsManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	bool TryAddDefaulGameFactions();
	bool TryAddGameFaction(ECharFaction Faction, class UBotSpawnerComponent* BotSpawner, class UMapLocationsManagerComponent* LocationsManager, class UFactionsManagerComponent* FactionsMan, class UMsgCommandsFactoryComponent* MsgsFactory);
	template<class T>
	bool TryAddGenericFaction(ECharFaction Faction, class UBotSpawnerComponent* BotSpawner, class UMapLocationsManagerComponent* LocationsManager, class UFactionsManagerComponent* FactionsMan, class UMsgCommandsFactoryComponent* MsgsFactory);
	TArray<class ANodeWaypoint*> GetFactionNodes(ECharFaction Faction, bool bOnlyVipNodes = false);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess="true"))
		TMap<ECharFaction, AGameFaction*> GameFactions;
	
	FGameFactionDTStruct* GetFactionData(ECharFaction Faction);

	class UDataTable* FactionsDT;
		
};

template<class T>
inline bool UFactionsManagerComponent::TryAddGenericFaction(ECharFaction Faction, UBotSpawnerComponent* BotSpawner, UMapLocationsManagerComponent* LocationsManager, UFactionsManagerComponent* FactionsMan, UMsgCommandsFactoryComponent* MsgsFactory)
{
	if (GameFactions.Contains(Faction)) return false;

	if (!FactionsDT) return false;

	FGameFactionDTStruct* FactionConfig = GetFactionData(Faction);

	if (!FactionConfig) return false;

	AGameFaction* NewFaction = BotSpawner->GetWorld()->SpawnActor<T>();

	if (!NewFaction) return false;

	if (!NewFaction->InitFaction(FactionConfig, BotSpawner, LocationsManager, FactionsMan, MsgsFactory)) return false;

	GameFactions.Add(Faction, NewFaction);

	return true;
}
