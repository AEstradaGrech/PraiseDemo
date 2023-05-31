// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "MatineeCameraShake.h"
#include "UObject/ConstructorHelpers.h"
#include "Weapon.h"
#include "Misc/DefaultValueHelper.h"
#include "Particles/ParticleSystem.h"
#include "Components/SphereComponent.h"
#include "../Praise.h"
#include "Fist.h"
#include "../Characters/Player/PraisePlayerCharacter.h"
#include "../Structs/CommonUtility/FUtilities.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	WeaponAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Weapon Audio Comp"));
	WeaponAudioComp->AttachTo(RootComponent);

	SetRootComponent(WeaponMesh);
	WeaponsDBComp = CreateDefaultSubobject<UWeaponsDBComponent>(TEXT("Weapons DB"));

	WeaponCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Collider"));
	WeaponCollider->AttachTo(RootComponent);
	WeaponCollider->SetBoxExtent(FVector(10,10,10));
	WeaponCollider->SetVisibility(true);
	WeaponCollider->SetHiddenInGame(true);
	WeaponCollider->SetCollisionProfileName(FName("NoCollision"));
	WeaponCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	InteractionCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Interaction Collider"));
	InteractionCollider->AttachTo(WeaponCollider);
	InteractionCollider->SetSphereRadius(90.f);
	InteractionCollider->SetVisibility(true);
	InteractionCollider->SetHiddenInGame(true);
	InteractionCollider->SetCollisionProfileName(FName("NoCollision"));
	InteractionCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DamageCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Collider"));
	DamageCollider->AttachTo(RootComponent);
	DamageCollider->SetBoxExtent(FVector(10, 10, 10));
	DamageCollider->SetVisibility(true);
	DamageCollider->SetHiddenInGame(true);
	DamageCollider->SetCollisionProfileName(FName("NoCollision"));
	DamageCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	StatsConfigID = -1;
	TargetType = ETargetType::ITEM;
	ItemStatus = ECharStatus::NORMAL;

	WeaponModel = FString("Default");

	static ConstructorHelpers::FObjectFinder<USoundCue> WeaponSoundsQ(TEXT("SoundCue'/Game/Core/Audio/Combat/CombatFX.CombatFX'"));

	if (WeaponSoundsQ.Succeeded())
	{
		WeaponSoundCue = WeaponSoundsQ.Object;

		WeaponAudioComp->SetSound(WeaponSoundCue);
	}
	
}

bool AWeapon::IsDead() const
{
	return WeaponState <= 0.f;
}

ETargetType AWeapon::IsTargetType() const
{
	return TargetType;
}

ECharStatus AWeapon::GetCharStatus() const
{
	return ItemStatus;
}

int32 AWeapon::GetTargetID() const
{
	return GetUniqueID();
}

float AWeapon::GetWeaponState() const
{
	return FMath::Clamp(WeaponState, 0.f, 100.f);
}

float AWeapon::GetWeaponDurability() const
{
	return FMath::Clamp(Durability, 0.f, 100.f);
}

float AWeapon::GetWeaponStateAsRatio() const
{	
	return GetWeaponState() / 100;
}

float AWeapon::GetDurabilityAsRatio() const
{
	return GetWeaponDurability() / 100;
}

void AWeapon::ClearCurrentHits()
{
	CurrentAttackHits.Empty();
}



UWeaponConfig* AWeapon::GetCurrentStatsAsConfig()
{
	UWeaponConfig* Config = NewObject<UWeaponConfig>();

	Config->BaseXP += BaseXP;
	Config->Damage += Damage;
	Config->CritDamage += CritDamage;
	Config->CritChance += CritChance;
	Config->ChanceToBreak += ChanceToBreak;
	Config->Weight += Weight;
	Config->AttackRate += AttackRate;
	Config->WeaponState += WeaponState;
	Config->Durability += Durability;
	Config->StateReductionConst += WeaponStateReductionConstant;
	
	return Config;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	bIsBroken = WeaponState <= 0;
	SetWeaponFx();
	SetCameraShaker();
	
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StatsConfig = NewObject<UWeaponConfig>(this, FName("Weapon Config"));
}

void AWeapon::SetDefaults()
{
	WeaponState = 100.0f;
	bIsBroken = WeaponState <= 0;
}

void AWeapon::SetWeaponFx()
{
	for (int i = 0; i < WeaponFX.Num(); i++)
	{
		std::vector<std::string> splitted = FUtilities::Split(FUtilities::FStringToStdString(WeaponFX[i]->GetName()), '_');

		FLogger::LogTrace(FString("Splitted :: " + WeaponFX[i]->GetName()) + " :: Num() :: " + FUtilities::StdStringToFString(std::to_string(splitted.size())));

		std::string FxType = splitted[splitted.size() - 1];

		FLogger::LogTrace(FUtilities::StdStringToFString(FxType));

		std::vector<std::string> FxStringyCategory = FUtilities::Split(FxType, '-');

		int32 FxCategory;

		FDefaultValueHelper::ParseInt(FUtilities::StdStringToFString(FxStringyCategory[FxStringyCategory.size() - 1]), FxCategory);

		AssignWeaponFX(FxCategory, WeaponFX[i]);
	}
}

void AWeapon::AddCameraShakeOnAttack()
{
	APawn* owner = Cast<APawn>(GetOwner());

	if (owner == nullptr || CameraShaker.Get() == nullptr) return;
	
	APraisePlayerCharacter* CharController = Cast<APraisePlayerCharacter>(owner);
	
	if(CharController != nullptr)
		Cast<APlayerController>(CharController->GetController())->ClientPlayCameraShake(CameraShaker);
}

void AWeapon::SetCameraShaker()
{
	UMatineeCameraShake* CS = Cast<UMatineeCameraShake>(CameraShaker.Get());

	if (CS == nullptr) return;

	SetShakePitch(*CS);
	SetShakeYaw(*CS);
	SetShakeFOV(*CS);
}

