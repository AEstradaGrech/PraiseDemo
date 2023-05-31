// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "../../Enums/CombatSystem/EWeaponSlot.h"
#include "RotateMeshNotify.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API URotateMeshNotify : public UAnimNotifyState
{
	GENERATED_BODY()
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		float Degrees;

	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		FRotator NewRotation;
	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		EWeaponSlot WeaponHand;

	FRotator OriginalRot;
};
