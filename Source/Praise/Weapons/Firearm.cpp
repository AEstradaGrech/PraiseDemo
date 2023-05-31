// Fill out your copyright notice in the Description page of Project Settings.


#include "Firearm.h"
#include "../Characters/BasePraiseCharacter.h"
#include "Camera/CameraComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Misc/DefaultValueHelper.h"
#include "../Praise.h"
#include "Configs/FirearmConfig.h"
#include "PhysicalMaterials/PhysicalMaterial.h"


AFirearm::AFirearm() : Super()
{
	WeaponType = EWeaponType::FIREARM;
	BeamFxTargetParameterName = "BeamTarget";
	MuzzleSocketName = FName("MuzzleSocket");

	BreakGuardCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Break Guard Collider"));
	BreakGuardCollider->AttachTo(RootComponent);
	BreakGuardCollider->SetBoxExtent(FVector(0, 0, 0));
	BreakGuardCollider->SetVisibility(false);
	BreakGuardCollider->SetCollisionProfileName(FName("NoCollision"));
	BreakGuardCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	SetDefaults();
}


void AFirearm::ResetToCurrentConfig()
{
	Super::ResetToCurrentConfig();

	UFirearmConfig* Config = GetCurrentConfigAs<UFirearmConfig>();

	if (!Config) return;

	Range = Config->Range;
	ReloadTime = Config->ReloadTime;
	MaxAmmo = Config->MaxAmmo;
	MaxMagAmmo = Config->MaxMagAmmo;
	SpreadCorrectionPercentage = Config->SpreadCorrectionPercentage;
	HorizontalSpreadDegs = Config->HorizontalSpreadDegs;
	VerticalSpreadDegs = Config->VerticalSpreadDegs;
}

void AFirearm::ReloadMag(int Bullets, bool bIsOverwrite)
{
	if (TotalAmmo < 0) return;

	if (TotalAmmo - Bullets < 0)
	{
		if (!bIsOverwrite) return;

		Bullets = FMath::Abs(TotalAmmo - Bullets);
	}

	if (Ammo >= MaxMagAmmo) return;

	if (Bullets <= 0) return;

	if (Bullets > MaxMagAmmo)
		Bullets = MaxMagAmmo;

	if (bIsOverwrite)
		Ammo = Bullets;

	else Ammo += Bullets;

	TotalAmmo -= Bullets;
}

void AFirearm::BeginPlay()
{
	Super::BeginPlay();

	TotalAmmo = MaxAmmo;

	BeamFxTargetParameterName = "BeamTarget";
}

void AFirearm::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StatsConfig = NewObject<UFirearmConfig>(this, FName("Firearm Config"));
}

int AFirearm::GetSoundSampleIdx(ECombatSFX SFX)
{
	if (SFX == ECombatSFX::WEAPON_ATTACK)
	{
		if (GetOwner() && GetOwner()->IsA<ABasePraiseCharacter>())
		{
			ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(GetOwner());

			if (Char->IsAttacking())
			{
				if (Char->GetCurrentAttackType() == EAttackType::SECONDARY_ATTACK)
				{
					return WeaponSFX.Contains(SFX) ? WeaponSFX[SFX].Last() : 0;
				}

				else return WeaponSFX.Contains(SFX) && WeaponSFX[SFX].Num() > 0 ? WeaponSFX[SFX][FMath::RandRange(0, WeaponSFX[SFX].Last() - 2)] : 1;
			}
		}
	}
	if (SFX == ECombatSFX::WEAPON_HIT)
	{
		if (GetOwner() && GetOwner()->IsA<ABasePraiseCharacter>())
		{
			ABasePraiseCharacter* Char = Cast<ABasePraiseCharacter>(GetOwner());

			if (Char->IsAttacking())
			{
				if (Char->GetCurrentAttackType() == EAttackType::SECONDARY_ATTACK)
				{
					return WeaponSFX.Contains(SFX) && WeaponSFX[SFX].Num() > 0 ? WeaponSFX[SFX][0] : 0;
				}

				else return FMath::RandRange(1, 2);
			}
		}
	}
	return WeaponSFX.Contains(SFX) && WeaponSFX[SFX].Num() > 0 ? WeaponSFX[SFX][FMath::RandRange(0, WeaponSFX[SFX].Num() - 1)] : 0;
}


