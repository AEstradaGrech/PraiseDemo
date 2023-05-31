// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Enums/CombatSystem/EWeaponSlot.h"
#include "../Enums/Animation/EHolsterPosition.h"
#include "../Weapons/Weapon.h"
#include "CharWeaponSlot.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UCharWeaponSlot : public UObject
{
	GENERATED_BODY()
	
public:
	UCharWeaponSlot();

	
	FORCEINLINE EWeaponSlot GetSlotID() const { return SlotID; }
	FORCEINLINE FString GetSlotName() const { return SlotName; }
	FORCEINLINE bool IsLocked() const { return bIsLocked; }
	FORCEINLINE bool IsEquiped() const { return bIsEquiped; }
	FORCEINLINE bool IsEmpty() const { return SlotWeapon == nullptr ? true : false; }
	FORCEINLINE EHolsterPosition GetHolsterPosition() const { return HolsterPosition; }
	FORCEINLINE AWeapon* GetSlotWeapon() const { return SlotWeapon; }
	void SetIsEquiped(bool Value, EWeaponSlot Hand);
	EWeaponSlot GetEquipedHand();
	bool HasDefaultFist();
	void SetSlotID(EWeaponSlot ID);
	void SetupSlot(EWeaponSlot ID, AWeapon* SlotWeapon = nullptr, bool bSetEquiped = false, EWeaponSlot EquipedHand = EWeaponSlot::INVALID, EHolsterPosition Holster = EHolsterPosition::UNARMED);
	void ChangeHolsterPosition(EHolsterPosition Position);
	bool CanSetHolsterPosition(EHolsterPosition NewPosition);
	bool CanShareEquipAnim(UCharWeaponSlot* OtherSlot);
	void SetSlotWeapon(AWeapon* NewWeapon, EHolsterPosition Holster);
	bool IsShareableEquipAnim(EHolsterPosition Holster);
	AWeapon* ClearSlot();
	EWeaponSlot GetHolsterEquippingHand() const;

	template<class T>
	bool HoldsA() const;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		EWeaponSlot SlotID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FString SlotName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		EHolsterPosition HolsterPosition;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		bool bIsLocked;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		bool bIsEquiped;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		AWeapon* SlotWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		EWeaponSlot EquipedHand;
};

template<class T>
inline bool UCharWeaponSlot::HoldsA() const
{
	if (!SlotWeapon) return false;

	return SlotWeapon->IsA<T>();
}
