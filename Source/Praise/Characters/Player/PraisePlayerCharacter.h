// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BasePraiseCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "../../Animation/PraiseAnimInterface.h"
#include "../../Structs/Characters/FPlayerStats.h"
#include "../../Interfaces/UI/MenuInterface.h"
#include "../../Interfaces/UI/PlayerHUDInterface.h"
#include "../../Components/Actor/WeaponsFactoryComponent.h"

#include "../../Praise.h"
#include "PraisePlayerCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FOnTriggerEquipWeaponsSignature, const EWeaponSlot);
DECLARE_DELEGATE_OneParam(FOnTriggerSlotWeaponSignature, const EWeaponSlot);
DECLARE_DELEGATE_OneParam(FOnDropSlotWeaponSignature, const EWeaponSlot);
DECLARE_DELEGATE_OneParam(FOnTriggerPickupItemSignature, const EWeaponSlot);

UCLASS()
class PRAISE_API APraisePlayerCharacter : public ABasePraiseCharacter
{
	GENERATED_BODY()
	
public:
	APraisePlayerCharacter();

	FORCEINLINE bool IsGameMenuEnabled() const { return bInGameMenuEnabled; }
	FORCEINLINE void SetGameMenuEnabled(bool bEnabled) { bInGameMenuEnabled = bEnabled; }

	USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	UCameraComponent* GetPlayerCamera() const { return PlayerCamera; }
	
	virtual void GetPlayerViewPoint(FVector& OutLocation, FRotator& OutRotation, bool bIsCameraView = true) const override;
	virtual void EnableHandCollider(bool bEnable, bool bIsRightHand) override;
	virtual void EnableKickCollider(bool bEnable, bool bIsRightLeg) override;
	bool IsTargetInCameraFOV(AActor* Target, bool bCheckCol);

	virtual void TriggerEvade() override;

	UFUNCTION()
		virtual void OnFistBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnKickBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnInteractionBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnInteractionEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
	virtual void HandleMessage(FTelegram& Msg) override;

private:

	float Sine(float Time, float Freq, float Amplitude = 1.f, float Phase = 0.f);

	virtual void SetCharacterDead(AActor* Killer) override;
	virtual void DestroyCharacter() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void TrySetHandColliders() override;
	virtual void OverrideCharStatsComp() override;

	ECharVector GetCharMovementVector();

	//----------------- CAMERA STUFF --------------------
	float GetClampedcameraOffset() const { return FMath::Clamp<float>(CameraOffset, MIN_CAMERA_OFFSET, MAX_CAMERA_OFFSET); }
	float GetCharToCameraDegrees() const;
	void HandleZoom(float DeltaTime, bool bZoomIn);
	void HandleRotateToView(float DeltaTime);
	void HandleAimWeaponPosition(float DeltaTime);
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = ActionFlags, meta = (AllowPrivateAccess = "true"))
		bool bIsZoomRequested;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* PlayerCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Camera, meta = (AllowPrivateAccess = "true", ClampMin = "-100", ClampMax = "100"))
		float CameraOffset;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Camera, meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "300"))
		float CameraZoom;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Camera, meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "300"))
		float CameraZoomSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		float CameraTargetFOV;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		FRotator MouseRot;
	// -------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = ActionFlags, meta = (AllowPrivateAccess = "true"))
		bool bIsHoldingPrimary;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = ActionFlags, meta = (AllowPrivateAccess = "true"))
		bool bIsHoldingSecondary;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = CharacterColliders, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* HandCollider_L;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = CharacterColliders, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* HandCollider_R;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = CharacterColliders, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* KickCollider_R;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = CharacterColliders, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* KickCollider_L;

	IMenuInterface* PlayerMenuInterface;
	IPlayerHUDInterface* PlayerHUDInterface;

	UPROPERTY()
	UWeaponsFactoryComponent* CharWeaponsFactory;
	

	UFUNCTION()
		void OnHoldedAttackHandler(EAttackType AttackType);
	UFUNCTION()
		void OnHandColliderHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& HitResult);

	UFUNCTION()
	void MoveForward(float Value);
	UFUNCTION()
	void MoveRight(float Value);
	UFUNCTION()
	void AddMouseVerticalInput(float Value);
	UFUNCTION()
		void AddMouseHorizontalInput(float Value);
	UFUNCTION()
		void TriggerEquipWeapons();
	UFUNCTION()
		void HandleInGameMenu();
	UFUNCTION()
		void EnsureReleaseBlock();
	bool bInGameMenuEnabled;
	virtual void TriggerWeaponSlot(EWeaponSlot Slot) override;
	virtual void DropWeaponSlot(EWeaponSlot Slot) override;
	
	UFUNCTION()
		void TrySkipTarget_L();
	UFUNCTION()
		void TrySkipTarget_R();

	virtual void OnAnimStateUpdate(ECharAnimState State, bool bIsEnabled) override;
	virtual void SetupCharWeaponSlots() override;

	virtual void StartJump() override;
	virtual void EndJump() override;
	virtual void HandleSpeedChange(float DeltaTime) override;
	virtual void StartCrouch() override;
	virtual void EndCrouch() override;
	

	EWeaponSlot PressedWeaponSlot;
	bool bIsPressingWeaponSlotKey;
	bool bIsPressingPickupKey;

	float SecondsToDropWeapon;
	float SecondsPressingWeaponKey;

	virtual void EquipWeapons(EWeaponSlot Slot = EWeaponSlot::INVALID, bool bTryDualEquip = true) override;
	
	virtual void TryLockTarget() override;
	void TrySkipTarget(bool bSkipLeft);
	virtual void StartPrimaryAttack() override;
	virtual void EndPrimaryAttack() override;
	virtual void StartSecondaryAttack() override;
	virtual void EndSecondaryAttack() override;
	virtual void StartBlocking() override;
	virtual void EndBlocking() override;
	virtual void StartParry() override;
	virtual void EndParry() override;
	
	//////// DEV ONLY ////////
	FPlayerStats GetDefaultPlayerStats();

	UFUNCTION()
		void TriggerTestKey();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess="true"))
		float TestDamage;
};