bool AFirearm::TrySetupWeaponColliders(FWeaponArtDTStruct* ArtData)
{
	if (!Super::TrySetupWeaponColliders(ArtData)) return false;

	if (!CurrentSkin) return false;

	if (!BreakGuardCollider) return false;

	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);

	BreakGuardCollider->AttachToComponent(WeaponMesh, AttachmentRules);
	BreakGuardCollider->SetRelativeLocation(CurrentSkin->BreakGuardLocation);
	BreakGuardCollider->SetBoxExtent(CurrentSkin->BreakGuardExtents);
	BreakGuardCollider->OnComponentBeginOverlap.AddDynamic(this, &AFirearm::OnAttackBoxBeginOverlap);

	HandleEnableCollider(BreakGuardCollider, false);

	return true;
}

bool AFirearm::TrySetupSkeletalMesh(FWeaponArtDTStruct* ArtData)
{
	CurrentSkin = ArtData;

	return Super::TrySetupSkeletalMesh(ArtData);
}

void AFirearm::FullOverwriteStats(FWeaponDTStruct* NewStats)
{
	Super::FullOverwriteStats(NewStats);

	FFiregunDTStruct* NewGunStats = (FFiregunDTStruct*)NewStats;

	Range = NewGunStats->Range;
	ReloadTime = NewGunStats->ReloadTime;
	MaxAmmo = NewGunStats->MaxAmmo;
	MaxMagAmmo = NewGunStats->MaxMagAmmo;
	SpreadCorrectionPercentage = NewGunStats->SpreadCorrectionPercentage;
	HorizontalSpreadDegs = NewGunStats->HorizontalSpreadDegs;
	VerticalSpreadDegs = NewGunStats->VerticalSpreadDegs;
	AimLocation = NewGunStats->AimLocation;
	AimRotation = NewGunStats->AimRotation;
}

void AFirearm::OverwriteStats(FWeaponDTStruct* NewStats, bool bAppliesOnConfig)
{
	Super::OverwriteStats(NewStats, bAppliesOnConfig);

	FFiregunDTStruct* NewGunStats = (FFiregunDTStruct*)NewStats;

	UFirearmConfig* CurrentStats = GetCurrentConfigAs<UFirearmConfig>();

	if (!CurrentStats) return;

	switch (bAppliesOnConfig)
	{
		case(false):
			Range = NewGunStats->Range;
			MaxAmmo = NewGunStats->MaxAmmo;
			MaxMagAmmo = NewGunStats->MaxMagAmmo;
			SpreadCorrectionPercentage = NewGunStats->SpreadCorrectionPercentage;
			HorizontalSpreadDegs = NewGunStats->HorizontalSpreadDegs;
			VerticalSpreadDegs = NewGunStats->VerticalSpreadDegs;
			ReloadTime = NewGunStats->ReloadTime;

			break;
		case(true):

			CurrentStats->Range = NewGunStats->Range;
			CurrentStats->MaxAmmo = NewGunStats->MaxAmmo;
			CurrentStats->MaxMagAmmo = NewGunStats->MaxMagAmmo;
			CurrentStats->SpreadCorrectionPercentage = NewGunStats->SpreadCorrectionPercentage;
			CurrentStats->HorizontalSpreadDegs = NewGunStats->HorizontalSpreadDegs;
			CurrentStats->VerticalSpreadDegs = NewGunStats->VerticalSpreadDegs;
			CurrentStats->ReloadTime = NewGunStats->ReloadTime;
		
			break;
	}
	
}

