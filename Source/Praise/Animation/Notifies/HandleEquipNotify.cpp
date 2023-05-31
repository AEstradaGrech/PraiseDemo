// Fill out your copyright notice in the Description page of Project Settings.


#include "HandleEquipNotify.h"
#include "../../Characters/BasePraiseCharacter.h"

void UHandleEquipNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{

	if (!MeshComp->GetOwner()) return;

	if (!MeshComp->GetOwner()->IsA<ABasePraiseCharacter>()) return;

	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());
}


void UHandleEquipNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp->GetOwner()) return;

	if (!MeshComp->GetOwner()->IsA<ABasePraiseCharacter>()) return;

	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());

	Char->ResetEquipingStuff();
}
