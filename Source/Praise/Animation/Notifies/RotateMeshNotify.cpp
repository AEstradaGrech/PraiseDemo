// Fill out your copyright notice in the Description page of Project Settings.


#include "RotateMeshNotify.h"
#include "../../Weapons/Firearm.h"
#include "../../Inventory/CharWeaponSlot.h"
#include "../../Characters/Player/PraisePlayerCharacter.h"
void URotateMeshNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp->GetOwner()) return;

	if (!MeshComp->GetOwner()->IsA<APraisePlayerCharacter>()) return;

	if (WeaponHand == EWeaponSlot::INVALID) return;

	APraisePlayerCharacter* Char = Cast<APraisePlayerCharacter>(MeshComp->GetOwner());
	if (!Char) return;
	
	AWeapon* WeaponToRotate = nullptr;
	
	if (Char->GetCharCurrentWeapons().Num() <= 0 || !Char->GetCharCurrentWeapons().Contains(WeaponHand) || Char->GetCharCurrentWeapons()[WeaponHand]->IsEmpty()) return;

	WeaponToRotate = Char->GetCharCurrentWeapons()[WeaponHand]->GetSlotWeapon();

	if (WeaponToRotate) {
		Char->RotateWeapon(WeaponHand, NewRotation);
	}
	
}


