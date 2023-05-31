// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "../../Enums/Animation/EHolsterPosition.h"
#include "../../Enums/CombatSystem/EWeaponSlot.h"
#include "EquipWeaponNotify.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UEquipWeaponNotify : public UAnimNotifyState
{
	GENERATED_BODY()
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HolsterPosition)
		EHolsterPosition HolsterPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HolsterPosition)
		EWeaponSlot CharWeapon;
	
};
