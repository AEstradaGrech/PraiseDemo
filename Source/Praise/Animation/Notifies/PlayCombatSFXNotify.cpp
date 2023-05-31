// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayCombatSFXNotify.h"
#include "../../Characters/BasePraiseCharacter.h"
#include "../../Structs/CommonUtility/FUtilities.h"

void UPlayCombatSFXNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (CombatFX == ECombatSFX::NONE) return;

	if (!MeshComp->GetOwner()) return;

	ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(MeshComp->GetOwner());

	if (!Char) return;

	FName ParamName = FUtilities::GetCombatSFXParamName(CombatFX);

	if (bIsRandomSample && MaxSamples < 0)
		MaxSamples = 0;
	
	if (SampleIdx < 0)
		SampleIdx = 0;

	SampleIdx = FMath::RandRange(SampleIdx, MaxSamples);

	Char->PlayCombatSFX(CombatFX, ParamName, bIsRandomSample ? FMath::RandRange(0, MaxSamples) : SampleIdx);
	
}


