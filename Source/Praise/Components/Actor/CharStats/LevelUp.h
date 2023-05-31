// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LevelUp.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API ULevelUp : public UObject
{
	GENERATED_BODY()
	
public:
	FORCEINLINE int32 FromLevel() const { return CurrentLvl; }
	FORCEINLINE int32 ToLevel() const { return NewLvl; }

	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }
	FORCEINLINE float GetMaxMana() const { return MaxMana; }
	FORCEINLINE float GetMaxAdrenaline() const { return MaxAdrenaline; }
	FORCEINLINE float GetMaxRestness() const { return MaxRestness; }

	FORCEINLINE void SetCurrentLvl(int32 lvl) { CurrentLvl = lvl; NewLvl = CurrentLvl + 1; }

	UPROPERTY(EditAnywhere)
		int32 CurrentLvlMaxXP;
	UPROPERTY(EditAnywhere)
		float RemainingXP;
	UPROPERTY(EditAnywhere)
		int32 AttributePoints;
	UPROPERTY(EditAnywhere)
		int32 SkillPoints;

private:
	UPROPERTY(VisibleAnywhere)
		int32 CurrentLvl;
	UPROPERTY(VisibleAnywhere)
		int32 NewLvl;
	UPROPERTY(VisibleAnywhere)
		float MaxHealth;
	UPROPERTY(VisibleAnywhere)
		float MaxStamina;
	UPROPERTY(VisibleAnywhere)
		float MaxMana;
	UPROPERTY(VisibleAnywhere)
		float MaxAdrenaline;
	UPROPERTY(VisibleAnywhere)
		float MaxRestness;
};
