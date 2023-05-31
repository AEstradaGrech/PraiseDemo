// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeaponAttackNotify.h"
#include "../../Characters/BasePraiseCharacter.h"

UMeleeWeaponAttackNotify::UMeleeWeaponAttackNotify()
{

}

void UMeleeWeaponAttackNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp->GetOwner()) return;

	if (!MeshComp->GetOwner()->IsA<ABasePraiseCharacter>()) return;

	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());

	if (!Char) return;

	Char->EnableWeaponCollider(EnabledWeaponHand, AttackType, true);
	
	if(!Char->GetCharAudioComp()->IsPlaying())
		Char->PlayCharSFX(ECharSFX::CHAR_ATTACK);
	
}

void UMeleeWeaponAttackNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp->GetOwner()) return;

	if (!MeshComp->GetOwner()->IsA<ABasePraiseCharacter>()) return;

	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());

	if (!Char) return;

	Char->EnableWeaponCollider(EnabledWeaponHand, AttackType, false);
}