void AFirearm::PlusStats(FWeaponDTStruct* Addition, bool bAppliesOnConfig)
{
	Super::PlusStats(Addition, /*bAppliesOnConfig:*/ bAppliesOnConfig);

	FFiregunDTStruct* addition = (FFiregunDTStruct*)Addition;

	UFirearmConfig* Config = GetCurrentConfigAs<UFirearmConfig>();

	if (!Config) return;

	switch (bAppliesOnConfig)
	{
		case(false):

			Range = Range += addition->Range;
			MaxAmmo = MaxAmmo += addition->MaxAmmo;
			MaxMagAmmo = MaxMagAmmo += addition->MaxMagAmmo;
			SpreadCorrectionPercentage = SpreadCorrectionPercentage += addition->SpreadCorrectionPercentage;
			HorizontalSpreadDegs = HorizontalSpreadDegs += addition->HorizontalSpreadDegs;
			VerticalSpreadDegs = VerticalSpreadDegs += addition->VerticalSpreadDegs;
			ReloadTime = ReloadTime += addition->ReloadTime;
			break;
		case(true):

			Config->Range = Range += addition->Range;
			Config->MaxAmmo = MaxAmmo += addition->MaxAmmo;
			Config->MaxMagAmmo = MaxMagAmmo += addition->MaxMagAmmo;
			Config->SpreadCorrectionPercentage = SpreadCorrectionPercentage += addition->SpreadCorrectionPercentage;
			Config->HorizontalSpreadDegs = HorizontalSpreadDegs += addition->HorizontalSpreadDegs;
			Config->VerticalSpreadDegs = VerticalSpreadDegs += addition->VerticalSpreadDegs;
			Config->ReloadTime = ReloadTime += addition->ReloadTime;
			break;
	}
	
}

void AFirearm::ApplyModifier(FWeaponDTStruct* Modifier, bool bAppliesOnConfig)
{
	FFiregunDTStruct* NewGunMod = (FFiregunDTStruct*)Modifier;
	
	if (!NewGunMod) return;

	UFirearmConfig* Config = GetCurrentConfigAs<UFirearmConfig>();

	if (!Config) return;

	if (bAppliesOnConfig)
	{
		Config->Range += NewGunMod->Range;
		Config->MaxAmmo += NewGunMod->MaxAmmo;
		Config->MaxMagAmmo += NewGunMod->MaxMagAmmo;
		Config->SpreadCorrectionPercentage += NewGunMod->SpreadCorrectionPercentage;
		Config->HorizontalSpreadDegs += NewGunMod->HorizontalSpreadDegs;
		Config->VerticalSpreadDegs += NewGunMod->VerticalSpreadDegs;
		Config->ReloadTime += NewGunMod->ReloadTime;
	}
	else
	{
		Range += NewGunMod->Range;
		MaxAmmo += NewGunMod->MaxAmmo;
		MaxMagAmmo += NewGunMod->MaxMagAmmo;
		SpreadCorrectionPercentage += NewGunMod->SpreadCorrectionPercentage;
		HorizontalSpreadDegs += NewGunMod->HorizontalSpreadDegs;
		VerticalSpreadDegs += NewGunMod->VerticalSpreadDegs;
		ReloadTime += NewGunMod->ReloadTime;
	}
	
	Super::ApplyModifier(Modifier, bAppliesOnConfig);
}

