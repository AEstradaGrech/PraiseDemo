// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBotCharacter.h"
#include "BasicZombieCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API ABasicZombieCharacter : public ABaseBotCharacter
{
	GENERATED_BODY()
	
public:
	ABasicZombieCharacter();

	FORCEINLINE bool IsRoaring() const { return bIsRoaring; }

	virtual void BeginPlay() override;
	virtual void OnAnimStateUpdate(ECharAnimState State, bool bIsEnabled) override;
	void TriggerRoar();
	virtual void StartPrimaryAttack() override;
	virtual void StartSecondaryAttack() override;
protected:
	virtual void SetupCharWeaponSlots() override;
	virtual void EquipWeapons(EWeaponSlot Hand = EWeaponSlot::INVALID, bool bTryDualEquip = false) override;
	virtual void SetCharacterDead(AActor* Killer) override;
	virtual void TrySetHandColliders() override;
	virtual void EnableHandCollider(bool bEnable, bool bIsRightHand) override;
	virtual class AFist* SpawnDefaultUnarmedWeapon();
	UFUNCTION()
		virtual void OnBotFistBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		bool bIsRoaring;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = CharacterColliders)
		UBoxComponent* HandCollider_L;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = CharacterColliders)
		UBoxComponent* HandCollider_R;

	UPROPERTY()
		class UWeaponsFactoryComponent* CharWeaponsFactory;
};
