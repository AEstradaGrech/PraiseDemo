// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../../../Characters/AI/BotCharacters/BasicZombieCharacter.h"
#include "../../../Enums/CombatSystem/EAttackType.h"
#include "ZombieAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UZombieAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category = AnimProperties)
		virtual void UpdateAnimationProperties(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category = AnimProperties)
		ABasicZombieCharacter* GetAnimCharacter();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		bool bIsDead;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		float MovementSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		bool bIsRunning;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CombatSystem)
		bool bIsAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Locomotion)
		bool bIsInAir;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		APawn* AnimPawn;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		ABasicZombieCharacter* AnimChar;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		float ScaledSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		float RadsToDir;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimProperties)
		int EquippingHand;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		int32 AttackCounter;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		bool bIsCombo;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		bool bCanAttack;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		bool bIsRoaring;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimProperties)
		float SpeedLerpMult;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		EAttackType CurrentAttackType;
};
