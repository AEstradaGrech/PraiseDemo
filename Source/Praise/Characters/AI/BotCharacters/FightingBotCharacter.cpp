// Fill out your copyright notice in the Description page of Project Settings.


#include "FightingBotCharacter.h"
#include "../../../Components/Actor/AI/BotBrainComponent.h"
#include "../../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../../../Components/Actor/AI/T200BrainComponent.h"
#include "../../../Components/Actor/CharStats/BotStatsComponent.h"
#include "../../../Interfaces/CombatSystem/Targeteable.h"
#include "../../../Networking/PraiseGameState.h"
#include "../../../Components/GameMode/MachineStatesFactoryComponent.h"
#include "../../../Components/Actor/AnimInstances/PraiseAnimInstance.h"
#include "../../../Weapons/Fist.h"
#include "../../../Weapons/MeleeWeapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../../Weapons/Firearm.h"

AFightingBotCharacter::AFightingBotCharacter() : Super()
{
	CharWeaponsFactory = CreateDefaultSubobject<UWeaponsFactoryComponent>(TEXT("Weapons Factory Component"));
	CharArmorsFactory = CreateDefaultSubobject<UArmorsFactoryComponent>(TEXT("Armors Factory"));

	BotClass = EBotClass::FIGHTER;
	BotBrainType = EBrainType::T200;

	static ConstructorHelpers::FObjectFinder<USoundCue> CombatQ(TEXT("SoundCue'/Game/Core/Audio/Combat/CombatFX.CombatFX'"));

	if (CombatQ.Succeeded())
		CombatSoundCue = CombatQ.Object;
}

void AFightingBotCharacter::SetupCharWeaponSlots()
{
	SetupDefaultWeapons();
}

void AFightingBotCharacter::SetCharacterDead(AActor* Killer)
{
	if (HasWeapon())
	{
		for (int i = 0; i < CharWeaponSlots.Num(); i++)
			if (!CharWeaponSlots[(EWeaponSlot)i]->IsEmpty())
				CharWeaponSlots[(EWeaponSlot)i]->ClearSlot()->OnWeaponDropped();
	}

	Super::SetCharacterDead(Killer);
}

void AFightingBotCharacter::EquipWeapons(EWeaponSlot Slot, bool bTryDualEquip)
{
	if (!CanExecuteAction(ECharAction::EQUIP_WEAPON)) return;

	EquippingHand = -1;

	EWeaponSlot SecondSlot = Slot == EWeaponSlot::INVALID ? EWeaponSlot::INVALID : Slot == EWeaponSlot::PRIMARY_WEAPON ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON;

	AWeapon* PrimaryWeapon = nullptr;
	AWeapon* SecondaryWeapon = nullptr;

	if (CharWeaponSlots.Num() <= 0) return;

	if (IsWeaponEquiped())
	{
		if (Slot == EWeaponSlot::INVALID)
		{
			Slot = GetCurrentMainWeapon()->GetEquipedHand();
			EWeaponSlot SecondHand = Slot == EWeaponSlot::PRIMARY_WEAPON ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON;

			SecondSlot = CurrentWeapons.Contains(SecondHand) && !CurrentWeapons[SecondHand]->IsEmpty() ? SecondHand : EWeaponSlot::INVALID;
		}

		if (CurrentWeapons.Contains(Slot))
		{
			if (!CurrentWeapons[Slot]->IsEmpty() && CurrentWeapons[Slot]->IsEquiped())
			{
				if (!CurrentWeapons[Slot]->GetSlotWeapon()->IsTwoHand())  
				{
					if (SecondSlot != EWeaponSlot::INVALID && CurrentWeapons.Contains(SecondSlot) && CurrentWeapons[SecondSlot]->HoldsA<AWeapon>() && !CurrentWeapons[SecondSlot]->GetSlotWeapon()->IsTwoHand())
					{
						SecondaryWeapon = CurrentWeapons[SecondSlot]->GetSlotWeapon();

						EquipSlotWeapon(CurrentWeapons[Slot],
							/*bIsEquip*/ false,
							/*EquipHand*/ CurrentWeapons[Slot]->GetHolsterEquippingHand(),
							/*bIsDualEquip*/ CurrentWeapons[Slot]->HoldsA<AFist>() && (CurrentWeapons[SecondSlot]->HoldsA<AFist>() || CurrentWeapons[SecondSlot]->IsEmpty()) ? false : bTryDualEquip,
							/*SecondaryWeapon*/  CurrentWeapons[Slot]->HoldsA<AFist>() && (CurrentWeapons[SecondSlot]->HoldsA<AFist>() || CurrentWeapons[SecondSlot]->IsEmpty()) ? nullptr : CurrentWeapons[SecondSlot],
							/*bShouldLinkAnims:*/ CurrentWeapons[Slot]->HoldsA<AFist>() && (CurrentWeapons[SecondSlot]->HoldsA<AFist>() || CurrentWeapons[SecondSlot]->IsEmpty()) ? false : !CurrentWeapons[Slot]->CanShareEquipAnim(CurrentWeapons[SecondSlot])); // desenfunda las dos armas con la misma animacion
					
					}

					else EquipSlotWeapon(CurrentWeapons[Slot], /*bIsEquip*/ false, Slot);
				}

				else EquipSlotWeapon(CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON], /*bIsEquip*/ false, EWeaponSlot::PRIMARY_WEAPON); 

			}

			else FLogger::LogTrace(__FUNCTION__ + FString("CURRENT SLOT NOT EQUIPED"));
		}

		else FLogger::LogTrace(__FUNCTION__ + FString("CURRENT WEAPONS DOES NOT CONTAIN SLOT"));
	}
	else
	{
		if (Slot == EWeaponSlot::INVALID)
		{
			Slot = EWeaponSlot::PRIMARY_WEAPON;
			SecondSlot = CharWeaponSlots.Contains(EWeaponSlot::SECONDARY_WEAPON) && !CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->IsEmpty() ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::INVALID;
		}

		if (!CharWeaponSlots[Slot]->IsEmpty())
		{
			PrimaryWeapon = CharWeaponSlots[Slot]->GetSlotWeapon();

			if (PrimaryWeapon->IsTwoHand())
			{
				EquipSlotWeapon(CharWeaponSlots[Slot], true, EWeaponSlot::PRIMARY_WEAPON);
				return;
			}
			else
			{
				if (SecondSlot != EWeaponSlot::INVALID && !CharWeaponSlots[SecondSlot]->IsEmpty())
				{
					SecondaryWeapon = CharWeaponSlots[SecondSlot]->GetSlotWeapon();

					if (!SecondaryWeapon->IsTwoHand())
					{
						if ((CharWeaponSlots[Slot]->GetHolsterEquippingHand() != CharWeaponSlots[SecondSlot]->GetHolsterEquippingHand()) && bTryDualEquip)
						{
							EquipSlotWeapon(CharWeaponSlots[Slot],
								/*bIsEquip*/ true,
								/*EquipHand*/ CharWeaponSlots[Slot]->GetHolsterEquippingHand(),
								/*bIsDualEquip*/  bTryDualEquip,
								/*SecondaryWeapon*/ CharWeaponSlots[SecondSlot],
								/*bShouldLinkAnims*/!CharWeaponSlots[Slot]->CanShareEquipAnim(CharWeaponSlots[SecondSlot]));
						}

						else EquipSlotWeapon(CharWeaponSlots[Slot], true, CharWeaponSlots[Slot]->GetHolsterEquippingHand()); 
					}

					else EquipSlotWeapon(CharWeaponSlots[Slot], true, CharWeaponSlots[Slot]->GetHolsterEquippingHand(), false);
				}

				else EquipSlotWeapon(CharWeaponSlots[Slot], true, CharWeaponSlots[Slot]->GetHolsterEquippingHand()); 
			}
		}
	}
}

