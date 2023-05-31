// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../../Enums/Characters/ECharClass.h"
#include "../../../Enums/Characters/ECharFaction.h"
#include "../../../Structs/Characters/FCharStats.h"
#include "CharStatsHandler.generated.h"

/**
 *
 */
UCLASS(DefaultToInstanced, EditInlineNew)
class PRAISE_API UCharStatsHandler : public UObject
{
	GENERATED_BODY()
public:
	UCharStatsHandler();
	
	FORCEINLINE bool DidInit() const { return bDidInit; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }
	FORCEINLINE float GetMaxMana() const { return MaxMana; }
	FORCEINLINE float GetMaxAdrenaline() const { return MaxAdrenaline; }

	virtual void Init(FCharStats* Stats);

	void DebugStuff();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECharClass Class;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECharFaction Faction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Strenght;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Constitution;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Dextrity; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Intelligence; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Luck;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float MaxMana;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float MaxAdrenaline;

	void SetMaxHealth();
	void SetMaxStamina();
	void SetMaxMana();
	void SetMaxAdrenaline();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bDidInit;

};
