// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Gameframework/Actor.h"
#include "FactionZone.h"
#include "../../../Structs/AI/Factions/FGameFactionDTStruct.h"
#include "../../../Structs/AI/Factions/FFactionZoneDTStruct.h"
#include "../../../Enums/Characters/ECharFaction.h"
#include "../../../Interfaces/AI/GameFactionInterface.h"
#include "../Navigation/WaypointHandler.h"
#include "GameFaction.generated.h"

/**
 * 
 */

UCLASS()
class PRAISE_API AGameFaction : public AActor, public IGameFactionInterface
{
	GENERATED_BODY()
	
public:
	AGameFaction();
	FORCEINLINE ECharFaction FactionID() const { return Faction; }
	FORCEINLINE FString GetFactionName() const { return FactionName; }
	FORCEINLINE TArray<AFactionZone*> GetFactionZones() const { return FactionZones; }
	FORCEINLINE TArray<ECharFaction> GetEnemyFactions() const { return EnemyFactions; }
	FORCEINLINE TArray<ECharFaction> GetFriendlyFactions() const { return FriendlyFactions; }
	FORCEINLINE TMap<AActor*, float> GetKnownEnemies() const { return KnownEnemies; }
	FORCEINLINE TMap<AActor*, float> GetKnownAllies() const { return KnownAllies; }
	FORCEINLINE UMsgCommandsFactoryComponent* GetMsgComandsFactory() const { return MsgCommandsFactory; }
	FORCEINLINE UMsgDispatcherComponent* GetMsgDispatcher() const { return FactionMsgDispatcher; }
	virtual UWaypointHandler* GetWaypointHandler() const override;
	virtual class UBotSpawnerComponent* GetBotSpawner() const override { return BotSpawner; }
	virtual class UMapLocationsManagerComponent* GetMapLocationsManager() const override { return LocationsManager; }
	virtual class UFactionsManagerComponent* GetFactionsManager() const override { return FactionsManager; }
	virtual TMap<FString, TSubclassOf<class ABaseBotCharacter>> GetFactionBotBPs() const override { return BotBPs; }

	bool InitFaction(FGameFactionDTStruct* FactionConfig, class UBotSpawnerComponent* Spawner, class UMapLocationsManagerComponent* LocsManager, class UFactionsManagerComponent* FactionsManager, UMsgCommandsFactoryComponent* MsgsFactory);
	bool TryGetConnectedPathNodes(class ANodeWaypoint* Origin, TArray<class ANodeWaypoint*>& PathNodes, TArray<class ANodeWaypoint*>& Nodes, TArray<class ANodeWaypoint*>& CheckedNodes, bool bIncludeCommonNodes = true);
	bool TryGetConnectedPathNodes(class ANodeWaypoint* Origin, TArray<class ANodeWaypoint*>& PathNodes, TArray<class ANodeWaypoint*>& CheckedNodes, bool bIncludeCommonNodes = true);
	bool TryGetClosestNode(FVector Origin, class ANodeWaypoint*& OutNode, bool bCheckAll = false);
	bool TryGetClosestCommonNode(FVector Origin, class ANodeWaypoint*& OutNode);
	UFUNCTION()
	void HandleNewFoE(AActor* FoE, bool bIsEnemy);
	virtual TArray<class ANodeWaypoint*> GetPathNodes() const override;
	virtual TArray<class ANodeWaypoint*> GetLocations() const override;
	virtual TArray<class ANodeWaypoint*> GetZoneNodes(FString ZoneName) const override;
	virtual TArray<class ANodeWaypoint*> GetZoneNodes(ECharFaction NodeFaction, FString ZoneName) const override;
	virtual void AddZonePaths(AFactionZone* Zone) override;
	virtual TArray<class UAIPath*> GetCommonPaths() override;
	virtual TArray<class UAIPath*> GetZonePaths(FString ZoneName = "") override;
	virtual TArray<class UAIPath*> GetRoutePaths(TArray<class ANodeWaypoint*>& RouteNodes) override;
protected:
	template<class T>
	bool InitFactionZones(FGameFactionDTStruct* FactionConfig);
	template<class T>
	bool InitZone(FFactionZoneDTStruct* ZoneConfig);

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		ECharFaction Faction;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		FString FactionName;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		TArray<AFactionZone*> FactionZones;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		TArray<ECharFaction> EnemyFactions;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		TArray<ECharFaction> FriendlyFactions;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		float KnownEnemiesResetSeconds;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		TMap<AActor*, float> KnownEnemies;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		TMap<AActor*, float> KnownAllies;

	virtual void CacheBotBPs(); 
	virtual void CacheFactionLocations();
	void CacheCommonPathNodes();
	virtual void CacheZoneVIPs(AFactionZone* Zone);
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		class UBotSpawnerComponent* BotSpawner;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		class UMapLocationsManagerComponent* LocationsManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		class UFactionsManagerComponent* FactionsManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		class UMsgCommandsFactoryComponent* MsgCommandsFactory;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		class UMsgDispatcherComponent* FactionMsgDispatcher;

	UPROPERTY(EditAnywhere, BlueprintReadonly)
		TMap<FString, TSubclassOf<class ABaseBotCharacter>> BotBPs;

	UPROPERTY()
		UWaypointHandler* WaypointHandler;

	class UDataTable* FactionZonesDT;

	float EnemiesRecallCheckSeconds;
	FTimerHandle KnownEnemiesRecallTimer;

	UFUNCTION()
		void OnKnownEnemiesRecallCheck();
};

template<class T>
inline bool AGameFaction::InitFactionZones(FGameFactionDTStruct* FactionConfig)
{
	try
	{
		const FString ContextString = FString(" :: GETTING ZONES DATA FOR FACTION --> ");
		for (int i = 0; i < FactionConfig->FactionZones.Num(); i++)
		{
			FFactionZoneDTStruct* ZoneConfig = FactionZonesDT->FindRow<FFactionZoneDTStruct>(*FString::FromInt(FactionConfig->FactionZones[i]), ContextString);

			if (!ZoneConfig) continue;

			if (!InitZone<T>(ZoneConfig))
				FLogger::LogTrace(__FUNCTION__ + FString(" :: FAILED TO INITIALIZE FACTION ZONE --> ") + ZoneConfig->ZoneName);
		}

		return true;
	}
	catch (std::exception ex)
	{
		FLogger::LogTrace(__FUNCTION__ + FString(" :: EX MSG --> ") + FString(ex.what()));
		return false;
	}
	return false;
}

template<class T>
inline bool AGameFaction::InitZone(FFactionZoneDTStruct* ZoneConfig)
{
	if (!BotSpawner) return false;

	if (!ensure(BotSpawner->GetWorld() != nullptr)) return false;

	AFactionZone* Zone = BotSpawner->GetWorld()->SpawnActor<T>();

	if (!Zone) return false;

	if (!(Zone->InitZone(ZoneConfig, this))) return false;

	FactionZones.Add(Zone);
	
	CacheZoneVIPs(Zone);
	
	for (UAIPath* ZonePath : Zone->GetWaypointHandler()->GetAllPaths())
		if (ZonePath->IsBuildingLink())
			LocationsManager->AddPath(ZonePath);

	return true;
}
