// Fill out your copyright notice in the Description page of Project Settings.


#include "FistWeaponAttackNotify.h"
#include "../../Characters/BasePraiseCharacter.h"
void UFistWeaponAttackNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp->GetOwner()) return;

	if (!MeshComp->GetOwner()->IsA<ABasePraiseCharacter>()) return;

	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());

	bool bIsKick = Char->GetCurrentAttackType() == EAttackType::KICK;
	
	switch (bIsKick)
	{
		case(true):
			Char->EnableKickCollider(true, /*bIsRightLeg: */ bIsRightHand);
		break;
		case(false):
			Char->EnableHandCollider(true, /*bIsRightHand: */ bIsRightHand);
		break;
	}
}

void UFistWeaponAttackNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp->GetOwner()) return;

	if (!MeshComp->GetOwner()->IsA<ABasePraiseCharacter>()) return;

	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());

	bool bIsKick = Char->GetCurrentAttackType() == EAttackType::KICK;

	switch (bIsKick)
	{
		case(true):
			Char->EnableKickCollider(false, /*bIsRightLeg: */ bIsRightHand);
		break;
		case(false):
			Char->EnableHandCollider(false, /*bIsRightHand: */ bIsRightHand);
		break;
	}
}