void AWeapon::SetShakePitch(UMatineeCameraShake& CS)
{
	CS.RotOscillation.Pitch.Amplitude = 0.5f;
	CS.RotOscillation.Pitch.Frequency = 30.0f;
	CS.RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
	CS.RotOscillation.Pitch.Waveform = EOscillatorWaveform::SineWave;
}

void AWeapon::SetShakeYaw(UMatineeCameraShake& CS)
{
	CS.RotOscillation.Yaw.Amplitude = 0.5f;
	CS.RotOscillation.Yaw.Frequency = 30.0f;
	CS.RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
	CS.RotOscillation.Yaw.Waveform = EOscillatorWaveform::SineWave;
}

void AWeapon::SetShakeFOV(UMatineeCameraShake& CS)
{
	CS.FOVOscillation.Amplitude = 0.5f;
	CS.FOVOscillation.Frequency = 30.0f;
	CS.FOVOscillation.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
	CS.FOVOscillation.Waveform = EOscillatorWaveform::SineWave;
}

bool AWeapon::TryCalculateWeaponLvl()
{
	Lvl = 1;

	return true;
}

void AWeapon::SetWeaponState(float Value)
{
	WeaponState = FMath::Clamp(Value, 0.f, 100.f);

	if (WeaponState <= 50.f)
	{
		ItemStatus = ECharStatus::DAMAGED;
	}
	if (WeaponState <= 20.f)
	{
		ItemStatus = ECharStatus::VERY_DAMAGED;
	}

	if (WeaponState <= 0.f)
	{
		bIsBroken = true;
	}
}

UParticleSystem* AWeapon::GetImpactFX(EPhysicalSurface ImpactSurface)
{
	switch (ImpactSurface)
	{
		case(PHYSICS_SURFACE_FLESH_DEFAULT):
		case(PHYSICS_SURFACE_FLESH_VULNERABLE):
			return CurrentWeaponFX[(int)EWeaponFX::TargetImpactFX];
		case(PHYSICS_SURFACE_HITTABLE):
		default:
			return CurrentWeaponFX[(int)EWeaponFX::DefaultImpactFX];
	}
}

void AWeapon::AssignWeaponFX(int FxType, UParticleSystem* ParticleFX)
{
	switch (FxType)
	{
		case((int)EWeaponFX::DefaultImpactFX):
			CurrentWeaponFX[(int)EWeaponFX::DefaultImpactFX] = ParticleFX;
			break;
		case((int)EWeaponFX::TargetImpactFX):
			CurrentWeaponFX[(int)EWeaponFX::TargetImpactFX] = ParticleFX;
			break;
	}
}

float AWeapon::GetAttackDamage()
{
	float dmg = CalculateAttackDamage();

	return dmg;
}

float AWeapon::CalculateAttackDamage()
{
	float dmg = Damage + GetFloatRandomizer(Damage, 2);

	dmg += TryGetAttackCritDamage(Damage);

	dmg = dmg * GetWeaponStateFactor();

	return dmg;
}

float AWeapon::TryGetAttackCritDamage(float CurrentDamage)
{
	float dmg = 0.f;

	bool bHasChance = FMath::RandRange(0, 100) <= CritChance ? true : false;

	if (!bHasChance) return dmg;

	dmg = CurrentDamage * CritDamage / 100.f;

	float randomizer = GetFloatRandomizer(dmg, 10);

	dmg += randomizer;

	return dmg;
}

bool AWeapon::InitWeapon(FString Model, EWeaponType Type)
{
	WeaponType = Type;

	WeaponModel = Model;

	if (!TryInitStats()) return false;

	if (!(this->IsA<AFist>() && WeaponModel == FString("Default")))
		if (!TrySetupSkeletalMesh())
			return false;

	if (!TrySetWeaponFX()) return false;

	bDidInit = true;

	return bDidInit;
}

bool AWeapon::InitWeapon(FWeaponDTStruct* WeaponData)
{
	FullOverwriteStats(WeaponData);

	if(!TryAddStatsConfig()) return false;

	if(!(this->IsA<AFist>() && WeaponModel == FString("Default")))
		if (!TrySetupSkeletalMesh()) 
			return false;

	if(!TrySetWeaponFX()) return false;

	bDidInit = true;

	return bDidInit;
}

void AWeapon::EnableWeaponCollider(EAttackType AttackType, bool bEnable)
{
	
	switch (AttackType)
	{
		case(EAttackType::PRIMARY_ATTACK):

			if (!DamageCollider) return;

			HandleEnableCollider(DamageCollider, bEnable);

			if (bEnable) 
				PlaySFX(ECombatSFX::WEAPON_ATTACK, GetSoundSampleIdx(ECombatSFX::WEAPON_ATTACK), false, 0.3);

			break;
		case(EAttackType::BREAK_GUARD):

			if (!BreakGuardCollider) return;

			HandleEnableCollider(BreakGuardCollider, bEnable);

			if (bEnable) 
				PlaySFX(ECombatSFX::WEAPON_ATTACK, GetSoundSampleIdx(ECombatSFX::WEAPON_ATTACK), false, 0.3);

			break;
		case(EAttackType::SECONDARY_ATTACK):

			if (!WeaponCollider) return;

			HandleEnableCollider(WeaponCollider, bEnable);

			break;
		default:

			WeaponCollider->SetCollisionProfileName(FName("Pawn"));
			WeaponCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			WeaponCollider->SetActive(true);
			CurrentAttackHits.Empty();
			break;
	}

	
}

void AWeapon::HandleEnableCollider(UBoxComponent* Collider, bool bEnable)
{
	FName ColProfileName = bEnable ? FName("Weapon") : FName("NoCollision");
	ECollisionEnabled::Type ColType = bEnable ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;

	Collider->SetCollisionProfileName(ColProfileName);
	Collider->SetCollisionEnabled(ColType);
	Collider->SetActive(bEnable);

	if (!bEnable)
		CurrentAttackHits.Empty();
}

