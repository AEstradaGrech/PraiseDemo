// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipWeaponNotify.h"
#include "../../Characters/BasePraiseCharacter.h"

void UEquipWeaponNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp->GetOwner()) return;

	if (!MeshComp->GetOwner()->IsA<ABasePraiseCharacter>()) return;
	
	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());
	TMap<EWeaponSlot, UCharWeaponSlot*> CharWeapons;

	bool bIsEquip = Char->IsEquippingWeapon();
	FName Socket = FName();
	if (bIsEquip)
	{
		Socket = CharWeapon == EWeaponSlot::PRIMARY_WEAPON ? FName("Hand_R_Socket") : FName("Hand_L_Socket");
		CharWeapons = Char->GetCharCurrentWeapons();
	}
	else
	{
		Socket = FUtilities::GetSocketName(HolsterPosition);
		CharWeapons = Char->GetCharPreviousWeapons();
	}

	if (CharWeapons.Num() <= 0) return;
	
	if (!CharWeapons.Contains(CharWeapon)) return;

	if (!CharWeapons[CharWeapon] || CharWeapons[CharWeapon]->IsEmpty()) return;
	
	Char->HandleWeaponSlotEquip(CharWeapons[CharWeapon], bIsEquip, Socket);
}

void UEquipWeaponNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{

}


