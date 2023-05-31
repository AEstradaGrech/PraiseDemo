// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../Enums/CombatSystem/EWeaponSlot.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ReloadMagNotify.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UReloadMagNotify : public UAnimNotifyState
{
	GENERATED_BODY()
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EWeaponSlot CharWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int ReloadedBullets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsMagOverwrite;
};
