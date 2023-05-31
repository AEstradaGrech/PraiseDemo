// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimationFinishNotify.h"
#include "../../Characters/BasePraiseCharacter.h"

void UAnimationFinishNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (bSkipReset) return;

	if (!MeshComp->GetOwner()) return;

	if (!MeshComp->GetOwner()->IsA<ABasePraiseCharacter>()) return;

	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());

	if (!Char) return;

	Char->OnAnimStateUpdate(AnimState, false);
}