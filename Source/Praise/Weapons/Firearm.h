// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "../Structs/CombatSystem/Weapons/FWeaponDTStruct.h"
#include "../Structs/CombatSystem/Weapons/FFiregunDTStruct.h"
#include "Firearm.generated.h"

/**
 *
 */
UCLASS()
class PRAISE_API AFirearm : public AWeapon
{
	GENERATED_BODY()

public:
	AFirearm();
	FORCEINLINE int GetAmmo() const { return Ammo; }
	FORCEINLINE int GetMaxAmmo() const { return MaxAmmo; }
	FORCEINLINE int GetMagAmmo() const { return MaxMagAmmo; }
	FORCEINLINE FVector GetAimingLocation() const { return AimLocation; }
	FORCEINLINE FRotator GetAimingRotation() const { return AimRotation; }
	FORCEINLINE int GetTotalAmmo() const { return TotalAmmo; }
	FORCEINLINE bool CanShoot() const { return TotalAmmo > 0 && Ammo > 0; }
	void ReloadMag(int Bullets = 1, bool bIsOverwrite = false);

protected:

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual bool TrySetupSkeletalMesh(FWeaponArtDTStruct* ArtData = nullptr) override;
	virtual bool TrySetupWeaponColliders(FWeaponArtDTStruct* ArtData = nullptr) override;
	virtual void ResetToCurrentConfig() override;
	virtual void FullOverwriteStats(FWeaponDTStruct* NewStats) override;
	virtual void OverwriteStats(FWeaponDTStruct* NewStats, bool bAppliesOnConfig = true) override;
	virtual void PlusStats(FWeaponDTStruct* Addition, bool bAppliesOnConfig = false) override;

	virtual void ApplyModifier(FWeaponDTStruct* Modifier, bool bAppliesOnConfig = false) override;
	virtual void ApplyMultiplier(FWeaponDTStruct* Multiplier, bool bAppliesOnConfig = false) override;
	virtual void ApplyPercentage(FWeaponDTStruct* Modifier, bool bAppliesOnConfig = false, bool bIsConfigBased = true) override;
	
	virtual UWeaponConfig* GetCurrentStatsAsConfig()override;
	virtual void SetDefaults() override;
	virtual void Attack(bool bIsPlayerAttack = true) override;
	virtual bool CanFire();
	virtual bool TryGetHitResultFromViewpoint(FHitResult& Hit, FVector& HitDirection, bool bIsCameraView = true, float SpreadHrz = 0.f, float SpreadVrt = 0.f);
	virtual FCollisionQueryParams GetShootLineTraceQueryParams(bool bIsTraceComplex, TArray<AActor*>IgnoredActors);
	virtual void OnShootHitSpawnFX(FHitResult& Hit);
	void DrawDebugSphereOnImpactPoint(const FHitResult& Hit);
	virtual void SpawnWeaponEmitters(const FVector& HitPoint, const FHitResult& Hit, UParticleSystem* HitImpactFX) override;
	virtual void AssignWeaponFX(int FxType, UParticleSystem* ParticleFX) override;
	
	virtual void Attack(AActor* Target) override;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon)
	float Range;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon  )
	int Ammo;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon)
	int TotalAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon  )
	int MaxAmmo; 
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon  )
	int MaxMagAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon, meta = (MinClamp="0", MaxClamp="100"))
	float SpreadCorrectionPercentage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float ReloadTime;
	UPROPERTY(EditAnywhere, meta = (MinClamp = "0", MaxClamp = "80"))
	float HorizontalSpreadDegs = 0.f;
	UPROPERTY(EditAnywhere, meta = (MinClamp = "0", MaxClamp = "80"))
	float VerticalSpreadDegs = 0.f;

	FName MuzzleSocketName = "b_gun_muzzleflash";
	FName BeamFxTargetParameterName = "Target";
	float TimeSinceLastShoot;
	class UAudioComponent* ShootSFX;

	FRotator AimRotation;
	FVector  AimLocation;
	

public:
	virtual bool InitWeapon(FWeaponDTStruct* WeaponData) override;
	virtual int GetSoundSampleIdx(ECombatSFX SFX) override;



};
