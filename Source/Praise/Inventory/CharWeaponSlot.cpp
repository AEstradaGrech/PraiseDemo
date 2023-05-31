// Fill out your copyright notice in the Description page of Project Settings.


#include "CharWeaponSlot.h"
#include "../Structs/CommonUtility/FLogger.h"
#include "../Structs/CommonUtility/FUtilities.h"
#include "../Weapons/Fist.h"

UCharWeaponSlot::UCharWeaponSlot() 
{

}

void UCharWeaponSlot::SetIsEquiped(bool Value, EWeaponSlot Hand)
{
	bIsEquiped = Value; 
	EquipedHand = Hand;
}

EWeaponSlot UCharWeaponSlot::GetEquipedHand()
{
	return EquipedHand;
}

bool UCharWeaponSlot::HasDefaultFist()
{
	if (!SlotWeapon) return false;

	return SlotWeapon->IsA<AFist>() && SlotWeapon->GetWeaponModel() == FString("Default");
}

void UCharWeaponSlot::SetSlotID(EWeaponSlot ID)
{
	SlotID = ID;

	SlotName = FUtilities::EnumToString(TEXT("EWeaponSlot"), (int32)SlotID);
}

void UCharWeaponSlot::SetupSlot(EWeaponSlot ID, AWeapon* Weapon, bool bSetEquiped, EWeaponSlot CurrentHand, EHolsterPosition Holster)
{
	SetSlotID(ID);

	SlotWeapon = nullptr;
	bIsEquiped = false;
	HolsterPosition = Holster;
	EquipedHand = CurrentHand;

	switch (ID) {
		case(EWeaponSlot::PRIMARY_WEAPON):
		case(EWeaponSlot::SECONDARY_WEAPON):
			bIsLocked = false;
			break;
		case(EWeaponSlot::EXTRA_SLOT_1):
		case(EWeaponSlot::EXTRA_SLOT_2):
		case(EWeaponSlot::INVALID):
			bIsLocked = true;
			break;
	}

	if (bIsLocked) return;

	if (Weapon)
	{
		SlotWeapon = Weapon;
		bIsEquiped = bSetEquiped;
		EHolsterPosition HandHolster = SlotWeapon->GetHolsterForHand(CurrentHand);

		HolsterPosition = HolsterPosition == EHolsterPosition::UNARMED ? HandHolster == EHolsterPosition::UNARMED ? SlotWeapon->GetHolsterPositions()[0] : HandHolster : Holster;
	}
}

void UCharWeaponSlot::ChangeHolsterPosition(EHolsterPosition Position)
{
	if (CanSetHolsterPosition(Position))
		HolsterPosition = Position;
}

bool UCharWeaponSlot::CanSetHolsterPosition(EHolsterPosition NewPosition)
{
	if (bIsLocked) return false;
	if (bIsEquiped) return false;
	
	return true;
}


