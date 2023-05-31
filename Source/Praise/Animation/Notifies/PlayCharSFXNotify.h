// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "../../Enums/Characters/ECharSFX.h"
#include "PlayCharSFXNotify.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UPlayCharSFXNotify : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECharSFX CharFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsRandomSample;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int SampleIdx;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int MaxSamples;
private:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	
};