void AFirearm::ApplyMultiplier(FWeaponDTStruct* Multiplier, bool bAppliesOnConfig)
{
	FFiregunDTStruct* CastedMult = (FFiregunDTStruct*)Multiplier;

	if (!CastedMult) return;

	UFirearmConfig* Config = GetCurrentConfigAs<UFirearmConfig>();

	if (!Config) return;

	if (bAppliesOnConfig)
	{
		Config->Range *= CastedMult->Range;
		Config->MaxAmmo *= CastedMult->MaxAmmo;
		Config->MaxMagAmmo *= CastedMult->MaxMagAmmo;
		Config->SpreadCorrectionPercentage *= CastedMult->SpreadCorrectionPercentage;
		Config->HorizontalSpreadDegs *= CastedMult->HorizontalSpreadDegs;
		Config->VerticalSpreadDegs *= CastedMult->VerticalSpreadDegs;
		Config->ReloadTime *= CastedMult->ReloadTime;
	}
	else
	{
		Range *= CastedMult->Range;
		MaxAmmo *= CastedMult->MaxAmmo;
		MaxMagAmmo *= CastedMult->MaxMagAmmo;
		SpreadCorrectionPercentage *= CastedMult->SpreadCorrectionPercentage;
		HorizontalSpreadDegs *= CastedMult->HorizontalSpreadDegs;
		VerticalSpreadDegs *= CastedMult->VerticalSpreadDegs;
		ReloadTime *= CastedMult->ReloadTime;
	}

	Super::ApplyMultiplier(Multiplier, bAppliesOnConfig);
}

void AFirearm::ApplyPercentage(FWeaponDTStruct* Modifier, bool bAppliesOnConfig, bool bIsConfigBased)
{
	FFiregunDTStruct* NewGunMod = (FFiregunDTStruct*)Modifier;

	if (!NewGunMod) return;

	UFirearmConfig* Config = GetCurrentConfigAs<UFirearmConfig>();

	if (!Config) return;

	UFirearmConfig* CurrentStats = Cast<UFirearmConfig>(GetCurrentStatsAsConfig());

	UFirearmConfig* Source = bIsConfigBased ? Config : CurrentStats;

	if (bAppliesOnConfig)
	{
		Config->Range += Source->Range * NewGunMod->Range / 100;
		Config->MaxAmmo += Source->MaxAmmo * NewGunMod->MaxAmmo / 100;
		Config->MaxMagAmmo += Source->MaxMagAmmo * NewGunMod->MaxMagAmmo / 100;
		Config->SpreadCorrectionPercentage += Source->SpreadCorrectionPercentage * NewGunMod->SpreadCorrectionPercentage / 100;
		Config->HorizontalSpreadDegs += Source->HorizontalSpreadDegs * NewGunMod->HorizontalSpreadDegs / 100;
		Config->VerticalSpreadDegs += Source->VerticalSpreadDegs * NewGunMod->VerticalSpreadDegs / 100;
		Config->ReloadTime += Source->ReloadTime * NewGunMod->ReloadTime / 100;
	}
	else
	{
		Range += Source->Range * NewGunMod->Range / 100;
		MaxAmmo += Source->MaxAmmo * NewGunMod->MaxAmmo / 100;
		MaxMagAmmo += Source->MaxMagAmmo * NewGunMod->MaxMagAmmo / 100;
		SpreadCorrectionPercentage += Source->SpreadCorrectionPercentage * NewGunMod->SpreadCorrectionPercentage / 100;
		HorizontalSpreadDegs += Source->HorizontalSpreadDegs * NewGunMod->HorizontalSpreadDegs / 100;
		VerticalSpreadDegs += Source->VerticalSpreadDegs * NewGunMod->VerticalSpreadDegs / 100;
		ReloadTime += Source->ReloadTime * NewGunMod->ReloadTime / 100;
	}
	
	Super::ApplyPercentage(Modifier, bAppliesOnConfig, bIsConfigBased);
}

UWeaponConfig* AFirearm::GetCurrentStatsAsConfig()
{
	UFirearmConfig* Config = NewObject<UFirearmConfig>();

	Config = Cast<UFirearmConfig>(Super::GetCurrentStatsAsConfig());

	Config->Range  = Range;
	Config->MaxAmmo  = MaxAmmo;
	Config->MaxMagAmmo  = MaxMagAmmo;
	Config->SpreadCorrectionPercentage  = SpreadCorrectionPercentage;
	Config->HorizontalSpreadDegs  = HorizontalSpreadDegs;
	Config->VerticalSpreadDegs  = VerticalSpreadDegs;
	Config->ReloadTime  = ReloadTime;

	return Config;
}