void AFightingBotCharacter::SetupDefaultWeapons()
{
	for (int i = 0; i < 4; i++)
	{
		UCharWeaponSlot* Slot = NewObject<UCharWeaponSlot>(this);
		
		Slot->SetupSlot(EWeaponSlot(i));
		
		if (!CharWeaponSlots.Contains(Slot->GetSlotID()))
		{
			CharWeaponSlots.Add(Slot->GetSlotID(), Slot);
		}

		EWeaponClass DefaultWeaponClass = EWeaponClass::UNARMED;
		
		if (bStartWithRandomWeapons)
		{
			if(!Slot->IsLocked())
				AddWeaponToSlot(Slot, GetAvailableDefaultWeapon((EWeaponSlot)i));
			
			continue;
		}

		if (DefaultWeapons.Num() <= 0)
		{
			if (i == 0)
				AddDefaultUnarmedWeapon();
			
			return; 
		}

		EHolsterPosition WeaponHolster = EHolsterPosition::UNARMED;
		if (DefaultWeapons.Contains((EWeaponSlot)i) && !Slot->IsLocked())
		{
			AddWeaponToSlot(Slot, DefaultWeapons[(EWeaponSlot)i]);
		}
	}

	if ((CharWeaponSlots.Contains(EWeaponSlot::PRIMARY_WEAPON) && !CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->IsEmpty()) && 
		(CharWeaponSlots.Contains(EWeaponSlot::SECONDARY_WEAPON) && !CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->IsEmpty()))
	{
		if (!CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->IsTwoHand() &&
			CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->GetSlotWeapon()->GetWeaponClass() != EWeaponClass::SHIELD)
		{
			if (!CharWeaponsFactory) return;

			float rnd = FMath::RandRange(0, 100);

			if (rnd > 80.f) return;

			AWeapon* Shield = CharWeaponsFactory->SpawnWeapon<AMeleeWeapon>(EHolsterPosition::LOW_BACK_L, EWeaponType::SHIELD, DEFAULT_MID_SHIELD_MODEL);

			if (!Shield)
			{
				GetDamage(-GetHealth(), this);
				return;
			}
			else
			{
				AttachWeapon(Shield, EHolsterPosition::LOW_BACK_L);
				CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->ClearSlot()->Destroy();
				CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->SetSlotWeapon(Shield, EHolsterPosition::LOW_BACK_L);
			}
		}
	}
	
}

