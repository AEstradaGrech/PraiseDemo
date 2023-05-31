// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupItemNotify.h"
#include "../../Characters/BasePraiseCharacter.h"

void UPickupItemNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp->GetOwner()) return;

	if (!MeshComp->GetOwner()->IsA<ABasePraiseCharacter>()) return;

	if (PickingHand != EWeaponSlot::PRIMARY_WEAPON && PickingHand != EWeaponSlot::SECONDARY_WEAPON) return;

	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());

	Char->PickWeapon(PickingHand);
}


void UPickupItemNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp->GetOwner()) return;

	if (!MeshComp->GetOwner()->IsA<ABasePraiseCharacter>()) return;

	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());

	Char->OnAnimStateUpdate(ECharAnimState::PICKING_ITEM, false);
}