void AFirearm::SetDefaults()
{
	Super::SetDefaults();

	BeamFxTargetParameterName = "Target";

	TimeSinceLastShoot = 0.0f;

	SetSoundCue();
}


void AFirearm::SpawnWeaponEmitters(const FVector& HitPoint, const FHitResult& Hit, UParticleSystem* HitImpactFX)
{	
	UGameplayStatics::SpawnEmitterAttached(CurrentWeaponFX[(int)EWeaponFX::MuzzleFX], WeaponMesh, MuzzleSocketName);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitImpactFX, HitPoint, Hit.ImpactNormal.Rotation());

	FVector MuzzleSocketLocation = WeaponMesh->GetSocketLocation(MuzzleSocketName);

	UParticleSystemComponent* BeamFX = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CurrentWeaponFX[(int)EWeaponFX::ShootBeamFX], MuzzleSocketLocation);

	if (BeamFX != nullptr) 
		BeamFX->SetVectorParameter(BeamFxTargetParameterName, HitPoint);
}



bool AFirearm::InitWeapon(FWeaponDTStruct* WeaponData)
{
	if(!Super::InitWeapon(WeaponData)) return false;

	return true;
}

void AFirearm::AssignWeaponFX(int FxType, UParticleSystem* ParticleFX)
{
	switch (FxType)
	{
		case((int)EWeaponFX::DefaultImpactFX):
			CurrentWeaponFX[(int)EWeaponFX::DefaultImpactFX] = ParticleFX;
			break;
		case((int)EWeaponFX::TargetImpactFX):
			CurrentWeaponFX[(int)EWeaponFX::TargetImpactFX] = ParticleFX;
			break;
		case((int)EWeaponFX::MuzzleFX):
			CurrentWeaponFX[(int)EWeaponFX::MuzzleFX] = ParticleFX;
			break;
		case((int)EWeaponFX::ShootBeamFX):
			CurrentWeaponFX[(int)EWeaponFX::ShootBeamFX] = ParticleFX;
			break;
		case((int)EWeaponFX::BulletShellFX):
			CurrentWeaponFX[(int)EWeaponFX::BulletShellFX] = ParticleFX;
			break;
	}

}



void AFirearm::Attack(AActor* Target)
{
	if (GetOwner() && GetOwner()->IsA<ABasePraiseCharacter>() && Cast<ABasePraiseCharacter>(GetOwner())->GetCurrentAttackType() == EAttackType::SECONDARY_ATTACK) return;

	if (CurrentAttackHits.Contains(Target)) return;

	ABasePraiseCharacter* WeaponOwner = Cast<ABasePraiseCharacter>(GetOwner());

	if (!WeaponOwner->IsAttacking()) return;

	if (Target->Implements<UDamageable>())
	{
		IDamageable* Damageable = Cast<IDamageable>(Target);

		if (Damageable->IsDead()) return;

		Damageable->GetWeaponDamage(-GetAttackDamage(), WeaponOwner->GetCurrentAttackType(), EWeaponSlot::PRIMARY_WEAPON, WeaponOwner, this);

		CurrentAttackHits.Add(Target);
	}
}


void AFirearm::DrawDebugSphereOnImpactPoint(const FHitResult& Hit)
{
	DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 5.0f, 1, FColor::Yellow, false, 4.0f, 0.0f, 1.0f);
}