EWeaponClass AFightingBotCharacter::GetAvailableDefaultWeapon(EWeaponSlot CharHand)
{
	bool bIsAvailable = false;

	TArray<EWeaponClass> AvailableWeapons;

	for (int i = 0; i < (int)FUtilities::GetEnumCount(*FString("EWeaponClass")); i++)
	{
		switch ((EWeaponClass)i)
		{
		case EWeaponClass::UNARMED:
			bIsAvailable = CharHand != EWeaponSlot::PRIMARY_WEAPON;
				break;
		case EWeaponClass::PIKE:
		case EWeaponClass::HAMMER: 
		case EWeaponClass::SWORD:
		case EWeaponClass::AXE:
			bIsAvailable = CharHand != EWeaponSlot::SECONDARY_WEAPON;
				break;
		case EWeaponClass::RIFLE:
			bIsAvailable = false; 
			break;
		case EWeaponClass::SHIELD:

			bIsAvailable = CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->IsEmpty() ? false : CharHand == EWeaponSlot::SECONDARY_WEAPON && CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->GetWeaponClass() == EWeaponClass::HAMMER;
			break;
		
		case EWeaponClass::HANDGUN:
		case EWeaponClass::SHOTGUN:
		default:
			bIsAvailable = false;
			break;
		}

		if (bIsAvailable)
			AvailableWeapons.Add((EWeaponClass)i);

	}

	return AvailableWeapons.Num() > 0 ? AvailableWeapons[FMath::RandRange(0, AvailableWeapons.Num() -1)] : EWeaponClass::UNARMED;
}

void AFightingBotCharacter::AddWeaponToSlot(UCharWeaponSlot* Slot, EWeaponClass SlotWeaponClass)
{
	if (!CharWeaponsFactory) return;

	AWeapon* Weapon = nullptr;
	EHolsterPosition WeaponHolster = EHolsterPosition::UNARMED;

	switch (SlotWeaponClass)
	{
		case(EWeaponClass::UNARMED):

			WeaponHolster = EHolsterPosition::UNARMED;

			Weapon = SpawnDefaultUnarmedWeapon();

			if (Weapon) 
			{
				if (!Weapon->DidInit())
					Weapon->TryInitStats();

				if (Weapon->DidInit()) 
					AttachWeapon(Weapon, Slot->GetSlotID() == EWeaponSlot::PRIMARY_WEAPON ? FName("Hand_R") : FName("Hand_L"), false);	
			}

			break;
		case(EWeaponClass::RIFLE):
			WeaponHolster = EHolsterPosition::SHOULDER_R;
			Weapon = CharWeaponsFactory->SpawnWeapon<AFirearm>(WeaponHolster, EWeaponType::FIREARM, DEFAULT_FIREARM_MODEL);
			break;
		case(EWeaponClass::PIKE):
			WeaponHolster = EHolsterPosition::SHOULDER_R;
			Weapon = FMath::RandRange(0, 100) < 80.f ?  
				CharWeaponsFactory->SpawnWeapon<AMeleeWeapon> (WeaponHolster, EWeaponType::POINTED, DEFAULT_PIKE_2H_MODEL) :
				CharWeaponsFactory->SpawnWeapon<AMeleeWeapon>(WeaponHolster, EWeaponType::POINTED, DEFAULT_PIKE_1H_MODEL);
			break;
		case(EWeaponClass::HAMMER):
			WeaponHolster = EHolsterPosition::THIGH_R;
			Weapon = CharWeaponsFactory->SpawnWeapon<AMeleeWeapon>(WeaponHolster, EWeaponType::BLUNT, DEFAULT_BLUNT_1H_MODEL);
			break;
		case(EWeaponClass::AXE):
			WeaponHolster = EHolsterPosition::THIGH_R;
			Weapon = CharWeaponsFactory->SpawnWeapon<AMeleeWeapon>(WeaponHolster, EWeaponType::SHARP, DEFAULT_SHARP_1H_MODEL);
			break;
		case(EWeaponClass::SWORD):
			WeaponHolster = EHolsterPosition::THIGH_L;
			Weapon = CharWeaponsFactory->SpawnWeapon<AMeleeWeapon>(WeaponHolster, EWeaponType::POINTED, DEFAULT_SWORD_1H_MODEL);
			break;
		case(EWeaponClass::SHIELD):
			WeaponHolster = EHolsterPosition::LOW_BACK_L;
			Weapon = CharWeaponsFactory->SpawnWeapon<AMeleeWeapon>(WeaponHolster, EWeaponType::SHIELD, DEFAULT_MID_SHIELD_MODEL);
			break;
		default:
			break;
	}

	if (Weapon && Weapon->DidInit())
	{
		AttachWeapon(Weapon, WeaponHolster);
		Slot->SetSlotWeapon(Weapon, WeaponHolster);
	}

	if (!CharWeaponSlots.Contains(Slot->GetSlotID()))
		CharWeaponSlots.Add(Slot->GetSlotID(), Slot);
}
