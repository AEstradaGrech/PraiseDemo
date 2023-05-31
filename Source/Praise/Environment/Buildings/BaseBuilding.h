// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../Enums/Characters/ECharFaction.h"
#include "../../AI/CommonUtility/Navigation/AIWaypoint.h"
#include "../../AI/CommonUtility/Navigation/NodeWaypoint.h"
#include "NavModifierVolume.h"
#include "../../Praise.h"
#include "BaseBuilding.generated.h"


UCLASS()
class PRAISE_API ABaseBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseBuilding();

	FORCEINLINE bool HasOwner() const { return bHasOwner; }
	FORCEINLINE bool AllowNonOwnersIn() const { return bAllowNonOwnersIn; }
	FORCEINLINE bool IsOpen() const { return bIsOpen; }
	FORCEINLINE bool IsPrivate() const { return bIsPrivate; }
	FORCEINLINE bool IsOnFire() const { return bIsOnFire; }
	FORCEINLINE bool OnlyBuildingPatrols() const { return bOnlyBuildingPatrols; }
	FORCEINLINE bool ShouldSpawnBots() const { return bShouldSpawnBots; }
	FORCEINLINE float GetBotRespawnSeconds() const { return BotRespawnSeconds; }
	FORCEINLINE float GetBuildingState() const { return BuildingState; }
	FORCEINLINE ECharFaction FactionID() const { return Faction; }
	FORCEINLINE FString GetBuildingName() const { return BuildingName; }
	FORCEINLINE FString GetBuildingZoneName() const { return ZoneName; }
	FORCEINLINE TArray<AAIWaypoint*> GetBuildingWaypoints() const { return BuildingWaypoints; }
	FORCEINLINE TArray<ABasePraiseCharacter*> GetBuildingChars() const { return AssignedChars; }
	FORCEINLINE TMap<FString, FName> GetZoneLinkNodes() const { return ZoneLinkNodes; }

	void AddBuildingWaypoint(AAIWaypoint* NewWP);
	bool TrySpawnBuildingBots(class UBotSpawnerComponent* BotSpawner, class AGameFaction* BuildingFaction, class AFactionZone* BuildingZone, TArray<ABaseBotCharacter*>& OutBots);
	void OnBuildingCharDead(ABasePraiseCharacter* Char);
	ANodeWaypoint* GetMainDoorNode();
	TArray<ANodeWaypoint*> GetBuildingEntries() const { return BuildingEntries; }
	void SetIsPrivate(bool bPrivate);
	bool TryAssignChar(class ABasePraiseCharacter* Char);
protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString BuildingName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* BuildingRootComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* Walls;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* InteriorWalls;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* Floor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* Roof;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* Props;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* Doors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* Lights;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* Waypoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECharFaction Faction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ZoneName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsPrivate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHasOwner;							
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bAllowNonOwnersIn;					
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsOpen;								
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsOnFire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BuildingState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bShouldSpawnBots;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BotRespawnSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bSpawnBotsIndoor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bOnlyBuildingPatrols;
	UPROPERTY(VisibleAnywhere)
		TArray<ANodeWaypoint*> BuildingEntries;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, FName> ZoneLinkNodes;
	
	UPROPERTY(EditAnywhere)
		TMap<FString, int> DefaultBots;
	
	UPROPERTY(VisibleAnywhere)
		TArray<ABasePraiseCharacter*> AssignedChars;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AAIWaypoint*> BuildingWaypoints;
	UPROPERTY(EditAnywhere, BlueprintReadonly)
		bool bDisableLights;
	// DEVONLY
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bDebugWaypoints;

};
