// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Components/Actor/CharStats/CharStatsComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Animation/Blendspace1D.h"
#include "../Enums/CommonUtility/ECurveType.h"
#include "../Enums/CommonUtility/EMiscSFX.h"
#include "../Enums/Animation/ECharAnimState.h"
#include "../Enums/Characters/ECharStats.h"
#include "../Enums/Characters/ECharSFX.h"
#include "../Enums/Characters/ECharType.h"
#include "../Enums/Characters/ECharAction.h"
#include "../Enums/CombatSystem/EArmorType.h"
#include "../Enums/CombatSystem/EWeaponSlot.h"
#include "../Enums/CombatSystem/EAttackType.h"
#include "../Enums/CombatSystem/ETargetType.h"
#include "../Enums/CombatSystem/ECombatSFX.h"
#include "../Enums/Characters/ECharFaction.h"
#include "../Enums/Characters/ECharStatus.h"
#include "../Enums/AI/EAI_MsgChannel.h"
#include "../Enums/CommonUtility/ECharvector.h"
#include "../Structs/CommonUtility/FLogger.h"
#include "Curves/CurveFloat.h"
#include "Components/TimelineComponent.h"
#include "../Components/Actor/TargetingComponent.h"
#include "../Inventory/CharWeaponSlot.h"
#include "../Interfaces/CombatSystem/Damageable.h"
#include "../Interfaces/CombatSystem/Targeteable.h"
#include "../Interfaces/CombatSystem/Combatible.h"
#include "../Interfaces/AI/Factioneable.h"
#include "../Interfaces/UI/CharStatsInterface.h"
#include "../UI/CharStatsBarWidget.h"
#include "../Structs/CommonUtility/FUtilities.h"
#include "../Structs/AI/FTelegram.h"
#include "../AI/MsgCommands/BaseMsgCommand.h"
#include "Components/WidgetComponent.h"
#include "../Structs/AI/FTelegram.h"
#include "../AI/CommonUtility/Factions/FactionZone.h"
#include "../Components/Actor/ArmorsFactoryComponent.h"
#include "BasePraiseCharacter.generated.h"


