#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "../../../Enums/Characters/ECharFaction.h"
#include "FGameFactionDTStruct.generated.h"

USTRUCT()
struct FGameFactionDTStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECharFaction Faction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString FactionName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int> FactionZones;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int> EnemyFactions;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int> FriendlyFactions;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float KnownEnemiesResetSeconds;
};
