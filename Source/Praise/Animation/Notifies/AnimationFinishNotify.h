// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "../../Enums/Animation/ECharAnimState.h"
#include "AnimationFinishNotify.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UAnimationFinishNotify : public UAnimNotifyState
{
	GENERATED_BODY()

	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECharAnimState AnimState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bSkipReset;
};