bool AWeapon::TrySetWeaponFX(uint32 FxID)
{
	if (FX.Num() <= 0) return true; // using default FX

	if (FxID == 0)
	{
		if (FX.Num() > 0)
			SetWeponFxByID(FX[0]);
	}
	else
	{
		for (int i = 0; i < FX.Num(); i++)
			if (FX[i] == FxID)
				SetWeponFxByID(FX[i]);
	}

	return true;
}

// First reads the base default config. stores the config as the current StatsConfig and sets the default mesh, fx abilities and modifiers (arrays)
// Then adds a configuration for that weapon model (if any) and overrides the current StatsConfig. This may override the mesh and FX. Abilities and mods might be overriden OR added to the existing default values
// Then it adds any weapon modifier by changing the values of the StatsConfig and updating the weapon stats with the result. this may override the mesh & FX etc in the same way as the configuration
// Last step is set the mesh and FX reading from the current Weapon stats (not the StatsConfig that updates the rest of the values)
// This is because the mesh and Fx are constant during gameplay but the Weapon Stats can be modified by buffs or weapon abilities, but the StatsConfig stores the original values configured when spawned 
// and they are used to restore the weapon stats values
bool AWeapon::TryInitStats()
{
	if (!TryAddBaseStats()) return false;

	if (!TryAddStatsConfig()) return false;

	if (!TryAddStatModifiers()) return false;

	ResetToCurrentConfig();

	TrySetWeaponFX();

	return true;
}

void AWeapon::GetHolsterCoords(EHolsterPosition Position, FVector& OutLoc, FRotator& OutRot)
{
	FVector Loc;
	FRotator Rot;
	TArray<EHolsterPosition> Keys;
	GetHolsterLocations().GetKeys(Keys);
	
	for (int i = 0; i < Keys.Num(); i++)
		if (Keys[i] == Position)
			Loc = GetHolsterLocations()[Keys[i]];

	Keys.Empty();
	GetHolsterRotations().GetKeys(Keys);
	for (int i = 0; i < Keys.Num(); i++) {
		if (Keys[i] == Position)
			Rot = GetHolsterRotations()[Keys[i]];
	}

	OutLoc = Loc;
	OutRot = Rot;
}

void AWeapon::OnWeaponDropped()
{
	if (this->IsA<AFist>() && WeaponModel == FString("Default")) return;

	SetOwner(nullptr);
	bIsEquiped = false;
	FVector CurrentLoc = GetActorLocation();

	FDetachmentTransformRules DetachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);
	DetachFromActor(DetachmentRules);
	WeaponMesh->SetCollisionProfileName(FName("NoCollision"));
	WeaponMesh->SetSimulatePhysics(false);

	WeaponCollider->DetachFromParent(true);
	SetRootComponent(WeaponCollider);

	WeaponMesh->AttachTo(RootComponent);
	WeaponMesh->SetRelativeLocation(WeaponColLoc * -1);
	WeaponMesh->SetRelativeRotation(FRotator(0,0,0));
	
	WeaponAudioComp->AttachTo(RootComponent);
	
	SetActorLocation(CurrentLoc);

	WeaponCollider->SetCollisionProfileName(FName("Weapon"));
	WeaponCollider->SetBoxExtent(DroppedWeaponColExtents);
	WeaponCollider->SetSimulatePhysics(true);
	WeaponCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponCollider->SetActive(true);
	WeaponCollider->SetEnableGravity(true);

	InteractionCollider->SetCollisionProfileName(FName("Interaction"));
	InteractionCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollider->SetActive(true);
	InteractionCollider->SetHiddenInGame(true);

	PlaySFX(ECombatSFX::WEAPON_DROP, GetSoundSampleIdx(ECombatSFX::WEAPON_DROP), false, 0.1);
}

void AWeapon::OnWeaponPickup(ABasePraiseCharacter* NewOwner, EWeaponSlot PickingHand)
{
	if (this->IsA<AFist>() && WeaponModel == FString("Default")) return;

	if (!NewOwner->GetMesh()) return;

	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);

	WeaponMesh->DetachFromParent(true);

	SetRootComponent(WeaponMesh);

	SetActorRelativeScale3D(GetWeaponScale());

	AttachToComponent(NewOwner->GetMesh(), AttachmentRules, PickingHand == EWeaponSlot::PRIMARY_WEAPON ? FName("Hand_R_Socket") : FName("Hand_L_Socket"));

	WeaponAudioComp->AttachTo(RootComponent);

	if (WeaponCollider)
	{
		WeaponCollider->AttachToComponent(WeaponMesh, AttachmentRules);
		WeaponCollider->SetRelativeLocation(WeaponColLoc);
		WeaponCollider->SetBoxExtent(WeaponColExtents);
		WeaponCollider->SetHiddenInGame(true);
		HandleEnableCollider(WeaponCollider, false);
	}

	if (InteractionCollider)
	{
		InteractionCollider->AttachToComponent(WeaponCollider, AttachmentRules);
		InteractionCollider->SetRelativeLocation(WeaponColLoc);
		InteractionCollider->SetSphereRadius(InteractionColRadius);
		InteractionCollider->SetCollisionProfileName(FName("NoCollision"));
		InteractionCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		InteractionCollider->SetHiddenInGame(true);
	}

	if (!DamageCollider)
	{
		DamageCollider->AttachToComponent(WeaponMesh, AttachmentRules);
		DamageCollider->SetRelativeLocation(DamageColLoc);
		DamageCollider->SetBoxExtent(DamageColExtents);

		HandleEnableCollider(DamageCollider, false);
		DamageCollider->SetHiddenInGame(true);
	}

	PlaySFX(ECombatSFX::WEAPON_PICK, GetSoundSampleIdx(ECombatSFX::WEAPON_PICK), false, 0.1);

	bIsBroken = WeaponState <= 0;
}

