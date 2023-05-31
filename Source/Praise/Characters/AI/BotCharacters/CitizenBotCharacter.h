// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBotCharacter.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "CitizenBotCharacter.generated.h"


UCLASS()
class PRAISE_API ACitizenBotCharacter : public ABaseBotCharacter
{
	GENERATED_BODY()
	
public:
	ACitizenBotCharacter();


	virtual bool TryInitBrain() override;
	virtual void EnableHandCollider(bool bEnable, bool bIsRightHand) override;
	virtual void EnableKickCollider(bool bEnable, bool bIsRightLeg) override;
protected:
	virtual void TrySetHandColliders() override;
	
	UFUNCTION()
	void OnHandColliderHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& HitResult);
	UFUNCTION()
		virtual void OnBotKickBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnBotFistBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	

	virtual void SetupCharWeaponSlots() override;
	
	virtual void StartPrimaryAttack() override;
	virtual void EndPrimaryAttack() override;
	virtual void StartSecondaryAttack() override;
	virtual void EndSecondaryAttack() override;
	virtual void StartBlocking() override;
	virtual void EndBlocking() override;
	virtual void StartParry() override;
	virtual void EndParry() override;
	virtual void TryLockTarget() override;
	virtual void EquipWeapons(EWeaponSlot Hand = EWeaponSlot::INVALID, bool bTryDualEquip = true) override;
	virtual void SetCharacterDead(AActor* Killer) override;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = CharacterColliders)
		UBoxComponent* HandCollider_L;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = CharacterColliders)
		UBoxComponent* HandCollider_R;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = CharacterColliders, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* KickCollider_R;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = CharacterColliders, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* KickCollider_L;
};
