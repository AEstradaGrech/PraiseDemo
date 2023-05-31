// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../../../Characters/BasePraiseCharacter.h"
#include "../../../Enums/CombatSystem/EArmedPoses.h"
#include "../../../Enums/CommonUtility/ECharVector.h"
#include "../../../Enums/Animation/ECharAnimState.h"
#include "Animation/AnimMontage.h"
#include "Animation/Blendspace1D.h"
#include "../../../Enums/Animation/EHolsterPosition.h"
#include "../../../Enums/CommonUtility/ECharVector.h"
#include "../../../Animation/PraiseAnimInterface.h"
#include "../../../Structs/DataTables/CombatSystem/FEquipAnimationsDataRow.h"
#include "PraiseAnimInstance.generated.h"


DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnCharStateUpdateSignature, ECharAnimState, CharAction, bool, bIsEnabled);
UCLASS()
class PRAISE_API UPraiseAnimInstance : public UAnimInstance, public IPraiseAnimInterface
{
	GENERATED_BODY()
	
public:
	UPraiseAnimInstance();

	FORCEINLINE TMap<ECharVector, FRotator> GetAimOffsets(bool bIsCrouched) const { return bIsCrouched ? CrouchedAimOffsets : AimOffsets; }
	virtual EArmedPoses GetCurrentPose() const override { return CurrentPose; }
	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable,Category = AnimProperties)
		virtual void UpdateAnimationProperties(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category = AnimProperties)
		ABasePraiseCharacter* GetAnimCharacter() const;


	void EnableRootMotion(bool bEnable, bool bMontagesOnly);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
	float EquipAnimLerpVal;
	FOnCharStateUpdateSignature OnStateUpdate;

protected:
	void SetDefaults();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ActionFlag)
		bool bIsBotCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		bool bIsDead;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		float MovementSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		bool bIsRunning;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		bool bIsAccelerating;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		bool bIsCrouching;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		bool bIsInAir;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		bool bIsJumping;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		bool bIsRolling;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		bool bIsEvading;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CombatSystem)
		bool bIsAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CombatSystem)
		bool bIsBlocking;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = ActionFlags)
		bool bIsParring;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		bool bIsEquiping;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		bool bShouldLinkEquipAnim;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = MovementFlags)
		bool bIsDualEquipAnim;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CombatSystem)
		bool bIsTargeting; 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CombatSystem)
		bool bIsAiming;	  
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		bool bIsFullBody;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CombatSystem)
		bool bIsWeaponEquiped;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = MovementFlags)
		bool bIsTurning;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = MovementFlags)
		int TurnDirection; 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ActionFlag)
		bool bIsPlayingAnimation; 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ActionFlag)
		bool bCanInterruptAnimation;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = MovementFlags)
		bool bIsWalking;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		EWeaponType CurrentWeaponType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		EWeaponClass CurrentWeaponClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		EWeaponClass SecondaryWeaponClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		EWeaponType SecondaryWeaponType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		EWeaponType PrevWeaponType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		EWeaponClass PrevWeaponClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		APawn* AnimPawn;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		ABasePraiseCharacter* AnimChar;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CombatSystem)
		ECharVector EvadeDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AnimProperties)
		float LeanMultiplier;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		float YawRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		float ScaledSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimProperties)
		float SpeedLerpMult;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		float RadsToDir;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		FRotator LastFrameRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CombatSystem)
		EArmedPoses CurrentPose;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		int EquippingHand;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		int32 AttackCounter;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		bool bIsCombo;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		bool bCanAttack;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		bool bIsLeftHandOnly;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		bool bIsReloading;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		bool bShouldPlayReloadEnd;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		EAttackType CurrentAttackType;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = AnimProperties)
		FRotator AimRotOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimProperties)
		TMap<ECharVector, FRotator> AimOffsets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimProperties)
		TMap<ECharVector, FRotator> CrouchedAimOffsets;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		bool bIsSwitchingWeapons;
private:
	
	ECharVector GetCurrentDirectionVector(float RotDegrees);
	void SetLeanStuff(float DeltaTime);

	UCurveFloat* GetCurve(FString PathToCurve);
	UCurveFloat* GetCurve(ECurveType Type);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TMap<EWeaponSlot, UCharWeaponSlot*> CharWeapons;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TMap<EWeaponSlot, UCharWeaponSlot*> CharPrevWeapons;
};