DECLARE_DELEGATE_TwoParams(FOnWalkSpeedChangeSignature, float, bool);
//DECLARE_DYNAMIC_DELEGATE_FourParams(FOnSendMsgSignature, EAI_MsgChannel, Channel, FTelegram&, Msg, int, RecieverId, float, Delay);
//DECLARE_DELEGATE_OneParam(FOnApplyBuffSignature, ABaseCharBuff*);
//DECLARE_DELEGATE_OneParam(FonRemoveBuffSignature, ABaseCharBuff*);
DECLARE_DELEGATE_TwoParams(FOnModifyStatsSignature, ECharStats, float);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnNewFoeSignature, AActor*, FoE, bool, bIsEnemy);
DECLARE_DELEGATE(FOnRestoreWalkSpeedSignature);
DECLARE_DELEGATE_OneParam(FOnAttackEndDelaySignature, EAttackType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNotifyCharDeadSignature, AActor*, Self, AActor*, Killer);
UCLASS()
class PRAISE_API ABasePraiseCharacter : public ACharacter, public IDamageable, public ITargeteable, public ICombatible, public ICharStatsInterface, public IFactioneable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABasePraiseCharacter();
	

	//GETTERS
	FORCEINLINE class UAudioComponent* GetCharAudioComp() { return AudioComp; }
	FORCEINLINE FString GetCharBPName() const { return CharBPName; }
	FORCEINLINE FVector GetSpawnLocation() const { return CharSpawnLocation; }
	FORCEINLINE int GetEquippingHandIdx() const { return EquippingHand; }
	FORCEINLINE bool IsEquippingWeapon() const { return bIsEquippingWeapon; }
	FORCEINLINE bool IsReloading() const { return bIsReloading; }
	FORCEINLINE bool IsSwitchingWeapons() const { return bIsSwitchingWeapons; }
	FORCEINLINE FRotator GetAimOffset() const { return AimRotOffset; }
	virtual TArray<AActor*> GetPersonalEnemies() const override { return PersonalEnemies; }
	virtual TArray<AActor*> GetPersonalFriends() const override { return PersonalFriends; }
	FORCEINLINE UCharStatsComponent* GetCharStats() const { return CharStats; }
	class UPraiseAnimInstance* GetPraiseAnimInstance() const;
	class UMsgCommandsFactoryComponent* GetMsgsFactory() const { return CharFaction && CharFaction->GetMsgComandsFactory() ? CharFaction->GetMsgComandsFactory() : nullptr; };
	//IFactioneable
	virtual ECharFaction FactionID() const override { return CharFaction ? CharFaction->FactionID() : ECharFaction::NONE; }
	virtual AGameFaction* GetCharFaction() const override { return CharFaction; }
	virtual void SetFaction(AGameFaction* NewFaction) override { CharFaction = NewFaction; }
	virtual bool IsFriendTarget(IFactioneable* Target) const override;
	virtual bool IsEnemyTarget(IFactioneable* Target) const override;
	virtual TArray<ECharFaction> GetIgnoredFactions() const override;
	virtual void AddPersonalFoE(AActor* FoE, bool bIsFriend) override;
	//ITargeteable 
	virtual ETargetType	IsTargetType() const override;
	virtual int32 GetTargetID() const override;
	virtual bool IsDead() const override;
	virtual ECharStatus GetCharStatus() const override;
	
	//IDamageable	
	virtual void GetDamage(float Damage, AActor* Damager, EAttackType AttackType = EAttackType::PRIMARY_ATTACK) override;
	virtual void GetWeaponDamage(float Damage, EAttackType AttackType, EWeaponSlot AttackerHand, ABasePraiseCharacter* Damager, AWeapon* DamagerWeapon) override;
	virtual void EnableStatsBar(bool bEnable) override;
	virtual bool HasBeenRecentlyDamaged() override { return false; };
	
	//ICombatible  
	virtual float GetHealth() const override;
	virtual float GetStamina() const override;
	virtual bool HasWeapon() const override;					 
	virtual bool IsWeaponEquiped() const override;		 
	virtual bool IsEquiping() const override;					 
	virtual bool IsAttacking() const override;
	virtual bool CanAttack() const override;
	virtual bool IsBlocking() const override;
	virtual bool IsBeingDamaged() const override;
	virtual bool IsParring() const override;
	virtual bool IsAiming() const override;
	virtual bool IsTargeting() const override;
	virtual bool IsRolling() const override;
	virtual bool IsEvading() const override;
	virtual bool IsJumping() const override;						 
	virtual bool IsRunning() const override;
	virtual bool IsCrouching() const override;
	virtual EArmedPoses GetCurrentPose() const override;
	virtual EAttackType GetCurrentAttackType() const override;
	virtual TMap<EWeaponSlot, UCharWeaponSlot*> GetCharCurrentWeapons() const override;
	virtual bool IsInMeleeRange(AActor* Opponent, float MaxDistanceOffset = 0.f) const override;
	virtual bool IsInCombatRange(AActor* Opponent, float MaxDistanceOffset) const override;
	virtual ECharVector GetTargetLocationVector(AActor* Target) const override;
	void DisableTargeting();

	// ---------------- GETTERS
	FORCEINLINE float GetMaxCombatRange(bool bWithWeaponDistanceOffset = true) { return bWithWeaponDistanceOffset ? MaxCombatRange + GetMeleeDistanceOffset() : MaxCombatRange; }
	FORCEINLINE float GetMaxMeleeDistance() { return 100 + GetMeleeDistanceOffset(); }
	FORCEINLINE float GetMinMeleeDistance() { return MinCombatRange; }
	FORCEINLINE bool IsDualEquipAnim() const { return bIsDualEquipAnim; }
	FORCEINLINE bool ShouldLinkEquipAnims() const { return bShouldLinkEquipAnims; }
	FORCEINLINE bool IsInAir() const { return GetMovementComponent() ? GetMovementComponent()->IsFalling() : false; } 
	FORCEINLINE bool CanMove() const { return bCanMove; }
	FORCEINLINE bool CanInterruptAnimation() const { return bCanInterruptAnimation; }
	FORCEINLINE bool IsPlayingAnimation() const { return bIsPlayingAnimation; }
	FORCEINLINE ECharVector GetEvadeDirection() const { return EvadeDirection; }
	FORCEINLINE TMap<EWeaponSlot, UCharWeaponSlot*> GetCharWeaponSlots() const { return CharWeaponSlots; } 
	FORCEINLINE TMap<EWeaponSlot, UCharWeaponSlot*> GetCharPreviousWeapons() const { return PreviousWeapons; } 
	UCharWeaponSlot* GetCurrentMainWeapon() const;
	UCharWeaponSlot* GetCurrentSecondaryWeapon() const;
	FORCEINLINE AWeapon* GetCurrentAttackWeapon() const { return CurrentAttackWeapon; }
	FORCEINLINE virtual float GetHealthRatio() const { return CharStats ? CharStats->GetHealthRatio() : 0.f; }
	FORCEINLINE virtual float GetStaminaRatio() const { return CharStats ? CharStats->GetStaminaRatio() : 0.f; }
	FORCEINLINE int GetNeutralCombatHitsTolerance() const { return NeutralCombatHitsTolerance >= 0 ? NeutralCombatHitsTolerance : 2; }
	FORCEINLINE float GetCharSpeed() const { return GetMovementComponent()->Velocity.Size(); }
	FORCEINLINE float GetCharAcceleration() const { return GetCharacterMovement()->GetCurrentAcceleration().Size(); }
	
	FORCEINLINE int32 GetAttackCounter() const { return AttackCounter; }
	FORCEINLINE bool IsComboAttack() const { return bIsCombo; }
	FORCEINLINE bool IsTurning() const { return bIsTurning; }
	FORCEINLINE int32 GetTurnDirection() const { return TurnDirection; }
	FORCEINLINE float GetFieldOfView() const { return CharStats ? CharStats->GetFieldOfView() : 70.f; }
	FORCEINLINE UTargetingComponent* GetTargetingComponent() const { return TargetingComponent; }
	FORCEINLINE AActor* GetLockedTarget() const { return TargetingComponent ? TargetingComponent->GetCurrentTarget() : nullptr; }
	FORCEINLINE bool CanChangeDefaultSkin() const { return bCanChangeDefaultSkin; }
	virtual float GetMeleeDistanceOffset() const override;

	FORCEINLINE AFactionZone* GetCharSpawnZone() const { return CharSpawnZone; }
	FORCEINLINE bool HasBuildingAssigned() const { return bHasBuilding; }
	FORCEINLINE class ABaseBuilding* GetCharHome() const { return CharHome; }
	//SETTERS 
	FORCEINLINE void SetCanMove(bool bValue) { bCanMove = bValue; }
	FORCEINLINE void SetCanRequestCombo(bool bValue, bool bInterruptAttack = false) { bCanRequestCombo = bValue; bCanInterruptAnimation = true; };
	FORCEINLINE void SetShouldLinkEquipAnim(bool bValue) { bShouldLinkEquipAnim = bValue; }
	FORCEINLINE void SetCharSpawnZone(AFactionZone* Zone) { CharSpawnZone = Zone; }
	FORCEINLINE void SetTurn(bool bEnable, int TurnDir = 0) { bIsTurning = bEnable; TurnDirection = TurnDir; }

	void SetCharHome(class ABaseBuilding* Building);
	bool CanEnterBuilding(class ABaseBuilding* Building);

	void RotateTo(float DeltaTime, FVector TargetPosition);

	void RotateTo(AActor* Actor, float RotSpeed = 1.f);
	void RotateTo(FVector TargetPosition, float RotSpeed = 1.f);
	
	float RotationSpeed = 1.f;
	FVector RotationTarget;
	AActor* TargetActor;

	bool bComboRequested;
	virtual void EnableHandCollider(bool bEnable, bool bIsRightHand);
	virtual void EnableKickCollider(bool bEnable, bool bIsRightLeg);
	void EnableWeaponCollider(EWeaponSlot WeaponHand, EAttackType AttackType, bool bEnable);
	virtual void EquipWeapon(EHolsterPosition HolsterPos);
	void AttachWeaponToSocket(AWeapon* Weapon, FName SocketName, FVector WeaponLoc, FRotator WeaponRot, bool bPlaySFX = true);
	void HandleWeaponSlotEquip(UCharWeaponSlot* Slot, bool bIsEquip, FName WeaponSocket = FName());
	virtual void ResetComboStuff();
	float GetDirectionDegrees();
	ECharVector GetDirectionVector(float DirDegrees);

	FOnNewFoeSignature OnNewFoE;

	UFUNCTION()
		void UpdateCharStatus(ECharStatus NewStatus);
	UFUNCTION()
		virtual void HandleCharDeadNotify(AActor* Victim, AActor* Killer);
	UFUNCTION(BlueprintCallable)
		void RestoreWalkSpeed(bool bSetTimeline = false, ECurveType CurveType = ECurveType::LINEAR);
	UFUNCTION()
		virtual void OnAnimStateUpdate(ECharAnimState State, bool bIsEnabled);

	void ResetEquipingStuff();
	void RotateWeapon(EWeaponSlot WeaponHand, FRotator DesiredRot);
	void EnableTargetWidget(bool bEnable);
	void PlayCombatSFX(ECombatSFX FX);
	void PlayCombatSFX(ECombatSFX FX, FName SubParamName, int SubParamValue);
	void PlayMiscSFX(EMiscSFX FX);
	void PlayMiscSFX(EMiscSFX FX, FName SubParamName, int SubParamValue);
	void PlayCharSFX(ECharSFX FX, FName SubParamName, int SubParamValue);
	void PlayCharSFX(ECharSFX FX);
	void PlayFootstepSFX();

	UFUNCTION()
		void HandleWalkSpeed(float Value, bool bIsMultiplier, bool bSetTimeline = true, ECurveType CurveType = ECurveType::LINEAR, float SpeedTLPlayRate = 1.f);
	UFUNCTION()
		virtual void HandleMessage(FTelegram& Msg);

	void SendMessage(EAI_MsgChannel Channel, FTelegram& Msg);
	void PingMessage(class UBaseMsgCommand* MsgCommand);
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = AI)
		AFactionZone* CharSpawnZone;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = ActionFlags)
		bool bHandleWeaponRot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatSystem)
		int NeutralCombatHitsTolerance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		ECharType CharType;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		FRotator AimRotOffset;

	FRotator WeaponTargetRotation;
	FRotator CurrentWeaponRot;
	EWeaponSlot WeaponToRotate;
	
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void OverrideCharStatsComp();
	virtual void HandleSpeedChange(float DeltaTime) {};
	void HandleWeaponRotation(float DeltaTime);
	UFUNCTION()
	void ResetMainWeaponEquipPosition();
	FTimerHandle ResetEquipPositionTimer;
	UFUNCTION()
		virtual void DestroyCharacter();
	UFUNCTION()
		virtual void SetCharacterDead(AActor* Killer);
	
	UFUNCTION(Server, Reliable)
		virtual void Server_HandleCharDeadNotify(AActor* Victim, AActor* Killer);
	UFUNCTION(NetMulticast, Reliable)
		virtual void NetCast_HandleCharDeadNotify(AActor* Victim, AActor* Killer);

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CharAnimations)
		UAnimMontage* HitAnimsMontage;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CharAnimations)
		UAnimMontage* EvadeAnimsMontage;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CharAnimations)
		UAnimMontage* PickupAnimsMontage;

	AActor* CurrentlyPickedupItem;
	void PlayHitAnimation(AActor* Damager);
	void PlayEvadeAnimation(ECharVector Direction);
	void PlayPickupAnimation(AActor* PickedItems, EWeaponSlot RequestedSlot = EWeaponSlot::INVALID, bool bPickFromGround = true);
	void HandlePickupWeapon(AWeapon* Weapon, EWeaponSlot RequestedSlot = EWeaponSlot::INVALID, bool bPickFromGround = true);
	void PickWeaponForSlot(AWeapon* Weapon, EWeaponSlot RequestedSlot = EWeaponSlot::INVALID, bool bPickFromGround = true, bool bForceHandUpdate = false);
	void UpdateWeaponSlot(AWeapon* Weapon, EWeaponSlot RequestedSlot, bool bPickFromGround = true);
	void HandleSprint();
	void HandleRotateTo(float DeltaTime);
	UBoxComponent* InitHandCollider(FString ColName, FName SocketName, FVector Size = FVector(0.25f, 0.25f, 0.25f));

	virtual void TrySetHandColliders();

	UFUNCTION()
		virtual void OnInteractionBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnInteractionEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
		void HandleBlendspaceTL(float DeltaTime);
	UFUNCTION()
		virtual void OnBlendSpaceTLFinished();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString CharBPName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector CharSpawnLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		TArray<AActor*> PersonalEnemies;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		TArray<AActor*> PersonalFriends;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		TArray<ECharFaction> IgnoredFactions;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		AGameFaction* CharFaction;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		ETargetType TargetType;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		EArmedPoses CurrentPose;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		ECharStatus CharStatus;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterColliders);
		USphereComponent* InteractionCollider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UTargetingComponent* TargetingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		bool bHasBuilding;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = ActionFlags)
		bool bShouldLinkEquipAnim;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = ActionFlags)
		bool bIsDroppingWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = ActionFlags)
		bool bIsPickingupItem;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = ActionFlags)
		bool bIsEquippingWeapon;
	// -1 = None
	//  0 = Right
	//  1 = Left
	//  2 = Both

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = ActionFlags)
		int EquippingHand;
	// --------- Replicated (TODO) ------------
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Replicated, Category = CharacterComponents)
		UCharStatsComponent* CharStats;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Replicated, Category = ActionFlags)
		bool bIsEquiping;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = ActionFlags)
		bool bIsDualEquipAnim;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = ActionFlags)
		bool bShouldLinkEquipAnims;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Replicated, Category = ActionFlags)
		bool bIsAttacking;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Replicated, Category = ActionFlags)
		bool bIsBlocking;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Replicated, Category = ActionFlags)
		bool bIsParring;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Replicated, Category = MovementFlags)
		bool bIsRunning;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Replicated, Category = MovementFlags)
		bool bIsJumping;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Replicated, Category = MovementFlags)
		bool bIsRolling;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Replicated, Category = MovementFlags)
		bool bIsEvading;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Replicated, Category = MovementFlags)
		bool bIsAiming;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = MovementFlags)
		bool bCanRecoverStamina;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Replicated, Category = ActionFlags)
		bool bIsBeingDamaged;

	// -------------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = MovementFlags)
		bool bIsReloading;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = MovementFlags)
		bool bIsTurning;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = MovementFlags)
		int TurnDirection; // -1 = R | 1 = L
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = MovementFlags)
		bool bCanMove;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = MovementFlags)
		bool bIsTargeting;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = ActionFlags)
		bool bIsSwitchingWeapons;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ActionFlag)
		bool bIsPlayingAnimation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ActionFlag)
		bool bCanInterruptAnimation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CombatSystem)
		ECharVector EvadeDirection;

	UCurveFloat* GetCurve(FString PathToCurve);
	UCurveFloat* GetCurve(ECurveType Type);
	
	template<class T>
	bool HasSlotWeapon(FString WeaponModel = FString(), UCharWeaponSlot*& WeaponSlot = nullptr);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Inventory)
	TMap<EWeaponSlot, UCharWeaponSlot*> CharWeaponSlots;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
	TMap<EWeaponSlot, UCharWeaponSlot*> CurrentWeapons;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
	TMap<EWeaponSlot, UCharWeaponSlot*> PreviousWeapons;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		UCharWeaponSlot* AuxSlot;

	UPROPERTY()
	UCharWeaponSlot* PickWeaponSlot;
	UPROPERTY()
	UArmorsFactoryComponent* CharArmorsFactory;
	virtual void SetupCharWeaponSlots() {};
	virtual void SetupCharDefaultArmor();
	virtual void EquipSlotWeapon(UCharWeaponSlot* Slot, bool bIsEquip, EWeaponSlot Hand, bool bIsDualEquip = false, UCharWeaponSlot* SecondSlot = nullptr, bool bShouldLinkAnims = false);
	void AttachWeapon(AWeapon* Weapon, FName Socket, bool bIsVisible);
	void AttachWeapon(AWeapon* Weapon, EHolsterPosition Holster);
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		AWeapon* DefaultUnarmedWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		AWeapon* CurrentAttackWeapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterColliders)
	float MinCombatRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterColliders)
	float MaxCombatRange;

	void TrySwitchWeapon(EWeaponSlot RequestedSlot);
	void SetNextSlotToEquip(EWeaponSlot RequestedSlot, UCharWeaponSlot* Slot = nullptr);
	EWeaponSlot RequestedWeaponSlotID;
	UCharWeaponSlot* NextSlotToEquip;
	// ---------------------------------------------------------------------
	void PlaySoundQ(class USoundCue* SoundQ, FName SwitchParamName, int ParamValue, FName SubParamName = FName(), int SubParamValue = -1);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class UAudioComponent* FootstepsAudioComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class UAudioComponent* AudioComp;
	class USoundCue* CombatSoundCue;
	class USoundCue* MiscSoundCue;
	class USoundCue* CharSoundCue;
	// ---------------------- COMBAT SYSTEM --------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		int32 AttackCounter;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		bool bCanRequestCombo;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		bool bIsCombo;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		bool bCanAttack;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		bool bIgnoreDamage;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		bool bDamageAnimDisabled;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		EAttackType CurrentAttackType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatSystem)
		float AttackResetTime = 0.45f;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
		float AlertedStateSeconds = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		bool bCanChangeDefaultSkin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		EArmorType DefaultArmorType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		FString DefaultArmorModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Armor)
		int DefaultArmorConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Armor)
		class AArmor* CurrentArmor;
	virtual class AFist* SpawnDefaultUnarmedWeapon();
	void SetupCharArmor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatSystem)
		int PickingHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatSystem)
		bool bGroundPick;
	UFUNCTION()
		void ResetAttackStuff();
	
	void HandleAnimatorAttackRequest(UCharWeaponSlot* WeaponSlot);
	void HandleComboStuff(AWeapon* CurrentWeapon);
	FTimerHandle AttackResetTimerHandle;
	FTimerHandle EndAttackDelayHandle;
	FTimerHandle DestroyCharTimerHandle;
	EAttackType RequestedAttackType;
	FTimerHandle AnimStateUpdateTimer;
	FTimerHandle AlertedStateTimerHandle;
	float DestroyCharCountdown;
	// --------------------------------------------------------------------
	

	void SetBlendspaceTL(ECurveType BlendCurve, float TimePartition = 1.0f, bool bLooping = false);
	void SetBlendspaceTL(UCurveFloat* BlendCurve, float TimePartition = 1.0f, bool bLooping = false);
	UBlendSpace1D* CharAnimBS;
	UPROPERTY()
	FTimeline BlendSpace1DTL;
	bool bHandleBlendspaceTL;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
		UCurveFloat* SpeedChangeCurve;

	void SetSpeedChangeTL(float SpeedTLPlayRate = 1.f);
	bool TryLoadSpeedChangeCurve(ECurveType Type = ECurveType::LINEAR, bool bForceUpdate = true);

	UPROPERTY()
	FTimeline SpeedChangeTL;
	FTimerHandle SpeedChangeTimer;
	bool bHandleSpeedChange;
	float SpeedTLPartition;
	ECurveType CurrentSpeedChangeCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CharacterColliders)
		UWidgetComponent* CharStatsBarWidget;

	virtual void AddCharStatsBar() {};
	void SetupCharStatsBar();
	TSubclassOf<class UUserWidget> CharStatsBarBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharacterColliders)
		UWidgetComponent* TargetWidget;

	TSubclassOf<class UUserWidget> TargetWidgetBP;
	void SetupTargetWidget();
	
	class ABaseBuilding* CharHome;