EHolsterPosition AWeapon::GetHolsterForHand(EWeaponSlot EquippingHand)
{
	for (int i = 0; i < AvailableHolsterPositions.Num(); i++)
	{
		switch (AvailableHolsterPositions[i])
		{
			case(EHolsterPosition::SHOULDER_R):
			case(EHolsterPosition::HOLSTER_R):
			case(EHolsterPosition::LOW_BACK_R):
				if (EquippingHand == EWeaponSlot::PRIMARY_WEAPON)
					return AvailableHolsterPositions[i];
				break;
			case(EHolsterPosition::THIGH_R):
				if (EquippingHand == EWeaponSlot::SECONDARY_WEAPON && WeaponClass == EWeaponClass::SWORD)
					return AvailableHolsterPositions[i];
				if (EquippingHand == EWeaponSlot::PRIMARY_WEAPON && WeaponClass != EWeaponClass::SWORD)
					return AvailableHolsterPositions[i];
				break;
			case(EHolsterPosition::LOW_BACK_L):
				if (EquippingHand == EWeaponSlot::SECONDARY_WEAPON)
					return AvailableHolsterPositions[i];
			case(EHolsterPosition::THIGH_L):
				if (EquippingHand == EWeaponSlot::SECONDARY_WEAPON && WeaponClass != EWeaponClass::SWORD)
					return AvailableHolsterPositions[i];
				if (EquippingHand == EWeaponSlot::PRIMARY_WEAPON && WeaponClass == EWeaponClass::SWORD)
					return AvailableHolsterPositions[i];
				break;
		}
	}

	return EHolsterPosition::UNARMED;
}

void AWeapon::ReattachAudioComp()
{
	if (!WeaponAudioComp) return;

	WeaponAudioComp->AttachTo(RootComponent);

	if(WeaponSoundCue)
		WeaponAudioComp->SetSound(WeaponSoundCue);
}

bool AWeapon::TryAddBaseStats()
{
	if (WeaponsDBComp == nullptr) { FLogger::LogTrace(__FUNCTION__ + FString(" :: NO WEAPONS DB COMP :: ")); return false; }

	if (WeaponModel == "") return false; 

	if (!StatsConfig) return false;

	FWeaponDTStruct* WeaponData = WeaponsDBComp->GetWeaponByTypeAndModel(WeaponType, WeaponModel);

	if (WeaponData == nullptr) return false;

	FullOverwriteStats(WeaponData);

	return true;
}

bool AWeapon::TryAddStatsConfig()
{
	if (StatsConfigID == -1) return true;

	if (!StatsConfig) return false;

	FWeaponDTStruct* WeaponConfig = WeaponsDBComp->GetWeaponsConfigDTByClassAndId(WeaponClass, StatsConfigID);

	if (WeaponConfig == nullptr) return false;

	if (WeaponConfig->ModifierType != STAT_CONFIG) false;

	FullOverwriteStats(WeaponConfig);

	return true;
}

bool AWeapon::TryAddStatModifiers()
{
	if (Modifiers.Num() <= 0) return true;

	FWeaponDTStruct* WeaponMod = nullptr;

	for (int i = 0; i < Modifiers.Num(); i++)
	{
		WeaponMod = WeaponsDBComp->GetWeaponModsByClassAndId(WeaponClass, Modifiers[i]);

		if (WeaponMod == nullptr) continue;

		if (WeaponMod->WeaponModel != WeaponModel)
		{
			if (WeaponMod->WeaponModel == FString("Common"))
			{
				if (WeaponMod->WeaponClass != (int)WeaponClass || WeaponMod->WeaponType != (int)WeaponType)
					continue;
			}

			else continue;
		}

		StatsConfig->CurrentMods.Add(WeaponMod->Description);

		switch (WeaponMod->ModifierType)
		{
			case(STAT_MODIFIER):
				ApplyModifier(WeaponMod, WeaponMod->AppliesOnConfig);
				break;
			case(STAT_MULTIPLIER):
				ApplyMultiplier(WeaponMod, WeaponMod->AppliesOnConfig);
				break;
			case(STAT_PERCENTAGE):
				ApplyPercentage(WeaponMod, WeaponMod->AppliesOnConfig, WeaponMod->IsConfigBased);
				break;
			default:
				break;
		}
	}

	

	return true;
}

void AWeapon::SetWeponFxByID(uint32 ID)
{
	if (ID == 0) return;

	FWeaponFxDTStruct* FxData = WeaponsDBComp->GetWeaponFxDataById(ID);

	if (FxData == nullptr) return;

	if (FxData->PathToFX.Num() > 0)
	{
		for (int i = 0; i < FxData->PathToFX.Num(); i++)
		{
			UParticleSystem* Fx = LoadObject<UParticleSystem>(NULL, *FxData->PathToFX[i], NULL, LOAD_None, NULL);

			if (Fx == nullptr) continue;

			WeaponFX.Add(Fx);
		}
	}


	for (FString Key : FxData->WeaponSFX)
	{
		TArray<FString> Splitted = FUtilities::Split(Key, '-');
		
		int64 EnumVal = FUtilities::StringToEnumValue(TEXT("ECombatSFX"), Splitted[0]);

		if (EnumVal < 0) continue;

		ECombatSFX SFX = (ECombatSFX)EnumVal;

		TArray<int> Samples;
		int32 ParsedValue = 0;
		
		for (int i = 1; i < Splitted.Num(); i++)
			if (FDefaultValueHelper::ParseInt(Splitted[i], ParsedValue))
				Samples.Add(ParsedValue);

		if (!WeaponSFX.Contains(SFX))
			WeaponSFX.Add(SFX, Samples);
	}
		
}

