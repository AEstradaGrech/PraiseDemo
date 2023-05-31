// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "MeleeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API AMeleeWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	AMeleeWeapon();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
		class USoundCue* DrawWeaponSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
		class USoundCue* UnequipWeaponSound;

private:
	virtual bool TrySetupWeaponColliders(FWeaponArtDTStruct* ArtData = nullptr) override;
	
	virtual void Attack(AActor* Target) override;

};
