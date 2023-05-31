// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "../../Enums/CombatSystem/ECombatSFX.h"
#include "../../Enums/CommonUtility/EMiscSFX.h"
#include "Sound/SoundCue.h"
#include "PlayCombatSFXNotify.generated.h"


/**
 * 
 */
UCLASS()
class PRAISE_API UPlayCombatSFXNotify : public UAnimNotifyState
{
	GENERATED_BODY()

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECombatSFX CombatFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsRandomSample;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int SampleIdx;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int MaxSamples;
};
