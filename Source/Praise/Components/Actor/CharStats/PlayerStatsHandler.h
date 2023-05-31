// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharStatsHandler.h"
#include "../../../Structs/Characters/FPlayerStats.h"
#include "PlayerStatsHandler.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UPlayerStatsHandler : public UCharStatsHandler
{
	GENERATED_BODY()
	
public:
	UPlayerStatsHandler();
	void Init(FPlayerStats Stats);
	virtual void Init(FCharStats* Stats) override;
	class ULevelUp* SetLevelUp(class ULevelUp* LevelUp);
	void ApplyLvlUp(class ULevelUp* LevelUp);

	UPROPERTY(EditAnywhere)
		int32 Wisdom;
	UPROPERTY(EditAnywhere)
		int32 Charisma;
	UPROPERTY(EditAnywhere)
		float XP;
	UPROPERTY(EditAnywhere)
		float XPToLvlUp;
	UPROPERTY(EditAnywhere)
		float Restness;
private:
	float GetXPToNextLvl(int currentLvl);
	void BuildProgressionChart();

	UPROPERTY(VisibleAnywhere)
		int32 TotalAttributePoints;
	UPROPERTY(VisibleAnywhere)
		int32 TotalSkillPoints;
};
