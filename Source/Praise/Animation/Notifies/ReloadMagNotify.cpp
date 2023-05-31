// Fill out your copyright notice in the Description page of Project Settings.


#include "ReloadMagNotify.h"
#include "../../Characters/BasePraiseCharacter.h"
#include "../../Weapons/Firearm.h"

void UReloadMagNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp->GetOwner()) return;

	if (!MeshComp->GetOwner()->IsA<ABasePraiseCharacter>()) return;

	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());
	TMap<EWeaponSlot, UCharWeaponSlot*> CharWeapons = Char->GetCharCurrentWeapons();

	if (CharWeapons.Num() <= 0) return;

	if (!CharWeapons.Contains(CharWeapon)) return;

	if (!CharWeapons[CharWeapon] || CharWeapons[CharWeapon]->IsEmpty()) return;

	if (!CharWeapons[CharWeapon]->HoldsA<AFirearm>()) return;

	AFirearm* Firearm = Cast<AFirearm>(CharWeapons[CharWeapon]->GetSlotWeapon());

	if (Firearm->GetAmmo() >= Firearm->GetMagAmmo()) return;

	Firearm->ReloadMag(ReloadedBullets, bIsMagOverwrite);
}
