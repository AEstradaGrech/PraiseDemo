// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Structs/CommonUtility/FLogger.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "../Structs/CommonUtility/FUtilities.h"
#include <unordered_map>
#include "../Enums/CombatSystem/EWeaponFX.h"
#include "../Enums/CombatSystem/EWeaponType.h"
#include "../Enums/CombatSystem/EWeaponClass.h"
#include "../Enums/CombatSystem/EArmedPoses.h"
#include "../Enums/CombatSystem/ETargetType.h"
#include "../Enums/CombatSystem/EAttackType.h"
#include "../Enums/CombatSystem/EWeaponSlot.h"
#include "../Enums/Characters/ECharStatus.h"
#include "../Components/Actor/DbComponents/WeaponsDBComponent.h"
#include "../Structs/CombatSystem/Weapons/FWeaponDTStruct.h"
#include "../Structs/CombatSystem/Weapons/FWeaponArtDTStruct.h"
#include "../Structs/CombatSystem/Weapons/FWeaponAbilitiesDTStruct.h"
#include "../Structs/CombatSystem/Weapons/FWeaponFxDTStruct.h"
#include "../Interfaces/CombatSystem/Targeteable.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Configs/WeaponConfig.h"
#include "Sound/SoundCue.h"
#include "Weapon.generated.h"


