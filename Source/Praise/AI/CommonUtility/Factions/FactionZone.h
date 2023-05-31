// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Gameframework/Actor.h"
#include "../../../Enums/Characters/ECharFaction.h"
#include "../../../Structs/CommonUtility/FUtilities.h"
#include "../../../Structs/AI/Factions/FFactionZoneDTStruct.h"
#include "Components/PrimitiveComponent.h"
#include "NavigationSystem.h"
#include "../Navigation/AIWaypoint.h"
#include "../Navigation/NodeWaypoint.h"
#include "../../../Components/Actor/MsgDispatcherComponent.h"
#include "../../../Interfaces/AI/GameFactionInterface.h"
#include "../../../Praise.h"
#include "FactionZone.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnZoneBotDeadSignature, AActor*, Self, AActor*, Killer);
UCLASS()
class PRAISE_API AFactionZone : public AActor
{
	GENERATED_BODY()
	
public:
	AFactionZone();

	virtual bool InitZone(FFactionZoneDTStruct* ZoneConfig, IGameFactionInterface* FactionInterface);
	TArray<AAIWaypoint*> GetSubZoneWaypoints(FString Label = "") const;
	TArray<ANodeWaypoint*> GetZoneNodes(bool bOnlyVipNodes = false, FName Tag = FName(""), bool bExcludeTagged = false);
	FORCEINLINE FString GetZoneName()const { return ZoneName; }
	FORCEINLINE class UWaypointHandler* GetWaypointHandler() const { return WaypointHandler; }
	FORCEINLINE bool ShouldSpawnBuildingBots() const { return bBuildingBotsEnabled; }
	FORCEINLINE float GetBotRespawnSeconds() const { return BotRespawnSeconds; }
	FORCEINLINE int GetNeutralHitsTolerance() const { return NeutralHitsTolerance; }
	FORCEINLINE UMsgDispatcherComponent* GetMsgDispatcher() const { return ZoneMsgDispatcher; }
	FORCEINLINE bool HasZonePaths() const { return bHasZonePaths; }
	ANodeWaypoint* GetBuildingEntryNode(FString BuildingEditorName, FString EntryTag);
protected:

	virtual void BeginPlay() override;

	UPROPERTY()
	UNavigationSystemV1* NavMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECharFaction ZoneFaction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString FactionName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ZoneName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, int> ZoneBots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int NeutralHitsTolerance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BotRespawnSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* ZoneCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHasZoneCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHasZonePaths;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bBuildingBotsEnabled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float InitBotsDelaySeconds;
	
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = WaypointsManagement)
		class UWaypointHandler* WaypointHandler;

	FTimerHandle BotSpawnTimerHandle;
	
	UFUNCTION()
		virtual void InitBots();
	UFUNCTION()
		virtual void HandleCharDeath(AActor* DeadChar, AActor* Killer);

	virtual void SetZoneWaypoints();
	void SetZoneBuildingWaypoints();
	void SetupBuildingPathLinks(class ABaseBuilding* ZoneBuilding);
	void SpawnBuildingBots();
	void SetupSpawnedBot(class ABaseBotCharacter* NewBot);
	void ProcessSpawnedBuildingBots(TArray<class ABaseBotCharacter*> NewBots);
	void SpawnRemainingBotType(FString BotBP, int Max);
	bool SpawnBot(FString BotBP, class ABaseBotCharacter*& OutBot, FVector SpawnLocation = FVector::ZeroVector);
	bool SpawnBot(FString BotBP, class AAIWaypoint* SpawnPoint, class ABaseBotCharacter*& OutBot);
	void RemoveBot(class ABaseBotCharacter* Bot);
	virtual void AddZoneBotKnownLocations(class ABaseBotCharacter* SpawnedBot, FString Zone);
	FOnZoneBotDeadSignature OnZoneBotDead;
	TMap<FString, TArray<class ABaseBotCharacter*>> SpawnedBots;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = CharactersManagement, Category = CharactersManagement)
		TArray<AActor*> ZoneForeigners;
	
	bool InitZoneCollider(FVector Location, FVector BoxExtent);
	UFUNCTION()
		void OnZoneColliderBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnZoneColliderEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FVector GetZoneSpawnLocation();
	virtual class AAIWaypoint* GetZoneSpawnPoint();

	IGameFactionInterface* GameFaction;
	UMsgDispatcherComponent* ZoneMsgDispatcher;
	TArray<class ABaseBuilding*> ZoneBuildings;
};