bool UCharWeaponSlot::CanShareEquipAnim(UCharWeaponSlot* OtherSlot)
{
	if (OtherSlot->IsEmpty()) return true; 
	
	if (OtherSlot->GetHolsterPosition() == EHolsterPosition::UNARMED) return true;

	switch (HolsterPosition)
	{
		case EHolsterPosition::UNARMED:
				
			if (OtherSlot->GetHolsterPosition() == EHolsterPosition::UNARMED) return true;
			if (!OtherSlot->GetSlotWeapon()->IsTwoHand()) return true;
			return false;
		case EHolsterPosition::SHOULDER_R:
			if (OtherSlot->GetHolsterPosition() == EHolsterPosition::LOW_CHEST_L) return true;
			if (OtherSlot->GetHolsterPosition() == EHolsterPosition::LOW_BACK_L) return true;
			return false;
		case EHolsterPosition::LOW_BACK_L:
			if (OtherSlot->GetHolsterPosition() == EHolsterPosition::SHOULDER_R) return true;
			if (OtherSlot->GetHolsterPosition() == EHolsterPosition::HOLSTER_R) return true;
			if (OtherSlot->GetHolsterPosition() == EHolsterPosition::THIGH_R && OtherSlot->GetSlotWeapon()->GetWeaponClass() != EWeaponClass::SWORD) return true;
			return false;
			break;
		case EHolsterPosition::LOW_BACK_R:
			if (OtherSlot->GetHolsterPosition() == EHolsterPosition::LOW_BACK_L) return true;
			if (OtherSlot->GetHolsterPosition() == EHolsterPosition::THIGH_R) return true;
			return false;
		case EHolsterPosition::THIGH_R:
			if (SlotWeapon)
			{
				if (SlotWeapon->GetWeaponClass() == EWeaponClass::SWORD)
				{
					if (OtherSlot->GetHolsterPosition() == EHolsterPosition::THIGH_L && OtherSlot->GetSlotWeapon()->GetWeaponClass() == EWeaponClass::SWORD) return true;
					if (OtherSlot->GetHolsterPosition() == EHolsterPosition::THIGH_R && OtherSlot->GetSlotWeapon()->GetWeaponClass() != EWeaponClass::SWORD) return true;
				}
				if (SlotWeapon->GetWeaponClass() == EWeaponClass::HAMMER || SlotWeapon->GetWeaponClass() == EWeaponClass::AXE)
				{
					if (OtherSlot->GetHolsterPosition() == EHolsterPosition::THIGH_L && OtherSlot->GetSlotWeapon()->GetWeaponClass() != EWeaponClass::SWORD) return true;
					if (OtherSlot->GetHolsterPosition() == EHolsterPosition::THIGH_R && OtherSlot->GetSlotWeapon()->GetWeaponClass() == EWeaponClass::SWORD) return true;
				}

				if (OtherSlot->GetHolsterPosition() == EHolsterPosition::LOW_BACK_L)
				{
					if (SlotWeapon->GetWeaponClass() == EWeaponClass::HAMMER || SlotWeapon->GetWeaponClass() == EWeaponClass::AXE) return true;
				}
			}
			
			if (OtherSlot->GetHolsterPosition() == EHolsterPosition::THIGH_R && OtherSlot->GetSlotWeapon()->GetWeaponClass() == EWeaponClass::SWORD) return true;
			
			return false;
		case EHolsterPosition::THIGH_L:
			if (OtherSlot->GetHolsterPosition() == EHolsterPosition::THIGH_R) return true;
			return false;
		case EHolsterPosition::CHEST_L:
		case EHolsterPosition::LOW_CHEST_L:
			break;
		case EHolsterPosition::HOLSTER_R:
			if (OtherSlot->GetHolsterPosition() == EHolsterPosition::LOW_BACK_L) return true;
			return false;
		default:
			return OtherSlot->GetHolsterPosition() == EHolsterPosition::UNARMED;
	}

	return false;
}

void UCharWeaponSlot::SetSlotWeapon(AWeapon* NewWeapon, EHolsterPosition Holster)
{
	if (bIsLocked) return; 

	SlotWeapon = NewWeapon;
	HolsterPosition = Holster;
	
}

bool UCharWeaponSlot::IsShareableEquipAnim(EHolsterPosition Holster)
{
	return false;
}

AWeapon* UCharWeaponSlot::ClearSlot()
{
	if (!SlotWeapon) return nullptr;

	AWeapon* Wp = SlotWeapon;
	
	SlotWeapon = nullptr;
	bIsEquiped = false;
	EquipedHand = EWeaponSlot::INVALID;
	HolsterPosition = EHolsterPosition::UNARMED;
	return Wp;
}

EWeaponSlot UCharWeaponSlot::GetHolsterEquippingHand() const
{
	switch (HolsterPosition)
	{
		case EHolsterPosition::UNARMED:
		case EHolsterPosition::SHOULDER_R:
		case EHolsterPosition::LOW_BACK_R:
		case EHolsterPosition::HOLSTER_R:
			return EWeaponSlot::PRIMARY_WEAPON;

		case(EHolsterPosition::LOW_BACK_L):
		case EHolsterPosition::CHEST_L:
		case EHolsterPosition::LOW_CHEST_L:
			return EWeaponSlot::SECONDARY_WEAPON;

		case EHolsterPosition::THIGH_R:

			if (SlotWeapon)
				return SlotWeapon->GetWeaponClass() == EWeaponClass::SWORD ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON;
			
			else return EWeaponSlot::PRIMARY_WEAPON;
		case EHolsterPosition::THIGH_L:

			if (SlotWeapon)
				return SlotWeapon->GetWeaponClass() == EWeaponClass::SWORD ? EWeaponSlot::PRIMARY_WEAPON : EWeaponSlot::SECONDARY_WEAPON;

			else return EWeaponSlot::SECONDARY_WEAPON;
		default:
			return EWeaponSlot::PRIMARY_WEAPON;
	}

	return EWeaponSlot::PRIMARY_WEAPON;
}