private:
	UFUNCTION()
		virtual void HandleSpeedChangeTL(float DeltaTime);
	UFUNCTION()
		virtual void OnTimelineFinished();

	protected:
		void CacheDefaultUnarmedWeapon();
		void ResetDefaultWeapon();
	void ManageStamina(float DeltaTime);
	void HandleStaminaRecovery(float DeltaTime);
	void HandleSprintStamina(float DeltaTime);
	virtual void HandleBlockingStamina(float DeltaTime);
	float GetActionStaminaDecrease(ECharAction Action);
	float GetAttackStaminaDecrease(EAttackType AttackType);
	UFUNCTION()
		void EnableStamRecovery();
	FTimerHandle EnableStaminaRecoveryTimer;
public:
	virtual bool CanExecuteAction(ECharAction CharAction);
	virtual bool CanExecuteAttack(EAttackType AttackType);
	virtual bool HasMinActionStam(ECharAction CharAction);
	virtual bool HasMinAttackStam(EAttackType AttackType);
	UFUNCTION()
		void HandleActionStaminaDecrease(ECharAction CharAction, bool bRunOnServer = false);

	UFUNCTION()
		void UpdateStamina(float Value, bool bRunOnServer = false);
	UFUNCTION(Server, Reliable)
		void Server_UpdateStamina(float Value);
	
	UFUNCTION(BlueprintCallable)
		void UpdateHealth(float Value, bool bRunOnServer = false);
	UFUNCTION(Server, Reliable)
		void Server_UpdateHealth(float Value);
	UFUNCTION(Server, Reliable)
		void Server_HandleActionStaminaDecrease(ECharAction CharAction);
	
	
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void GetPlayerViewPoint(FVector& OutLocation, FRotator& OutRotation, bool bIsCameraView = true) const;

	int GetScaledSpeed() const;

	UFUNCTION()
		void UpdateWalkSpeed(ECurveType CurveType);
	UFUNCTION()
		float PlayMontage(UAnimMontage* Montage, FName Section = FName("Default"), float Speed = 1.f, bool bAddAnimPlayedTimer = true, bool bWithRootMot = true, bool bNetCast = false);
	UFUNCTION(Server, Reliable)
		virtual void Server_PlayMontage(UAnimMontage* Montage, FName Section = FName("Default"), float Speed = 1.f, bool bAddAnimPlayedTimer = true);
	UFUNCTION(NetMulticast, Reliable)
		virtual void NetCast_PlayMontage(UAnimMontage* Montage, FName Section = FName("Default"), float Speed = 1.f, bool bAddAnimPlayedTimer = true);
	UFUNCTION()
		float ExecuteBlendspace(UBlendSpace1D* BS, UCurveFloat* AnimCurve,float TimelinePartition, bool bWithRootMot = true, bool bNetCast = false);
	UFUNCTION()
		void OnMontageSectionPlayed();

	void SetPlayMontageRestoreTimer(float CurrentAnimLenght, float PlayRate = 1.f);
	FTimerHandle MontagePlayedRestoreTimer;

	UFUNCTION()
		virtual void DropWeaponSlot(EWeaponSlot Slot) {};
	UFUNCTION()
		virtual void TriggerPickupItem(EWeaponSlot Slot = EWeaponSlot::INVALID);

	UFUNCTION()
		virtual void TriggerWeaponSlot(EWeaponSlot Slot) {};
	UFUNCTION()
		virtual void EquipWeapons(EWeaponSlot Hand = EWeaponSlot::INVALID, bool bTryDualEquip = true) {};

	UFUNCTION()
		void DropWeapon(EWeaponSlot WeaponSlot, bool bAddDefaultWeapon = true);
	UFUNCTION()
		void PickWeapon(EWeaponSlot WeaponSlot); 
	UFUNCTION()
		virtual void StartSprint();
	UFUNCTION()
		virtual void EndSprint();
	UFUNCTION()
		virtual void StartJump();
	UFUNCTION()
		virtual void EndJump();
	UFUNCTION()
		virtual void StartCrouch();
	UFUNCTION()
		virtual void EndCrouch();

	
	UFUNCTION()
		virtual void StartPrimaryAttack() {};
	UFUNCTION()
		virtual void EndPrimaryAttack() {};
	UFUNCTION()
		virtual void StartSecondaryAttack() {};
	UFUNCTION()
		virtual void EndSecondaryAttack() {};
	UFUNCTION()
		void DropKick();
	UFUNCTION()
		virtual void StartBlocking() {};
	UFUNCTION()
		virtual void EndBlocking() {};
	
	UFUNCTION()
		virtual void StartParry() {};
	UFUNCTION()
		virtual void EndParry() {};
	UFUNCTION()
		virtual void TriggerEvade() {};
	UFUNCTION()
		virtual void TryLockTarget() {};
	template<class T>
	T* GetCharStats() const { return CharStats ? Cast<T>(CharStats) : nullptr; }

	bool CheckBodyCollisionToByChannel(AActor* Target, TArray<AActor*>& Hitten, bool bOverlap = true, FVector Origin = FVector::ZeroVector, TArray<AActor*> IgnoredActors = TArray<AActor*>(), float ShapeMult = 1.f, ECollisionChannel Channel = ECollisionChannel::ECC_Visibility);
	bool CheckBodyCollisionToByChannel(FVector Target, TArray<AActor*>& Hitten, bool bOverlap = true, FVector Origin = FVector::ZeroVector, TArray<AActor*> IgnoredActors = TArray<AActor*>(), float ShapeMult = 1.f, ECollisionChannel Channel = ECollisionChannel::ECC_Visibility);
	bool CheckBodyCollisionToByObject(FVector Target, TArray<AActor*>& Hitten, bool bOverlap = true, FVector Origin = FVector::ZeroVector, TArray<AActor*> IgnoredActors = TArray<AActor*>(), float ShapeMult = 1.f, bool bWorldStatic = true);
	bool CheckBodyCollisionToByProfile(FVector Target, TArray<AActor*>& Hitten, FName Profile, bool bOverlap = true, FVector Origin = FVector::ZeroVector, TArray<AActor*> IgnoredActors = TArray<AActor*>(), float ShapeMult = 1.f);

	TArray<AActor*> GetSweepResult(FVector Origin, float SphereRadius, bool bOverlap = true, ECollisionChannel Channel = ECollisionChannel::ECC_Visibility, TArray<AActor*> IgnoredActors = TArray<AActor*>());
	TArray<AActor*> GetBodySweepResult(FVector Origin, FVector End, bool bOverlap = true, ECollisionChannel Channel = ECollisionChannel::ECC_Visibility, TArray<AActor*> IgnoredActors = TArray<AActor*>(), float ShapeMult = 1.f);
	TArray<AActor*> GetBodySweepResult(FVector Origin, FVector End, bool bWorldStatic, bool bOverlap = true, TArray<AActor*> IgnoredActors = TArray<AActor*>(), float ShapeMult = 1.f);
	TArray<AActor*> GetBodySweepResult(FVector Origin, FVector End, FName Profile, bool bOverlap = true, TArray<AActor*> IgnoredActors = TArray<AActor*>(), float ShapeMult = 1.f);

	bool HasTargetsAtSight(FVector OriginLoc, bool bCheckOnlyEnemies = false, float SweepRange = 3000);
	bool CanSeeTarget(AActor* Target, bool bWithSightRangeCheck = false, bool bOnlyCharCollisions = true);
	bool HasObstacleInBetween(AActor* Target);
	bool IsFacingObstacle();
	FOnNotifyCharDeadSignature OnNotifyDead;
	FOnWalkSpeedChangeSignature OnWalkSpeedChange;
	FOnRestoreWalkSpeedSignature OnRestoreWalkSpeed;
	FOnModifyStatsSignature OnModifyStats;

	bool IsTargetAside(FVector TargLoc, ECharVector Side, float FwdAxisToleranceDegs = DEFAULT_CHAR_FOV) const;
	bool IsTargetForward(FVector TargLoc,  float ToleranceDegs = DEFAULT_CHAR_FOV) const;
	bool IsTargetBackward(FVector TargLoc, float ToleranceDegs = DEFAULT_CHAR_FOV) const;
	bool IsTargetForward(AActor* Target, float ToleranceDegs = DEFAULT_CHAR_FOV);
	virtual bool IsTargetInsideFOV(AActor* Target, bool bCheckCol, float Angle = 0.f) override;

	TArray<ITargeteable*> GetClosestTargeteables(ETargetType Type, bool bGetClosest = false, float SweepRadius = 1000.f, float MinDistance = 0.f);
	void OnItemCollected(AActor* Item, EWeaponSlot RequestedSlot);
};

template<class T>
inline bool ABasePraiseCharacter::HasSlotWeapon(FString WeaponModel, UCharWeaponSlot*& WeaponSlot)
{
	if (CharWeaponSlots.Num() <= 0) return false;

	TArray<EWeaponSlot> Keys;
	CharWeaponSlots.GetKeys(Keys);

	for (EWeaponSlot Key : Keys)
	{
		if (!CharWeaponSlots[Key]->IsEmpty() && CharWeaponSlots[Key]->HoldsA<T>())
		{
			if (!WeaponModel.IsEmpty())
			{
				if (CharWeaponSlots[Key]->GetSlotWeapon()->GetWeaponModel() == WeaponModel)
				{
					WeaponSlot = CharWeaponSlots[Key];
					return true;
				}
			}
			else
			{
				WeaponSlot = CharWeaponSlots[Key];
				return true;
			}
		}
	}

	return false;
}
