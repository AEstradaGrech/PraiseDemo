#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "../../../Enums/Characters/ECharFaction.h"
#include "FFactionZoneDTStruct.generated.h"

USTRUCT()
struct FFactionZoneDTStruct : public FTableRowBase
{
	GENERATED_BODY()
public:
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
		float InitBotsDelaySeconds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BotRespawnSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bBuildingBotsEnabled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHasZoneCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHasZonePaths;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector ZoneBoxLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector ZoneBoxExtents;
	
};