void AFirearm::Attack(bool bIsPlayerAttack)
{
	if (!HasOwner()) return;

	if (GetOwner() && GetOwner()->IsA<ABasePraiseCharacter>() && Cast<ABasePraiseCharacter>(GetOwner())->GetCurrentAttackType() != EAttackType::SECONDARY_ATTACK) return;
	
	if (bIsBroken)
	{
		PlaySFX(ECombatSFX::WEAPON_BROKEN, GetSoundSampleIdx(ECombatSFX::WEAPON_BROKEN));

		return;
	}

	if (!CanFire()) return;

	if (Ammo <= 0) { PlaySFX(ECombatSFX::WEAPON_ATTACK, DEFAULT_FIREARM_EMPTY_SFX); return; }

	FHitResult Hit;
	FVector ShootDirection;
	if (TryGetHitResultFromViewpoint(Hit, ShootDirection, bIsPlayerAttack, HorizontalSpreadDegs, VerticalSpreadDegs))
	{
		if (Hit.Actor.IsValid() && Hit.Actor->Implements<UDamageable>()) {
			if (Hit.Actor.Get()->IsA<ABasePraiseCharacter>() && GetOwner()->IsA<ABasePraiseCharacter>())
				Cast<IDamageable>(Hit.Actor.Get())->GetWeaponDamage(-GetAttackDamage(), EAttackType::PRIMARY_ATTACK, EWeaponSlot::PRIMARY_WEAPON, Cast<ABasePraiseCharacter>(GetOwner()), this);

			else Cast<IDamageable>(Hit.Actor.Get())->GetDamage(-GetAttackDamage(), this->GetOwner());

		}

		OnShootHitSpawnFX(Hit);
	}

	AddCameraShakeOnAttack();

	PlaySFX(ECombatSFX::WEAPON_ATTACK, GetSoundSampleIdx(ECombatSFX::WEAPON_ATTACK));

	Ammo--;

	ApplyWeaponStateReduction();
}


bool AFirearm::CanFire()
{
	float CurrentTime = GetGameTimeSinceCreation();

	if (TimeSinceLastShoot <= CurrentTime)
	{
		TimeSinceLastShoot = CurrentTime + AttackRate;
		return true;
	}

	return false;
}


bool AFirearm::TryGetHitResultFromViewpoint(FHitResult& Hit, FVector& HitDirection, bool bIsCameraView, float SpreadHrz, float SpreadVrt)
{
	ABasePraiseCharacter* Character = Cast<ABasePraiseCharacter>(GetOwner());

	if (Character == nullptr) return false;

	FVector ViewpointLocation;
	FRotator ViewpointRotation;

	Character->GetPlayerViewPoint(ViewpointLocation, ViewpointRotation, bIsCameraView);

	if (ViewpointLocation == FVector::ZeroVector) return false;

	FVector Direction = ViewpointRotation.Vector();
	float SpreadHrzRads = FMath::DegreesToRadians((SpreadHrz - (SpreadHrz * SpreadCorrectionPercentage / 100)) / 2);
	float SpreadVrtRads = FMath::DegreesToRadians((SpreadVrt - (SpreadVrt * SpreadCorrectionPercentage / 100)) / 2);

	HitDirection = FMath::VRandCone(Direction, SpreadHrzRads, SpreadVrtRads);
	
	FVector TraceEnd = ViewpointLocation + HitDirection * Range;
	
	return GetWorld()->LineTraceSingleByChannel(Hit, ViewpointLocation, TraceEnd, ECollisionChannel::ECC_Visibility, GetShootLineTraceQueryParams(true, {this, Character}));
}

FCollisionQueryParams AFirearm::GetShootLineTraceQueryParams(bool bIsTraceComplex, TArray<AActor*>IgnoredActors)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(IgnoredActors);
	QueryParams.bTraceComplex = bIsTraceComplex;
	QueryParams.bReturnPhysicalMaterial = true;

	return QueryParams;
}

void AFirearm::OnShootHitSpawnFX(FHitResult& Hit)
{
	UParticleSystem* HitImpactFX = CurrentWeaponFX[(int)EWeaponFX::DefaultImpactFX];

	EPhysicalSurface HittenSurface = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

	HitImpactFX = GetImpactFX(HittenSurface);

	FVector FxImpactPoint = Hit.ImpactPoint;

	SpawnWeaponEmitters(FxImpactPoint, Hit, HitImpactFX);
}
