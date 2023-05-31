// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerStatsComponent.h"
#include "../../../Structs/Characters/FCharStats.h"
#include "../../../Structs/Characters/FBotStats.h"
#include "BotStatsComponent.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UBotStatsComponent : public UPlayerStatsComponent
{
	GENERATED_BODY()
	
public:
	UBotStatsComponent();

	FORCEINLINE int32 GetMaxPerceivedTargets() const { return MaxPerceivedTargets; }
	FORCEINLINE float GetTargetRecallSeconds() const { return TargetRecallSeconds; }
	FORCEINLINE int32 GetMaxPerceivedAllies() const { return MaxPerceivedAllies; }
	FORCEINLINE float GetAllyRecallSeconds() const { return AllyRecallSeconds; }
	FORCEINLINE int32 GetMaxPerceivedEnemies() const { return MaxPerceivedEnemies; }
	FORCEINLINE float GetEnemyRecallSeconds() const { return EnemyRecallSeconds; }
	FORCEINLINE float GetLastKnownEnemyRecallSeconds() const { return LastKnownEnemyRecallSeconds; }
	FORCEINLINE float GetAttackAwaitSeconds() const { return AttackAwaitSecs; }
	FORCEINLINE float GetEvadeAwaitSeconds() const { return EvadeAwaitSecs; }
	FORCEINLINE float GetAlertedStateSeconds() const { return AlertedStateSecs; }
	FORCEINLINE	bool IsSprintStamEnabled() const { return bSprintStamEnabled; }
	FORCEINLINE float GetAccelerationLerpSpeed() const { return AccelerationLerpSpeed; }
	
	virtual void InitializeStatsObject(FCharStats* StatsStruct) override;

private:
	// ----	Targets Mgmt
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
		int32 MaxPerceivedTargets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
		float TargetRecallSeconds;

	// ---- Allies Mgmt
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
		int32 MaxPerceivedAllies;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
		float AllyRecallSeconds;

	// ---- Enemies Mgmt
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
		int32 MaxPerceivedEnemies;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
		float EnemyRecallSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
		float LastKnownEnemyRecallSeconds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
		float AttackAwaitSecs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
		float EvadeAwaitSecs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
		float AlertedStateSecs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
		bool bSprintStamEnabled;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
		float AccelerationLerpSpeed;
protected:
	virtual void SetDefaults() override;
	virtual void BeginPlay() override;

};
