// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "../../Enums/CommonUtility/EMiscSFX.h"
#include "Sound/SoundCue.h"
#include "PlayMiscSFXNotify.generated.h"


/**
 * 
 */
UCLASS()
class PRAISE_API UPlayMiscSFXNotify : public UAnimNotifyState
{
	GENERATED_BODY()

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EMiscSFX MiscFX;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int MaxSamples;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsRandomSample;


};
