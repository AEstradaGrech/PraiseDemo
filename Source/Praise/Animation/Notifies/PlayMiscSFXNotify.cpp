// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayMiscSFXNotify.h"
#include "../../Characters/BasePraiseCharacter.h"
#include "../../Structs/CommonUtility/FUtilities.h"

void UPlayMiscSFXNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (MiscFX == EMiscSFX::NONE) return;

	if (!MeshComp->GetOwner()) return;

	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());

	if (!Char) return;

	if (MiscFX != EMiscSFX::FOOTSTEPS)
	{
		if (MaxSamples <= 0)
			MaxSamples = 1;

		int SampleIdx = FMath::RandRange(0, MaxSamples);

		FName ParamName = FUtilities::GetMiscSFXParamName(MiscFX);

		Char->PlayMiscSFX(MiscFX, ParamName, SampleIdx);
	}

	else Char->PlayFootstepSFX();
}



