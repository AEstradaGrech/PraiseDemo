// Fill out your copyright notice in the Description page of Project Settings.


#include "ComboRequestNotify.h"
#include "../../Characters/BasePraiseCharacter.h"
#include "../../Components/Actor/AnimInstances/PraiseAnimInstance.h"
void UComboRequestNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	SetCanRequestCombo(MeshComp);
}

void UComboRequestNotify::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	SetCanRequestCombo(MeshComp);
}

void UComboRequestNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp->GetOwner()) return;

	if (!MeshComp->GetOwner()->IsA<ABasePraiseCharacter>()) return;

	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());

	Char->ResetComboStuff();
}

void UComboRequestNotify::SetCanRequestCombo(USkeletalMeshComponent* MeshComp)
{
	if (!MeshComp->GetOwner()) return;

	if (!MeshComp->GetOwner()->IsA<ABasePraiseCharacter>()) return;

	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());

	bool bCanInterruptAttack = Char->GetCurrentPose() == EArmedPoses::RIFLE; 
	
	Char->SetCanRequestCombo(true, bCanInterruptAttack);
}


