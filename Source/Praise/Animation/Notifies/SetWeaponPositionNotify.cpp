// Fill out your copyright notice in the Description page of Project Settings.


#include "SetWeaponPositionNotify.h"
#include "../../Characters/BasePraiseCharacter.h"
#include "../../Weapons/Weapon.h"
#include "../../Weapons/Firearm.h"

void USetWeaponPositionNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (WeaponHand == EWeaponSlot::INVALID) return;

	if (!MeshComp->GetOwner()) return;

	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());

	if (!Char) return;

	if (!Char->IsWeaponEquiped()) return;

	if (!Char->GetCharCurrentWeapons().Contains(WeaponHand)) return;

	if (Char->GetCharCurrentWeapons()[WeaponHand]->IsEmpty()) return;

	AWeapon* Weapon = Char->GetCharCurrentWeapons()[WeaponHand]->GetSlotWeapon();

	FVector Loc;
	FRotator Rot;
	switch (AnimAction)
	{
		case(ECharAnimState::IDLE):
			Loc = Weapon->GetEquipLocation();
			Rot = Weapon->GetEquipRotation();
			break;
		case(ECharAnimState::BLOCKING):
			Loc = Weapon->GetBlockLocation();
			Rot = Weapon->GetBlockRotation();
			break;
		case(ECharAnimState::AIMING):

			if (!Weapon->IsA<AFirearm>()) return;

			Loc = Cast<AFirearm>(Weapon)->GetAimingLocation();
			Rot = Cast<AFirearm>(Weapon)->GetAimingRotation();

			break;
		default:
			break;
	}

	Weapon->Equip(true, Loc, Rot, false);
}

