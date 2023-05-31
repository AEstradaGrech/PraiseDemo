#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "FPlayerStats.h"
#include "FBotStats.generated.h"

USTRUCT(BlueprintType)
struct PRAISE_API FBotStats : public FPlayerStats
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MaxPerceivedTargets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MaxPerceivedAllies;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MaxPerceivedEnemies;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TargetRecallSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AllyRecallSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float EnemyRecallSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LastKnownEnemyRecallSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AttackAwaitSecs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float EvadeAwaitSecs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AlertedStateSecs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bStamLimitedSprint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DefaultMovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DefaultFOV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DefaultSightRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WalkSpeedMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxWalkSpeedMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ArmedSpeedMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AccelerationLerpSpeed;
};