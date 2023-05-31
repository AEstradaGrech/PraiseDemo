// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieAnimInstance.h"


void UZombieAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	GetAnimCharacter();
}

ABasicZombieCharacter* UZombieAnimInstance::GetAnimCharacter()
{
	if (AnimChar) return AnimChar;

	if (!AnimPawn)
		AnimPawn = TryGetPawnOwner();

	if (AnimPawn && AnimPawn->IsA<ABasicZombieCharacter>())
		AnimChar = Cast<ABasicZombieCharacter>(AnimPawn);

	return AnimChar;
}

void UZombieAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (AnimChar && AnimChar->GetCharStats())
	{
		float TargetSpeed = AnimChar->GetVelocity().GetSafeNormal().Size() * (AnimChar->GetCharacterMovement()->MaxWalkSpeed / AnimChar->GetCharStats()->GetDefaultWalkSpeed());
		float Speed = FMath::FInterpTo(ScaledSpeed, TargetSpeed, DeltaTime, SpeedLerpMult);

		ScaledSpeed = Speed;

		MovementSpeed = AnimChar->GetCharacterMovement()->MaxWalkSpeed;
		RadsToDir = CalculateDirection(AnimChar->GetVelocity(), AnimChar->GetActorRotation());
		bIsDead = AnimChar->IsDead();
		bIsAttacking = AnimChar->IsAttacking();
		bIsRunning = AnimChar->IsRunning();
		EquippingHand = AnimChar->GetEquippingHandIdx();
		AttackCounter = AnimChar->GetAttackCounter();
		bIsCombo = AnimChar->IsComboAttack();
		bCanAttack = AnimChar->CanAttack();
		bIsInAir = AnimChar->IsInAir();
		bIsRoaring = AnimChar->IsRoaring();
		CurrentAttackType = AnimChar->GetCurrentAttackType();
	}
}


