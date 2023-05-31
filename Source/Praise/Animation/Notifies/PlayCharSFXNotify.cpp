// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayCharSFXNotify.h"
#include "../../Characters/BasePraiseCharacter.h"
#include "../../Structs/CommonUtility/FUtilities.h"

void UPlayCharSFXNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (CharFX == ECharSFX::NONE) return;

	if (!MeshComp->GetOwner()) return;

	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());

	if (!Char) return;

	FName ParamName = FUtilities::GetCharSFXParamName(CharFX);

	if (bIsRandomSample && MaxSamples < 0)
		MaxSamples = 0;

	if (SampleIdx < 0)
		SampleIdx = 0;

	SampleIdx = FMath::RandRange(SampleIdx, MaxSamples);

	Char->PlayCharSFX(CharFX, ParamName, bIsRandomSample ? FMath::RandRange(0, MaxSamples) : SampleIdx);

}