bool AWeapon::TrySetupSkeletalMesh(FWeaponArtDTStruct* ArtData)
{
	if (ArtData == nullptr)
	{
		if (CurrentSkin == nullptr && WeaponSkins.Num() <= 0) return false;

		if (CurrentSkin != nullptr) return TrySetupSkeletalMesh(CurrentSkin);

		if (WeaponSkins.Num() <= 0) return false;

		FWeaponArtDTStruct* Data = WeaponsDBComp->GetWeaponArtDataById(WeaponSkins[0]);

		if (Data == nullptr) return false;

		CurrentSkin = Data;

		return TrySetupSkeletalMesh(CurrentSkin);
	}

	CurrentSkin = ArtData;

	if (CurrentSkin == nullptr && ArtData == nullptr) return false;

	CurrentSkin = ArtData;
	EquipLocationR = ArtData->EquipLocationR;
	EquipRotationR = ArtData->EquipRotationR;
	EquipLocationL = ArtData->EquipLocationL;
	EquipRotationL = ArtData->EquipRotationL;
	BlockLocation = ArtData->BlockLocation;
	BlockRotation = ArtData->BlockRotation;
	WeaponScale = ArtData->MeshScale;

	if (!TrySetSkeletalMeshFromPath(CurrentSkin->PathToMesh)) return false;

	if (WeaponType != EWeaponType::UNARMED) 
		if (!TrySetupWeaponColliders(CurrentSkin))
			return false;

	return true;
}

bool AWeapon::TrySetSkeletalMeshFromPath(FString Path)
{
	if (CurrentSkin == nullptr) return false;

	USkeletalMesh* Mesh = nullptr;

	UObject* Obj = LoadObject<USkeletalMesh>(NULL, *Path, NULL, LOAD_None, NULL);

	if (Obj == nullptr) return false;

	Mesh = Cast<USkeletalMesh>(Obj);

	if (Mesh == nullptr) return false;

	WeaponMesh->SetSkeletalMesh(Mesh);

	return true;
}

bool AWeapon::TrySetMeshMatsFromPath(FString Path, int idx, bool bOverwrite)
{
	UObject* Obj = LoadObject<UMaterial>(NULL, *Path, NULL, LOAD_None, NULL);

	if (Obj == nullptr) return false;

	UMaterial* Mat = Cast<UMaterial>(Obj);

	if (Mat == nullptr) return false;

	UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(Mat, this);

	if (DynMat == nullptr) return false;

	int index = bOverwrite ? idx : WeaponMesh->GetMaterials().Num();

	WeaponMesh->SetMaterial(index, DynMat);

	return true;
}

bool AWeapon::TrySetMeshMatsFromPathArray(TArray<FString> Paths)
{
	for (int i = 0; i < Paths.Num(); i++)
		if (!TrySetMeshMatsFromPath(Paths[i], i))
			return false;

	return true;
}

bool AWeapon::TrySetupWeaponColliders(FWeaponArtDTStruct* ArtData)
{
	if (ArtData == nullptr)
	{
		if (CurrentSkin == nullptr && WeaponSkins.Num() <= 0) return false;

		if (CurrentSkin != nullptr) return TrySetupWeaponColliders(CurrentSkin);

		if (WeaponSkins.Num() <= 0) return false;

		FWeaponArtDTStruct* Data = WeaponsDBComp->GetWeaponArtDataById(WeaponSkins[0]);

		if (Data == nullptr) return false;

		CurrentSkin = Data;

		return TrySetupWeaponColliders(CurrentSkin);
	}

	CurrentSkin = ArtData;
	WeaponColLoc = ArtData->WeaponBoxLocation;
	InteractionColRadius = ArtData->InteractionColRadius;

	if (InteractionColRadius <= 0.f)
		InteractionColRadius = 200.f;

	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);

	if (!InteractionCollider) return false;

	InteractionCollider->AttachToComponent(WeaponCollider, AttachmentRules);
	InteractionCollider->SetRelativeLocation(ArtData->WeaponBoxLocation);
	InteractionCollider->SetSphereRadius(InteractionColRadius);
	InteractionCollider->SetCollisionProfileName(FName("NoCollision"));
	InteractionCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InteractionCollider->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnInteractionColBeginOverlap);
	InteractionCollider->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnInteractionColEndOverlap);

	if (!WeaponCollider) return false;

	WeaponCollider->AttachToComponent(WeaponMesh, AttachmentRules);
	WeaponCollider->SetRelativeLocation(ArtData->WeaponBoxLocation);
	WeaponCollider->SetBoxExtent(ArtData->WeaponBoxExtents);
	WeaponCollider->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnWeaponBoxBeginOverlap);
	WeaponColExtents = ArtData->WeaponBoxExtents;
	DroppedWeaponColExtents = ArtData->DroppedWeaponBoxExtents;
	HandleEnableCollider(WeaponCollider, false);

	if (!DamageCollider) return false;	
	
	DamageCollider->AttachToComponent(WeaponMesh, AttachmentRules);
	DamageCollider->SetRelativeLocation(ArtData->DamageBoxLocation);
	DamageCollider->SetBoxExtent(ArtData->DamageBoxExtents);
	DamageCollider->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnAttackBoxBeginOverlap);
	DamageColLoc = ArtData->DamageBoxLocation;
	DamageColExtents = ArtData->DamageBoxLocation;
	HandleEnableCollider(DamageCollider, false);

	WeaponCollider->SetHiddenInGame(true);
	return true;
}

void AWeapon::UpdateStatsFromConfig(FWeaponDTStruct* Config, bool bIsAdditive, bool bAppliesOnConfig)
{
	switch (bIsAdditive)
	{
		case(true):
			PlusStats(Config, bAppliesOnConfig);
			break;
		case(false):
			OverwriteStats(Config, bAppliesOnConfig);
			break;
	}
}

void AWeapon::ResetToCurrentConfig()
{
	BaseXP = StatsConfig->BaseXP;
	Damage = StatsConfig->Damage;
	CritDamage = StatsConfig->CritDamage;
	CritChance = StatsConfig->CritChance;
	ChanceToBreak = StatsConfig->ChanceToBreak;
	Weight = StatsConfig->Weight;
	AttackRate = StatsConfig->AttackRate;
	WeaponState = StatsConfig->WeaponState;
	Durability = StatsConfig->Durability;
	WeaponStateReductionConstant = StatsConfig->StateReductionConst;

	CurrentMods = StatsConfig->CurrentMods;
}

