// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "../../Enums/CombatSystem/EWeaponSlot.h"
#include "../../Enums/CombatSystem/EAttackType.h"
#include "MeleeWeaponAttackNotify.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UMeleeWeaponAttackNotify : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UMeleeWeaponAttackNotify();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		EWeaponSlot EnabledWeaponHand = EWeaponSlot::INVALID;
	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
		EAttackType AttackType = EAttackType::NONE;
};
