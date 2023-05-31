// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CitizenBotCharacter.h"
#include "../../../Components/Actor/WeaponsFactoryComponent.h"
#include "FightingBotCharacter.generated.h"


UCLASS()
class PRAISE_API AFightingBotCharacter : public ACitizenBotCharacter
{
	GENERATED_BODY()
public:
	AFightingBotCharacter();
	FORCEINLINE bool ShouldFightToDeath() const { return bShouldFightToDeath; }
	FORCEINLINE void SetFightToDeath(bool bValue) { bShouldFightToDeath = bValue; }
protected:
	virtual void SetupCharWeaponSlots() override;
	virtual void EquipWeapons(EWeaponSlot Hand = EWeaponSlot::INVALID, bool bTryDualEquip = true) override;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = CombatSystem)
		bool bShouldFightToDeath;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = CombatSystem)
		bool bStartWithRandomWeapons;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatSystem)
		TMap<EWeaponSlot, EWeaponClass> DefaultWeapons;

	void SetupDefaultWeapons();
	EWeaponClass GetAvailableDefaultWeapon(EWeaponSlot CharHand);
	void AddWeaponToSlot(UCharWeaponSlot* Slot, EWeaponClass SlotWeaponClass);
	virtual void SetCharacterDead(AActor* Killer) override;

	UPROPERTY()
		UWeaponsFactoryComponent* CharWeaponsFactory;
};