void AWeapon::ApplyModifier(FWeaponDTStruct* Modifier, bool bAppliesOnConfig)
{
	if (bAppliesOnConfig)
	{
		StatsConfig->BaseXP += Modifier->BaseXP;
		StatsConfig->Damage += Modifier->Damage;
		StatsConfig->CritDamage += Modifier->CritDamage;
		StatsConfig->CritChance += Modifier->CritChance;
		StatsConfig->ChanceToBreak += Modifier->ChanceToBreak;
		StatsConfig->Weight += Modifier->Weight;
		StatsConfig->AttackRate += Modifier->AttackRate;
		StatsConfig->WeaponState += Modifier->WeaponState;
		StatsConfig->Durability += Modifier->Durability;
		StatsConfig->StateReductionConst += Modifier->StateReductionConst;
	}
	else
	{

		BaseXP += Modifier->BaseXP;
		Damage += Modifier->Damage;
		CritDamage += Modifier->CritDamage;
		CritChance += Modifier->CritChance;
		ChanceToBreak += Modifier->ChanceToBreak;
		Weight += Modifier->Weight;
		AttackRate += Modifier->AttackRate;
		WeaponState += Modifier->WeaponState;
		Durability += Modifier->Durability;
		WeaponStateReductionConstant += Modifier->StateReductionConst;
	}
	
}

void AWeapon::ApplyMultiplier(FWeaponDTStruct* Multiplier, bool bAppliesOnConfig)
{
	if (bAppliesOnConfig)
	{
		StatsConfig->BaseXP *= Multiplier->BaseXP;
		StatsConfig->Damage *= Multiplier->Damage;
		StatsConfig->CritDamage *= Multiplier->CritDamage;
		StatsConfig->CritChance *= Multiplier->CritChance;
		StatsConfig->ChanceToBreak *= Multiplier->ChanceToBreak;
		StatsConfig->Weight *= Multiplier->Weight;
		StatsConfig->AttackRate *= Multiplier->AttackRate;
		StatsConfig->WeaponState *= Multiplier->WeaponState;
		StatsConfig->Durability *= Multiplier->Durability;
		StatsConfig->StateReductionConst *= Multiplier->StateReductionConst;
	}
	else
	{
		BaseXP *= Multiplier->BaseXP;
		Damage *= Multiplier->Damage;
		CritDamage *= Multiplier->CritDamage;
		CritChance *= Multiplier->CritChance;
		ChanceToBreak *= Multiplier->ChanceToBreak;
		Weight *= Multiplier->Weight;
		AttackRate *= Multiplier->AttackRate;
		WeaponState *= Multiplier->WeaponState;
		Durability *= Multiplier->Durability;
		WeaponStateReductionConstant *= Multiplier->StateReductionConst;
	}
	
}

void AWeapon::ApplyPercentage(FWeaponDTStruct* Modifier, bool bAppliesOnConfig, bool bIsConfigBased)
{
	UWeaponConfig* Source = bIsConfigBased ? StatsConfig : GetCurrentStatsAsConfig();

	if (bAppliesOnConfig)
	{
		StatsConfig->BaseXP += Source->BaseXP * Modifier->BaseXP / 100;
		StatsConfig->Damage += Source->Damage * Modifier->Damage / 100;
		StatsConfig->CritDamage += Source->CritDamage * Modifier->CritDamage / 100;
		StatsConfig->CritChance += Source->CritChance * Modifier->CritChance / 100;
		StatsConfig->ChanceToBreak += Source->ChanceToBreak * Modifier->ChanceToBreak / 100;
		StatsConfig->Weight += Source->Weight * Modifier->Weight / 100;
		StatsConfig->AttackRate += Source->AttackRate * Modifier->AttackRate / 100;
		StatsConfig->WeaponState += Source->WeaponState * Modifier->WeaponState / 100;
		StatsConfig->Durability += Source->Durability * Modifier->Durability / 100;
		StatsConfig->StateReductionConst += Source->StateReductionConst * Modifier->StateReductionConst / 100;
	}
	else
	{
		BaseXP += Source->BaseXP * Modifier->BaseXP / 100;
		Damage += Source->Damage * Modifier->Damage / 100;
		CritDamage += Source->CritDamage * Modifier->CritDamage / 100;
		CritChance += Source->CritChance * Modifier->CritChance / 100;
		ChanceToBreak += Source->ChanceToBreak * Modifier->ChanceToBreak / 100;
		Weight += Source->Weight * Modifier->Weight / 100;
		AttackRate += Source->AttackRate * Modifier->AttackRate / 100;
		WeaponState += Source->WeaponState * Modifier->WeaponState / 100;
		Durability += Source->Durability * Modifier->Durability / 100;

		WeaponStateReductionConstant += Source->StateReductionConst * Modifier->StateReductionConst / 100;
	}
}


void AWeapon::FullOverwriteStats(FWeaponDTStruct* NewStats)
{
	if (!StatsConfig) return;

	StatsConfig->UpdateConfig(NewStats);

	WeaponType = (EWeaponType)NewStats->WeaponType;
	WeaponClass = (EWeaponClass)NewStats->WeaponClass;
	WeaponModel = NewStats->WeaponModel;
	ArmedPose = NewStats->ArmedPose;
	Description = NewStats->Description;
	Lvl = NewStats->Lvl;
	BaseXP = NewStats->BaseXP;
	Damage = NewStats->Damage;
	Weight = NewStats->Weight;
	AttackRate = NewStats->AttackRate;
	WeaponState = NewStats->WeaponState;
	Durability = NewStats->Durability;
	CritDamage = NewStats->CritDamage;
	CritChance = NewStats->CritChance;
	ChanceToBreak = NewStats->ChanceToBreak;
	WeaponStateReductionConstant = NewStats->StateReductionConst;
	MeleeDistanceOffset = NewStats->MeleeDistanceOffset;
	
	bCanEquipLeftOnly = NewStats->CanEquipLeftHandOnly;
	bIsTwoHand = NewStats->IsTwoHand;

	SetupCurrentStatsArrays(NewStats, true);
}