UCLASS()
class PRAISE_API AWeapon : public AActor, public ITargeteable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeapon();

	// ---- ITargeteable 
	virtual ETargetType	IsTargetType() const override;
	virtual int32 GetTargetID() const override;
	virtual bool IsDead() const override;
	virtual ECharStatus GetCharStatus() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Weapon)
		class USkeletalMeshComponent* WeaponMesh;

	FVector GetBlockLocation() const { return BlockLocation; }
	FRotator GetBlockRotation() const { return BlockRotation; }
	FRotator GetEquipRotation(bool bIsRightHand = true) const { return bIsRightHand ? EquipRotationR : EquipRotationL; }
	FVector GetEquipLocation(bool bIsRightHand = true) const { return bIsRightHand ? EquipLocationR : EquipLocationL; }
	FVector GetWeaponScale() const { return WeaponScale == FVector::ZeroVector ? FVector(1.f, 1.f, 1.f) : WeaponScale; }
	FORCEINLINE UAudioComponent* GetWeaponAudioComp() const { return WeaponAudioComp; }
	FORCEINLINE int32 GetWeaponLvl() const { return Lvl; }
	FORCEINLINE float GetWeaponDamage() const { return Damage; }
	FORCEINLINE float GetWeight() const { return Weight; }
	FORCEINLINE float GetAttackRate() const { return AttackRate; }
	FORCEINLINE float GetMeleeDistanceOffset() const { return MeleeDistanceOffset; }
	FORCEINLINE bool DidInit() const { return bDidInit; }
	FORCEINLINE bool IsTwoHand() const { return bIsTwoHand; }
	FORCEINLINE bool GetIsBroken() const { return bIsBroken; }
	FORCEINLINE bool IsEquiped() const { return bIsEquiped; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE EWeaponClass GetWeaponClass() const { return WeaponClass; }
	FORCEINLINE EArmedPoses GetCombatPose() const { return ArmedPose; }
	FORCEINLINE FString GetWeaponModel() const { return WeaponModel; }
	FORCEINLINE FWeaponArtDTStruct* GetCurrentSkinData() const { return CurrentSkin; }
	FORCEINLINE void SetStatsConfigID(int32 Value) { StatsConfigID = Value; }
	FORCEINLINE TMap<EHolsterPosition, FVector> GetHolsterLocations() const { return CurrentSkin ? CurrentSkin->HolsterLocations : TMap<EHolsterPosition, FVector>(); }
	FORCEINLINE TMap<EHolsterPosition, FRotator> GetHolsterRotations() const { return CurrentSkin ? CurrentSkin->HolsterRotations : TMap<EHolsterPosition, FRotator>(); }
	FORCEINLINE TArray<class UParticleSystem*> GetWeaponFX() const { return WeaponFX; }
	FORCEINLINE TArray<AActor*> GetCurrentAttackHits() const { return CurrentAttackHits; }
	FORCEINLINE TArray<EHolsterPosition> GetHolsterPositions() const { return AvailableHolsterPositions; }
	virtual bool InitWeapon(FString Model, EWeaponType Type);
	virtual bool InitWeapon(FWeaponDTStruct* WeaponData);
	void EnableWeaponCollider(EAttackType AttackType, bool bEnable);
	void HandleEnableCollider(UBoxComponent* Collider, bool bEnable);
	float GetWeaponState() const;
	float GetWeaponDurability() const;
	float GetWeaponStateAsRatio() const;
	float GetDurabilityAsRatio() const;
	bool HasOwner() const { return GetOwner() != nullptr; }
	class UParticleSystem* GetWeaponFxByKey(EWeaponFX Key) { return WeaponFX[(int)Key]; }
	void ClearCurrentHits();
	
	template<typename T>
	T* GetCurrentConfigAs();
	
	virtual UWeaponConfig* GetCurrentStatsAsConfig();
	virtual bool TryInitStats();
	void GetHolsterCoords(EHolsterPosition Position, FVector& OutLoc, FRotator& OutRot);
	void OnWeaponDropped();
	void OnWeaponPickup(class ABasePraiseCharacter* NewOwner, EWeaponSlot PickingHand);
	EHolsterPosition GetHolsterForHand(EWeaponSlot EquippingHand);
	void ReattachAudioComp();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void SetDefaults();
	virtual void SetSoundCue() {};
	virtual void SetWeaponFx();
	virtual void SpawnWeaponEmitters(const FVector& HitPoint, const FHitResult& Hit, UParticleSystem* HitImpactFX) {};
	virtual void AddCameraShakeOnAttack();
	virtual void SetCameraShaker();
	virtual void SetShakePitch(class UMatineeCameraShake& CS);
	virtual void SetShakeYaw(class UMatineeCameraShake& CS);
	virtual void SetShakeFOV(class UMatineeCameraShake& CS);
	virtual bool TryCalculateWeaponLvl();
	void SetWeaponState(float Value);
	virtual class UParticleSystem* GetImpactFX(EPhysicalSurface ImpactSurface);
	virtual void AssignWeaponFX(int FxType, UParticleSystem* FX);
	virtual float GetAttackDamage();
	virtual float CalculateAttackDamage();
	virtual float TryGetAttackCritDamage(float CurrentDamage);

	virtual bool TryAddBaseStats();
	virtual bool TryAddStatsConfig();
	virtual bool TryAddStatModifiers();
	virtual bool TrySetupSkeletalMesh(FWeaponArtDTStruct* ArtData = nullptr);
	virtual bool TrySetWeaponFX(uint32 FxID = 0);
	virtual bool TrySetSkeletalMeshFromPath(FString Path);
	virtual bool TrySetMeshMatsFromPath(FString Path, int idx = 0, bool bOverwrite = false);
	virtual bool TrySetMeshMatsFromPathArray(TArray<FString> Paths);

	virtual bool TrySetupWeaponColliders(FWeaponArtDTStruct* ArtData = nullptr);
	virtual void UpdateStatsFromConfig(FWeaponDTStruct* Config = nullptr, bool bIsAdditive = false, bool bAppliesOnConfig = true);
	virtual void ResetToCurrentConfig();
	// MODIFIER FUNCTIONS
	virtual void ApplyModifier(FWeaponDTStruct* Modifier, bool bAppliesOnConfig = true);
	virtual void ApplyMultiplier(FWeaponDTStruct* Multiplier, bool bAppliesOnConfig = true);
	virtual void ApplyPercentage(FWeaponDTStruct* Modifier, bool bAppliesOnConfig = true, bool bIsConfigBased = false);
	virtual void FullOverwriteStats(FWeaponDTStruct* NewStats);
	virtual void OverwriteStats(FWeaponDTStruct* NewStats, bool bAppliesOnConfig = true);
	virtual void PlusStats(FWeaponDTStruct* Addition, bool bAppliesOnConfig = false);

	void SetupCurrentStatsArrays(FWeaponDTStruct* Config, bool bIsAdditive = false);
	void SetWeponFxByID(uint32 ID);
public:
	// Called every frame

	virtual void Tick(float DeltaTime) override;
	virtual void Equip(bool bEquip, FVector SocketLoc, FRotator SocketRot, bool bPlaySFX = true);
	virtual void Attack(bool bIsPlayerAttack = true) {};
	virtual void Attack(AActor* Target) {};
	virtual void HandleBreakGuardAttack(AActor* Target) {};
	virtual void PlaySFX(ECombatSFX SFX, int SampleIdx, bool bLoopSound = false, float PitchVariationRange = 0.f);
	virtual int GetSoundSampleIdx(ECombatSFX SFX);
	void ApplyWeaponStateReduction();

	UFUNCTION()
		virtual void OnInteractionColBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnInteractionColEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		virtual void OnWeaponBoxBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnAttackBoxBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
		USoundCue* WeaponSoundCue;
	
	UAudioComponent* WeaponAudioComp;
	
	FRotator EquipRotationR;
	FVector EquipLocationR;
	FRotator EquipRotationL;
	FVector EquipLocationL;
	FRotator BlockRotation;
	FVector BlockLocation;

	FVector WeaponColExtents;
	FVector DroppedWeaponColExtents;
	FVector WeaponColLoc;
	FVector DamageColExtents;
	FVector DamageColLoc;
	FVector WeaponScale;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Weapon")
	UWeaponsDBComponent* WeaponsDBComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
	ETargetType TargetType;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = CombatSystem)
	ECharStatus ItemStatus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	EWeaponType WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	EWeaponClass WeaponClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	EArmedPoses ArmedPose;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	FString WeaponModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	int32 StatsConfigID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	int32 Lvl;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
	bool bIsTwoHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float BaseXP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float CurrentXP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float XPToLevelUp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon )
	float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float CritDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float CritChance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon )
	float Weight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon )
	float AttackRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (MinClamp = "0", MaxClamp = "100"))
	float WeaponState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (MinClamp = "0", MaxClamp = "100"))
	float Durability;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float WeaponStateReductionConstant;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (MinClamp = "0", MaxClamp = "100"))
	float ChanceToBreak;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float MeleeDistanceOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon )
	bool bIsBroken;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
	bool bIsEquiped;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	bool bCanEquipLeftOnly; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	bool bDidInit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	FString Description;
	UPROPERTY()
	TArray<int32> Modifiers;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = Weapon)
	TArray<FString> CurrentMods;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TArray<int32> WeaponAbilities;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TArray<int32> WeaponSkins;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TArray<int32> FX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TArray<EHolsterPosition> AvailableHolsterPositions;
	
	FWeaponArtDTStruct* CurrentSkin;
	
	UPROPERTY(EditAnywhere, Category = FX, meta = (AllowprivateAccess = "true"))
		TArray<class UParticleSystem*> WeaponFX;

		TMap<ECombatSFX,TArray<int>> WeaponSFX;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = WeaponColliders, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* WeaponCollider; 
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = WeaponColliders, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* DamageCollider; 
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = WeaponColliders, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* BreakGuardCollider;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = WeaponColliders, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* InteractionCollider;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = WeaponColliders, meta = (AllowPrivateAccess = "true"))
		float InteractionColRadius;
	std::unordered_map<int, class UParticleSystem*> CurrentWeaponFX;

	UPROPERTY(EditDefaultsOnly, Category = FX)
	TSubclassOf<class UMatineeCameraShake> CameraShaker;

	float GetFloatRandomizer(float Value, float percentage, bool bOnlyPositive = false);
	float GetWeaponStateFactor() const;
	float GetWeaponStateReduction() const;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
	UWeaponConfig* StatsConfig;

	TArray<AActor*> CurrentAttackHits;
};

template<typename T>
inline T* AWeapon::GetCurrentConfigAs()
{
	if (!StatsConfig) return nullptr;

	return Cast<T>(StatsConfig);
}
