// Fill out your copyright notice in the Description page of Project Settings.


#include "PraiseAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../../Praise.h"
#include "Engine/DataTable.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "../../../Weapons/Firearm.h"
#include "../../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include <exception>
#include "GameFramework/CharacterMovementComponent.h"

UPraiseAnimInstance::UPraiseAnimInstance() 
{
	SetDefaults();
}

void UPraiseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SetDefaults();

	APawn* PawnOwner = TryGetPawnOwner();

	if (PawnOwner) 
	{
		AnimPawn = PawnOwner;

		if (AnimPawn->IsA<ABasePraiseCharacter>())
			AnimChar = Cast<ABasePraiseCharacter>(AnimPawn);
	}

}

void UPraiseAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (!AnimPawn)
		AnimPawn = TryGetPawnOwner();

	if (AnimPawn) 
	{
		bIsJumping = AnimPawn->GetMovementComponent()->IsFalling();

		FVector PawnVelocity = AnimPawn->GetMovementComponent()->Velocity;
		
		MovementSpeed = FVector(PawnVelocity.X, PawnVelocity.Y, 0.f).Size();
		
		if (!AnimChar)
			AnimChar = Cast<ABasePraiseCharacter>(AnimPawn);

		if (AnimChar) 
		{
			if (!AnimChar->GetCharStats()) return;
			
		
			bIsPlayingAnimation = AnimChar->IsPlayingAnimation();
			bCanInterruptAnimation = AnimChar->CanInterruptAnimation();

			bIsDead = AnimChar->IsDead();
			bIsAccelerating = AnimChar->GetCharAcceleration() > 0; 
			bIsInAir = AnimChar->IsInAir();
			bIsTargeting = AnimChar->IsTargeting();
	
			bIsAttacking = AnimChar->IsAttacking();
			bIsBlocking = AnimChar->IsBlocking();
			bIsParring = AnimChar->IsParring();
			bIsEquiping = AnimChar->IsEquiping();
			bIsDualEquipAnim = AnimChar->IsDualEquipAnim();
			bShouldLinkEquipAnim = AnimChar->ShouldLinkEquipAnims();
			bIsJumping = AnimChar->IsJumping();
			bIsRunning = AnimChar->IsRunning();
			bIsCrouching = AnimChar->IsCrouching();
			bIsRolling = AnimChar->IsRolling();
			
			bIsAiming = AnimChar->IsAiming();
			bIsWeaponEquiped = AnimChar->IsWeaponEquiped();
			CurrentPose = AnimChar->GetCurrentPose();
			AttackCounter = AnimChar->GetAttackCounter();
			bIsCombo = AnimChar->IsComboAttack();
			bIsSwitchingWeapons = AnimChar->IsSwitchingWeapons();
			bCanAttack = AnimChar->CanAttack();
			CurrentAttackType = AnimChar->GetCurrentAttackType();

			CharWeapons = AnimChar->GetCharCurrentWeapons();
			CharPrevWeapons = AnimChar->GetCharPreviousWeapons();

			CurrentWeaponType = AnimChar->GetCharCurrentWeapons().Contains(EWeaponSlot::PRIMARY_WEAPON) && !AnimChar->GetCharCurrentWeapons()[EWeaponSlot::PRIMARY_WEAPON]->IsEmpty() ?
								AnimChar->GetCharCurrentWeapons()[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->GetWeaponType() : EWeaponType::NONE;

			CurrentWeaponClass = AnimChar->GetCharCurrentWeapons().Contains(EWeaponSlot::PRIMARY_WEAPON) && !AnimChar->GetCharCurrentWeapons()[EWeaponSlot::PRIMARY_WEAPON]->IsEmpty() ?
								 AnimChar->GetCharCurrentWeapons()[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->GetWeaponClass() : EWeaponClass::NONE;

			PrevWeaponType = AnimChar->GetCharPreviousWeapons().Contains(EWeaponSlot::PRIMARY_WEAPON) && !AnimChar->GetCharPreviousWeapons()[EWeaponSlot::PRIMARY_WEAPON]->IsEmpty() ?
							 AnimChar->GetCharPreviousWeapons()[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->GetWeaponType() : EWeaponType::NONE;

			PrevWeaponClass = AnimChar->GetCharPreviousWeapons().Contains(EWeaponSlot::PRIMARY_WEAPON) && !AnimChar->GetCharPreviousWeapons()[EWeaponSlot::PRIMARY_WEAPON]->IsEmpty() ?
							  AnimChar->GetCharPreviousWeapons()[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->GetWeaponClass() : EWeaponClass::NONE;

			SecondaryWeaponClass = AnimChar->GetCharCurrentWeapons().Contains(EWeaponSlot::SECONDARY_WEAPON) && !AnimChar->GetCharCurrentWeapons()[EWeaponSlot::SECONDARY_WEAPON]->IsEmpty() ? 
								   AnimChar->GetCharCurrentWeapons()[EWeaponSlot::SECONDARY_WEAPON]->GetSlotWeapon()->GetWeaponClass() : EWeaponClass::NONE;

			SecondaryWeaponType = AnimChar->GetCharCurrentWeapons().Contains(EWeaponSlot::SECONDARY_WEAPON) && !AnimChar->GetCharCurrentWeapons()[EWeaponSlot::SECONDARY_WEAPON]->IsEmpty() ?
								  AnimChar->GetCharCurrentWeapons()[EWeaponSlot::SECONDARY_WEAPON]->GetSlotWeapon()->GetWeaponType() : EWeaponType::NONE;

			bIsLeftHandOnly = CurrentWeaponClass == EWeaponClass::NONE && SecondaryWeaponClass != EWeaponClass::NONE;

			bIsTurning = AnimChar->IsTurning();
			TurnDirection = AnimChar->GetTurnDirection();
			EquippingHand = AnimChar->GetEquippingHandIdx();

			bIsReloading = AnimChar->IsReloading();

			if (AnimChar->IsWeaponEquiped())
			{
				UCharWeaponSlot* MainSlot = AnimChar->GetCurrentMainWeapon();

				if (MainSlot && MainSlot->HoldsA<AFirearm>() && bIsReloading)
				{
					AFirearm* Firearm = Cast<AFirearm>(MainSlot->GetSlotWeapon());

					bShouldPlayReloadEnd = Firearm->GetAmmo() == Firearm->GetMagAmmo();
				}

				else bShouldPlayReloadEnd = false;
			}
		}

		SetLeanStuff(DeltaTime);
		bIsBotCharacter = AnimPawn->IsA<ABaseBotCharacter>();
	
		float TargetSpeed = AnimChar->GetVelocity().GetSafeNormal().Size() * (AnimChar->GetCharacterMovement()->MaxWalkSpeed / AnimChar->GetCharStats()->GetDefaultWalkSpeed());
		float Speed = FMath::FInterpTo(ScaledSpeed, TargetSpeed, DeltaTime, SpeedLerpMult);
		
		ScaledSpeed = !bIsAccelerating && Speed <= 0.2f ? 0.f : Speed;
	
		float Dir = AnimChar->GetDirectionDegrees();

		if (RadsToDir > 160)
		{
			if (Dir <= -170)
			{
				Dir *= -1;
			}
		}

		RadsToDir = Dir;

		AimRotOffset = AnimChar->GetAimOffset();

		bIsWalking = bIsBotCharacter ? Cast<ABaseBotCharacter>(AnimChar)->ShouldMove() : MovementSpeed > 0;
	}
}

ABasePraiseCharacter* UPraiseAnimInstance::GetAnimCharacter() const
{ 
	return AnimChar ? AnimChar : AnimPawn ? Cast<ABasePraiseCharacter>(AnimPawn) : Cast<ABasePraiseCharacter>(TryGetPawnOwner());
}


void UPraiseAnimInstance::SetDefaults()
{
	MovementSpeed = 0.f;
	LeanMultiplier = 3.f;
	SpeedLerpMult = 20.f;
	CurrentPose = EArmedPoses::UNARMED;
	bIsWeaponEquiped = false;
	
	bIsDead = false;
	bIsAccelerating = false;
	bIsJumping = false;
	bIsInAir = false;
	bIsCrouching = false;
	bIsAttacking = false;
	bIsBlocking = false;
	bIsRolling = false;
	bIsEvading = false;
	bIsTargeting = false;
	bIsAiming = false;
	bIsEquiping = false;
	
	bIsFullBody = true;
	bIsPlayingAnimation = false;
	bCanInterruptAnimation = false;

}


ECharVector UPraiseAnimInstance::GetCurrentDirectionVector(float RotDegrees)
{
	if (FMath::Abs(RotDegrees) < 15)
		return ECharVector::FWD;
	if (FMath::Abs(RotDegrees) >= 160)
		return ECharVector::BWD;
	if (FMath::Abs(RotDegrees) >= 15 && FMath::Abs(RotDegrees) < 80)
		return FMath::Sign(RotDegrees) < 0 ? ECharVector::FWD_L : ECharVector::FWD_R;
	if (FMath::Abs(RotDegrees) >= 80 && FMath::Abs(RotDegrees) < 110)
		return FMath::Sign(RotDegrees) < 0 ? ECharVector::LEFT : ECharVector::RIGHT;
	if (FMath::Abs(RotDegrees) >= 110 && FMath::Abs(RotDegrees) < 160)
		return FMath::Sign(RotDegrees) < 0 ? ECharVector::BWD_L : ECharVector::BWD_R;
	
	return ECharVector::FWD;
}

void UPraiseAnimInstance::SetLeanStuff(float DeltaTime)
{
	if (!AnimPawn) return;

	FRotator CurrentRot = AnimPawn->GetActorRotation();

	FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(LastFrameRotation, CurrentRot);

	float TargetRot = Delta.Yaw / DeltaTime;

	float Interp = FMath::FInterpTo(YawRotation, TargetRot, DeltaTime, LeanMultiplier);

	YawRotation = FMath::Clamp(Interp, -90.f, 90.f);

	LastFrameRotation = CurrentRot;

}

UCurveFloat* UPraiseAnimInstance::GetCurve(FString PathToCurve)
{
	UCurveFloat* Curve = Cast<UCurveFloat>(StaticLoadObject(UCurveFloat::StaticClass(), NULL, *PathToCurve));

	return Curve ? Curve : Cast<UCurveFloat>(StaticLoadObject(UCurveFloat::StaticClass(), NULL, *CURVE_PATH_LINEAR));
}

UCurveFloat* UPraiseAnimInstance::GetCurve(ECurveType Type)
{
	FString Path;
	switch (Type) {
	case(ECurveType::LOG):
		Path = CURVE_PATH_LOG;
		break;
	case(ECurveType::ANTILOG):
		Path = CURVE_PATH_ANTILOG;
		break;
	case(ECurveType::S_LOG):
		Path = CURVE_PATH_S_LOG;
		break;
	case(ECurveType::S_ANTILOG):
		Path = CURVE_PATH_S_ANTILOG;
		break;
	case(ECurveType::LINEAR):
	default:
		Path = CURVE_PATH_LINEAR;
		break;
	}

	return GetCurve(Path);
}

void UPraiseAnimInstance::EnableRootMotion(bool bEnable, bool bMontagesOnly)
{
	ERootMotionMode::Type Mode = !bEnable ? ERootMotionMode::IgnoreRootMotion :
		bMontagesOnly ? ERootMotionMode::RootMotionFromMontagesOnly : ERootMotionMode::RootMotionFromEverything;

	SetRootMotionMode(bEnable ? ERootMotionMode::RootMotionFromEverything : ERootMotionMode::IgnoreRootMotion);
}