void AWeapon::PlusStats(FWeaponDTStruct* Addition, bool bAppliesOnConfig)
{
	switch (bAppliesOnConfig)
	{
		case(false):

			BaseXP = BaseXP += Addition->BaseXP;
			Damage = Damage += Addition->Damage;
			CritDamage = CritDamage += Addition->CritDamage;
			CritChance = CritChance += Addition->CritChance;
			Weight = Weight += Addition->Weight;
			AttackRate = AttackRate += Addition->AttackRate;
			WeaponState = WeaponState += Addition->WeaponState;
			Durability = Durability += Addition->Durability;
			ChanceToBreak = ChanceToBreak += Addition->ChanceToBreak;
			WeaponStateReductionConstant = WeaponStateReductionConstant += Addition->StateReductionConst;
			
			SetupCurrentStatsArrays(Addition, /*bIsAdditive:*/ true);

			break;
		case(true):

			StatsConfig->BaseXP = StatsConfig->BaseXP += Addition->BaseXP;
			StatsConfig->Damage = StatsConfig->Damage += Addition->Damage;
			StatsConfig->CritDamage = StatsConfig->CritDamage += Addition->CritDamage;
			StatsConfig->CritChance = StatsConfig->CritChance += Addition->CritChance;
			StatsConfig->Weight = StatsConfig->Weight += StatsConfig->Weight;
			StatsConfig->AttackRate = StatsConfig->AttackRate += Addition->AttackRate;
			StatsConfig->WeaponState = StatsConfig->WeaponState += Addition->WeaponState;
			StatsConfig->Durability = StatsConfig->Durability += Addition->Durability;
			StatsConfig->ChanceToBreak = StatsConfig->ChanceToBreak += Addition->ChanceToBreak;
			StatsConfig->StateReductionConst = StatsConfig->StateReductionConst += Addition->StateReductionConst;

			break;
	}
}

void AWeapon::OverwriteStats(FWeaponDTStruct* NewStats, bool bAppliesOnConfig)
{
	switch (bAppliesOnConfig)
	{
		case(false):
			//Lvl = NewStats->Lvl;
			BaseXP = NewStats->BaseXP;
			Damage = NewStats->Damage;
			CritDamage = NewStats->CritDamage;
			CritChance = NewStats->CritChance;
			ChanceToBreak = NewStats->ChanceToBreak;
			Weight = NewStats->Weight;
			AttackRate = NewStats->AttackRate;
			WeaponState = NewStats->WeaponState;
			Durability = NewStats->Durability;
			WeaponStateReductionConstant = NewStats->StateReductionConst;

			SetupCurrentStatsArrays(NewStats, /*bIsAdditive:*/ false);

			break;
		case(true):

			//StatsConfig->Lvl = NewStats->Lvl;
			StatsConfig->BaseXP = NewStats->BaseXP;
			StatsConfig->Damage = NewStats->Damage;
			StatsConfig->CritDamage = NewStats->CritDamage;
			StatsConfig->CritChance = NewStats->CritChance;
			StatsConfig->ChanceToBreak = NewStats->ChanceToBreak;
			StatsConfig->Weight = NewStats->Weight;
			StatsConfig->AttackRate = NewStats->AttackRate;
			StatsConfig->WeaponState = NewStats->WeaponState;
			StatsConfig->Durability = NewStats->Durability;
			StatsConfig->StateReductionConst = NewStats->StateReductionConst;

			break;
		}
}

// by default a Configuration will keep the same skins, abilities, fx or modifications previously added
// a Configuration with a value of '-1' in any array means that it is a destructive configuration
// and will override any array even being an additive process
// this is because a player might buy a weapon model (config) that has another skin | soundFX | abilities | modifiers
// and in that case it is a full new weapon and all those properties should be overwritten
// but a player might buy an upgrade (configuration) that updates some stats but keeps the mods , skin, fx etc from the base weapon
// in both cases the available holsters should be never empty (otherwise the player would not be able to equip that weapon)
void AWeapon::SetupCurrentStatsArrays(FWeaponDTStruct* Config, bool bIsAdditive)
{
	switch (bIsAdditive)
	{
		case(true):
			if (Config->Modifiers.Num() > 0) 
			{
				if (Config->Modifiers[0] == -1)
				{
					Modifiers.Empty();
				}
				else
				{
					for (int i = 0; i < Config->Modifiers.Num(); i++)
						if(!Modifiers.Contains(Config->Modifiers[i]))
							Modifiers.Add(Config->Modifiers[i]);
				}	
			}
				

			if (Config->WeaponAbilities.Num() > 0)
			{
				if (Config->WeaponAbilities[0] == -1)
				{
					WeaponAbilities.Empty();
				}
				else
				{
					for (int i = 0; i < Config->WeaponAbilities.Num(); i++)
						if(WeaponAbilities.Contains(Config->WeaponAbilities[i]))
							WeaponAbilities.Add(Config->WeaponAbilities[i]);
				}
			}
				

			if (Config->WeaponSkins.Num() > 0)
			{
				if (Config->WeaponSkins[0] == -1)
				{
					WeaponSkins.Empty();
				}
				else
				{
					for (int i = 0; i < Config->WeaponSkins.Num(); i++)
						if(!WeaponSkins.Contains(Config->WeaponSkins[i]))
							WeaponSkins.Add(Config->WeaponSkins[i]);
				}
			}
				
			if (Config->WeaponFX.Num() > 0)
			{
				if (Config->WeaponFX.Num() == -1)
				{
					WeaponFX.Empty();
				}
				else
				{
					for (int i = 0; i < Config->WeaponFX.Num(); i++)
						if(!FX.Contains(Config->WeaponFX[i]))
							FX.Add(Config->WeaponFX[i]);
				}
			}

			if (Config->HolsterPositions.Num() > 0)
			{
				for (int i = 0; i < Config->HolsterPositions.Num(); i++)
					if (!AvailableHolsterPositions.Contains(Config->HolsterPositions[i]))
						AvailableHolsterPositions.Add(Config->HolsterPositions[i]);
			}

			break;
		case(false):

			if (Config->Modifiers.Num() > 0)
			{
				if (Config->Modifiers[0] == -1)
					Modifiers.Empty();

				else Modifiers = Config->Modifiers;
			}

			if (Config->WeaponAbilities.Num() > 0)
			{
				if (Config->WeaponAbilities[0] == -1)
					WeaponAbilities.Empty();

				else WeaponAbilities = Config->WeaponAbilities;
			}
			
			if(Config->WeaponSkins.Num() > 0)
				WeaponSkins = Config->WeaponSkins;

			if(Config->WeaponFX.Num() > 0)
				FX = Config->WeaponFX;

			
			if (Config->HolsterPositions.Num() > 0)
				AvailableHolsterPositions = Config->HolsterPositions;

			break;
	}
}

