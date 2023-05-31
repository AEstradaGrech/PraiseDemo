// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "../../Enums/CombatSystem/EWeaponSlot.h"
#include "../../Enums/Animation/ECharAnimState.h"
#include "SetWeaponPositionNotify.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API USetWeaponPositionNotify : public UAnimNotifyState
{
	GENERATED_BODY()
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EWeaponSlot WeaponHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECharAnimState AnimAction;

};
