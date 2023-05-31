// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharStatsComponent.h"
#include "../../../Structs/Characters/FPlayerStats.h"
#include "PlayerStatsHandler.h"
#include "LevelUp.h"
#include "PlayerStatsComponent.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UPlayerStatsComponent : public UCharStatsComponent
{
	GENERATED_BODY()
public:
		UPlayerStatsComponent();
public:

	FORCEINLINE float GetMana() const { return Mana; }
	FORCEINLINE float GetMaxMana() const { return CharacterStats ? GetStatsObject<UPlayerStatsHandler>()->GetMaxMana() : DEFAULT_CHAR_MANA; }
	FORCEINLINE float GetMaxAdrenaline() const { return CharacterStats ? GetStatsObject<UPlayerStatsHandler>()->GetMaxAdrenaline() : DEFAULT_CHAR_ADRENALINE; }
	FORCEINLINE float GetAdrenaline()const { return Adrenaline; }
	FORCEINLINE TArray<ULevelUp*> GetLevelUps() const { return LevelUps; }

	virtual float GetWalkSpeed() const override;
	virtual float GetArmedSpeedMult() const override;

	UFUNCTION()
		void ManageXP(float Value);

	virtual float GetMainStatValue(ECharStats Stat) override;
	void InitializePlayerStatsObject(FPlayerStats PlayerStats);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void DebugStuff();
	virtual void UpdateMainStatValue(ECharStats Stat, float Value) override;

protected:
	virtual void SetDefaults() override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
		float Mana;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
		float Adrenaline;

	void LevelUp(float TotalXP, float CurrentLvlMaxXP);

	TArray<ULevelUp*> LevelUps;
};