void AWeapon::Equip(bool bEquip, FVector SocketLoc, FRotator SocketRot, bool bPlaySFX)
{
	bIsEquiped = bEquip;

	SetActorRelativeLocation(SocketLoc);
	SetActorRelativeRotation(SocketRot);
	SetActorRelativeScale3D(WeaponScale);

	if (bPlaySFX)
	{
		ECombatSFX SFX = bEquip ? ECombatSFX::WEAPON_EQUIP : ECombatSFX::WEAPON_UNEQUIP;

		PlaySFX(SFX, GetSoundSampleIdx(SFX), false, 0.2f);
	}
}	

void AWeapon::OnInteractionColBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasOwner()) return;

	if (OtherActor->IsA<APraisePlayerCharacter>())
		InteractionCollider->SetHiddenInGame(false);
}

void AWeapon::OnInteractionColEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasOwner()) return;

	if(OtherActor->IsA<APraisePlayerCharacter>())
		InteractionCollider->SetHiddenInGame(true);
}

float AWeapon::GetWeaponStateFactor() const
{
	float factor = (FMath::Sqrt(GetDurabilityAsRatio()) + FMath::Square(GetWeaponStateAsRatio() * 1.25f) * 1.25f) / 2;

	return factor;
}

float AWeapon::GetWeaponStateReduction() const
{
	float Value = 1 / (FMath::Square((GetDurabilityAsRatio() + GetWeaponStateAsRatio()) / 2));

	return  (Value / 10) * WeaponStateReductionConstant;
}

void AWeapon::ApplyWeaponStateReduction()
{
	SetWeaponState(WeaponState - GetWeaponStateReduction());
}
// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::PlaySFX(ECombatSFX SFX, int SampleIdx, bool bLoopSound, float PitchVariationRange)
{
	if (!WeaponSoundCue) return;

	if (!WeaponAudioComp) return;

	if (SFX == ECombatSFX::NONE) return;

	if (PitchVariationRange > 1)
		PitchVariationRange = 0.9;

	if (PitchVariationRange < 0)
		PitchVariationRange = 0.1;

	float Range = FMath::RandRange(-PitchVariationRange, PitchVariationRange);
	float PitchMultiplier = 1 + PitchVariationRange;

	WeaponAudioComp->SetPitchMultiplier(PitchMultiplier);
	WeaponAudioComp->SetIntParameter(FName("CombatFX"), (int)SFX);
	
	FName SubParamName = FUtilities::GetCombatSFXParamName(SFX);
	
	if(!SubParamName.IsNone())
		WeaponAudioComp->SetIntParameter(SubParamName, SampleIdx);

	WeaponAudioComp->Play();
}

int AWeapon::GetSoundSampleIdx(ECombatSFX SFX)
{
	return WeaponSFX.Contains(SFX) && WeaponSFX[SFX].Num() > 0 ? WeaponSFX[SFX][FMath::RandRange(0, WeaponSFX[SFX].Num() - 1)] : 0;
}

void AWeapon::OnWeaponBoxBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (GetOwner() && GetOwner()->IsA<ABasePraiseCharacter>())
	{
		ABasePraiseCharacter* WeaponOwner = Cast<ABasePraiseCharacter>(GetOwner());


		if (WeaponOwner->IsBlocking())
		{
			//TODO
		}

		if (WeaponOwner->GetCurrentAttackType() == EAttackType::PARRY)
		{
			//if(HasParried())
			//TODO
		}
	}
	if (OtherActor->Implements<UDamageable>())
	{

	}
}

void AWeapon::OnAttackBoxBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (GetOwner() && GetOwner()->IsA<ABasePraiseCharacter>())
	{
		if (OtherActor == GetOwner()) return;
		
		ABasePraiseCharacter* WeaponOwner = Cast<ABasePraiseCharacter>(GetOwner());

		if (OtherActor->Implements<UFactioneable>() && !Cast<IFactioneable>(WeaponOwner)->IsEnemyTarget(Cast<IFactioneable>(OtherActor))) return;

		if (OtherActor->Implements<UDamageable>())
		{
			IDamageable* Damageable = Cast<IDamageable>(OtherActor);

			if (Damageable->IsDead()) return;

			if (WeaponOwner->GetCurrentAttackType() == EAttackType::PRIMARY_ATTACK || WeaponOwner->GetCurrentAttackType() == EAttackType::SECONDARY_ATTACK) 
			{
				Attack(OtherActor);
				return;
			}

			if (WeaponOwner->GetCurrentAttackType() == EAttackType::BREAK_GUARD) 
			{
				Attack(OtherActor);
				return;
			}
		}
		
	}
	
}

float AWeapon::GetFloatRandomizer(float Value, float percentage, bool bOnlyPositive)
{
	float perc = FMath::RandRange(0.f, percentage);

	int mult = bOnlyPositive ? 1 : (FMath::RoundToInt(FMath::RandRange(1, 2)) %  2) ==  0 ? 1 : -1;

	float value = Value * (perc * mult) / 100;

	return value;
}



