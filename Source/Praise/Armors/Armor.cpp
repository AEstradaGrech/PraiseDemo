// Fill out your copyright notice in the Description page of Project Settings.


#include "Armor.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "../Components/Actor/DbComponents/ArmorsDBComponent.h"
#include "../Characters/BasePraiseCharacter.h"
#include "../Praise.h"

// Sets default values
AArmor::AArmor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ArmorsDB = CreateDefaultSubobject<UArmorsDBComponent>(TEXT("Armors DB"));
	DroppedArmorCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Dropped Armor Collider"));
	InteractionCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Interaction Collider"));

	DroppedArmorCollider->AttachTo(RootComponent);
	DroppedArmorCollider->SetBoxExtent(FVector(10, 10, 10));
	DroppedArmorCollider->SetVisibility(true);
	DroppedArmorCollider->SetHiddenInGame(true);
	DroppedArmorCollider->SetCollisionProfileName(FName("NoCollision"));
	DroppedArmorCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
	InteractionCollider->AttachTo(RootComponent);
	InteractionCollider->SetSphereRadius(90.f);
	InteractionCollider->SetVisibility(true);
	InteractionCollider->SetHiddenInGame(true);
	InteractionCollider->SetCollisionProfileName(FName("NoCollision"));
	InteractionCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StatsConfigID = -1;
	TargetType = ETargetType::ITEM;
	ItemStatus = ECharStatus::NORMAL;

	ArmorModel = FString("Default");
}

void AArmor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StatsConfig = NewObject<UArmorConfig>(this, FName("Armor Config"));
}

// Called when the game starts or when spawned
void AArmor::BeginPlay()
{
	Super::BeginPlay();
	
	InteractionColRadius = 200.f;
}

void AArmor::ResetToCurrentConfig()
{
	BaseXP = StatsConfig->BaseXP;
	Absorption = StatsConfig->Absorption;
	ChanceToBreak = StatsConfig->ChanceToBreak;
	Weight = StatsConfig->Weight;
	ArmorState = StatsConfig->ArmorState;
	Durability = StatsConfig->Durability;
	StateReductionConst = StatsConfig->StateReductionConst;
	RemainingDmgReductionMult = StatsConfig->RemainingDmgReductionMult;
}


float AArmor::GetCurrentAbsorption()
{
	return Absorption * GetArmorStateFactor();
}

float AArmor::GetArmorState() const
{
	return FMath::Clamp(ArmorState, 0.f, 100.f);
}

float AArmor::GetArmorDurability() const
{
	return FMath::Clamp(Durability, 0.f, 100.f);
}

float AArmor::GetArmorStateAsRatio() const
{
	return ArmorState / 100;
}

float AArmor::GetDurabilityAsRatio() const
{
	return Durability / 100;
}

bool AArmor::TryInitStats()
{
	if (!TryAddBaseStats()) return false;

	if (!TryAddStatsConfig()) return false;

	if (!TryAddStatModifiers()) return false;

	ResetToCurrentConfig();

	if (!TrySetupArmorArt()) return false;

	return true;
}

bool AArmor::TrySetupArmorArt()
{
	if (ArmorSkinsID > 0)
	{
		FArmorArtDTStruct* ArtData = ArmorsDB->GetArmorArtByID(ArmorSkinsID);

		if (!ArtData) return false;

		if (ArtData->PathToMeshes.Num() <= 0) return false;

		PathToMesh = ArtData->PathToMeshes[FMath::RandRange(0, ArtData->PathToMeshes.Num() - 1)];

		PathToDroppedArmorMesh = ArtData->DroppedArmorMesh;

		DroppedMeshScale = ArtData->DroppedMeshScale;

		DisplayName = ArtData->DisplayName;

		TrySetupArmorColliders(ArtData);
	}
	return true;
}

bool AArmor::InitArmor(EArmorType Type, FString Model)
{
	ArmorType = Type;
	ArmorModel = Model;

	return TryInitStats();
}

bool AArmor::TryAddBaseStats()
{
	if (!ArmorsDB) return false;

	if (ArmorModel == "") return false;

	if (!StatsConfig) return false;

	FArmorsDTStruct* ArmorData = ArmorsDB->GetArmorByTypeAndModel(ArmorType, ArmorModel);

	if (ArmorData == nullptr) return false;

	FullOverwriteStats(ArmorData);

	return true;
}

bool AArmor::TryAddStatsConfig()
{
	if (StatsConfigID == -1) return true;

	if (!StatsConfig) return false;

	FArmorsDTStruct* ArmorConfig = ArmorsDB->GetArmorConfigByID(StatsConfigID);

	if (ArmorConfig == nullptr) return false;

	if (ArmorConfig->ModifierType != STAT_CONFIG) false;

	FullOverwriteStats(ArmorConfig);

	return true;
}

bool AArmor::TryAddStatModifiers()
{
	if (ArmorModifiers.Num() <= 0) return true;

	FArmorsDTStruct* ArmorMod = nullptr;

	for (int i = 0; i < ArmorModifiers.Num(); i++)
	{
		ArmorMod = ArmorsDB->GetArmorModByID(ArmorModifiers[i]);

		if (ArmorMod == nullptr) continue;

		switch (ArmorMod->ModifierType)
		{
		case(STAT_MODIFIER):
			ApplyModifier(ArmorMod);
			break;
		case(STAT_MULTIPLIER):
			ApplyMultiplier(ArmorMod);
			break;
		case(STAT_PERCENTAGE):
			ApplyPercentage(ArmorMod);
			break;
		default:
			break;
		}
	}

	return true;
}


bool AArmor::TrySetupArmorColliders(FArmorArtDTStruct* ArtData)
{
	CurrentSkin = ArtData;

	if (!InteractionCollider) return false;

	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);

	InteractionCollider->AttachToComponent(RootComponent, AttachmentRules);
	InteractionCollider->SetSphereRadius(InteractionColRadius);
	InteractionCollider->SetCollisionProfileName(FName("NoCollision"));
	InteractionCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InteractionCollider->OnComponentBeginOverlap.AddDynamic(this, &AArmor::OnInteractionColBeginOverlap);
	InteractionCollider->OnComponentEndOverlap.AddDynamic(this, &AArmor::OnInteractionColEndOverlap);
	
	if (!DroppedArmorCollider) return false;

	DroppedArmorCollider->AttachToComponent(RootComponent, AttachmentRules);
	DroppedArmorBoxExtents = ArtData->DroppedArmorBoxExtents;
	DroppedArmorCollider->SetBoxExtent(DroppedArmorBoxExtents);
	
	HandleEnableCollider(DroppedArmorCollider, false);


	return true;
}

void AArmor::UpdateStatsFromConfig(FArmorsDTStruct* Config, bool bIsAdditive, bool bAppliesOnConfig)
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

void AArmor::ApplyModifier(FArmorsDTStruct* Modifier, bool bAppliesOnConfig)
{
	UArmorConfig* NewConfig = bAppliesOnConfig ? StatsConfig : GetCurrentStatsAsConfig();

	NewConfig->BaseXP += Modifier->BaseXP;
	NewConfig->Absorption += Modifier->Absorption;
	NewConfig->ChanceToBreak += Modifier->ChanceToBreak;
	NewConfig->Weight += Modifier->Weight;
	NewConfig->ArmorState += Modifier->ArmorState;
	NewConfig->Durability += Modifier->Durability;
	NewConfig->StateReductionConst += Modifier->StateReductionConst;
}

void AArmor::ApplyMultiplier(FArmorsDTStruct* Multiplier, bool bAppliesOnConfig)
{
	UArmorConfig* NewConfig = bAppliesOnConfig ? StatsConfig : GetCurrentStatsAsConfig();

	NewConfig->BaseXP *= Multiplier->BaseXP;
	NewConfig->Absorption *= Multiplier->Absorption;
	NewConfig->ChanceToBreak *= Multiplier->ChanceToBreak;
	NewConfig->Weight *= Multiplier->Weight;
	NewConfig->ArmorState *= Multiplier->ArmorState;
	NewConfig->Durability *= Multiplier->Durability;
	NewConfig->StateReductionConst *= Multiplier->StateReductionConst;
}

void AArmor::ApplyPercentage(FArmorsDTStruct* Modifier, bool bAppliesOnConfig, bool bIsConfigBased)
{
	UArmorConfig* NewConfig = bAppliesOnConfig ? StatsConfig : GetCurrentStatsAsConfig();
	UArmorConfig* Source = bIsConfigBased ? StatsConfig : GetCurrentStatsAsConfig();

	NewConfig->BaseXP += Source->BaseXP * Modifier->BaseXP / 100;
	NewConfig->Absorption += Source->Absorption * Modifier->Absorption / 100;
	
	NewConfig->ChanceToBreak += Source->ChanceToBreak * Modifier->ChanceToBreak / 100;
	NewConfig->Weight += Source->Weight * Modifier->Weight / 100;
	NewConfig->ArmorState += Source->ArmorState * Modifier->ArmorState / 100;
	NewConfig->Durability += Source->Durability * Modifier->Durability / 100;
	NewConfig->StateReductionConst += Source->StateReductionConst * Modifier->StateReductionConst / 100;

	
}


void AArmor::FullOverwriteStats(FArmorsDTStruct* NewStats)
{
	ArmorType = (EArmorType)NewStats->ArmorType;
	ArmorModel = NewStats->ArmorModel;
	Description = NewStats->Description;
	Lvl = NewStats->Lvl;
	BaseXP = NewStats->BaseXP;
	Absorption = NewStats->Absorption;
	Weight = NewStats->Weight;
	ArmorState = NewStats->ArmorState;
	Durability = NewStats->Durability;
	ChanceToBreak = NewStats->ChanceToBreak;
	bIsFullSet = NewStats->IsFullSet;
	StateReductionConst = NewStats->StateReductionConst;
	RemainingDmgReductionMult = NewStats->RemainingDmgReductionMult;

	ArmorSkinsID = NewStats->ArmorSkinsID > 0 ? NewStats->ArmorSkinsID : ArmorSkinsID;

	StatsConfig->UpdateConfig(NewStats);

	SetupCurrentStatsArrays(NewStats, false);
}

void AArmor::PlusStats(FArmorsDTStruct* Addition, bool bAppliesOnConfig)
{
	switch (bAppliesOnConfig)
	{
	case(false):

		BaseXP += Addition->BaseXP;
		Absorption += Addition->Absorption;
		Weight += Addition->Weight;
		ArmorState += Addition->ArmorState;
		Durability += Addition->Durability;
		ChanceToBreak += Addition->ChanceToBreak;
		StateReductionConst += Addition->StateReductionConst;
		RemainingDmgReductionMult += Addition->RemainingDmgReductionMult;
		SetupCurrentStatsArrays(Addition, true);

		break;
	case(true):

		StatsConfig->BaseXP += Addition->BaseXP;
		StatsConfig->Absorption += Addition->Absorption;
		StatsConfig->Weight += Addition->Weight;
		StatsConfig->ArmorState += Addition->ArmorState;
		StatsConfig->Durability += Addition->Durability;
		StatsConfig->ChanceToBreak += Addition->ChanceToBreak;
		StatsConfig->StateReductionConst += Addition->StateReductionConst;
		StatsConfig->RemainingDmgReductionMult += Addition->RemainingDmgReductionMult;
		break;
	}
}

void AArmor::OverwriteStats(FArmorsDTStruct* NewStats, bool bAppliesOnConfig)
{
	switch (bAppliesOnConfig)
	{
	case(false):
		Lvl = NewStats->Lvl;
		BaseXP = NewStats->BaseXP;
		Absorption = NewStats->Absorption;
		ChanceToBreak = NewStats->ChanceToBreak;
		Weight = NewStats->Weight;
		ArmorState = NewStats->ArmorState;
		Durability = NewStats->Durability;
		StateReductionConst = NewStats->StateReductionConst;
		RemainingDmgReductionMult = NewStats->RemainingDmgReductionMult;

		SetupCurrentStatsArrays(NewStats, /*bIsAdditive:*/ false);

		break;
	case(true):

		StatsConfig->BaseXP = NewStats->BaseXP;
		StatsConfig->Absorption = NewStats->Absorption;
		StatsConfig->ChanceToBreak = NewStats->ChanceToBreak;
		StatsConfig->Weight = NewStats->Weight;
		StatsConfig->ArmorState = NewStats->ArmorState;
		StatsConfig->Durability = NewStats->Durability;
		StatsConfig->StateReductionConst = NewStats->StateReductionConst;
		StatsConfig->RemainingDmgReductionMult = NewStats->RemainingDmgReductionMult;
		break;
	}
}

void AArmor::SetupCurrentStatsArrays(FArmorsDTStruct* Config, bool bIsAdditive)
{
	switch (bIsAdditive)
	{
	case(true):

		if (Config->ArmorModifiers.Num() > 0)
		{
			if (Config->ArmorModifiers[0] == -1)
			{
				ArmorModifiers.Empty();
			}
			else
			{
				for (int i = 0; i < Config->ArmorModifiers.Num(); i++)
					if (!ArmorModifiers.Contains(Config->ArmorModifiers[i]))
						ArmorModifiers.Add(Config->ArmorModifiers[i]);
			}
		}

		if (Config->ArmorAbilities.Num() > 0)
		{
			if (Config->ArmorAbilities[0] == -1)
			{
				ArmorAbilities.Empty();
			}
			else
			{
				for (int i = 0; i < Config->ArmorAbilities.Num(); i++)
					if (!ArmorAbilities.Contains(Config->ArmorAbilities[i]))
						ArmorAbilities.Add(Config->ArmorAbilities[i]);
			}
		}

		break;
	case(false):

		if (Config->ArmorModifiers.Num() > 0)
		{
			if (Config->ArmorModifiers[0] == -1)
				ArmorAbilities.Empty();
			
			else ArmorModifiers = Config->ArmorModifiers;
		}
		
		if (Config->ArmorAbilities.Num() > 0)
		{
			if (Config->ArmorAbilities[0] == -1)

				ArmorAbilities.Empty();

			else ArmorAbilities = Config->ArmorAbilities;
		}

		break;
	}
}

int32 AArmor::GetTargetID() const
{
	return GetUniqueID();
}

ETargetType AArmor::IsTargetType() const
{
	return TargetType;
}

ECharStatus AArmor::GetCharStatus() const
{
	return ItemStatus;
}

bool AArmor::IsDead() const
{
	return ArmorState <= 0.f;
}


void AArmor::Equip(bool bEquip, FName Socket, bool bPlaySFX)
{
	bIsEquiped = bEquip;

	/*if (bPlaySFX)
	{
		ECombatSFX SFX = bEquip ? ECombatSFX::WEAPON_EQUIP : ECombatSFX::WEAPON_UNEQUIP;

		PlaySFX(SFX, GetSoundSampleIdx(SFX), false, 0.2f);
	}*/
}

void AArmor::Equip(ABasePraiseCharacter* Char)
{
	if (PathToMesh.IsEmpty()) return;

	if (Char->CanChangeDefaultSkin())
	{
		USkeletalMesh* SkinMesh = GetSkinMesh(PathToMesh);

		if (!SkinMesh) return;

		Char->GetMesh()->SetSkeletalMesh(SkinMesh);
	}
	
	bIsEquiped = true;
}

void AArmor::OnArmorDropped()
{
	bIsEquiped = false;
}

void AArmor::OnArmorPickup(ABasePraiseCharacter* NewOwner)
{
}

float AArmor::HandleDamage(float DamagePoints)
{
	if (bIsBroken) return DamagePoints;

	float CurrentAbs = GetCurrentAbsorption();
	float RemainingDamage = DamagePoints - CurrentAbs;

	if (RemainingDamage <= 0)
	{
		float ArmorDamage = DamagePoints * (DamagePoints / CurrentAbs);

		// lower the armor state proportionally to the absorbed damage
		ApplyArmorStateReduction(ArmorDamage);

		return ArmorState - ArmorDamage <= 0 ? FMath::Abs(ArmorState - ArmorDamage) : 0.f;
	}
	else
	{
		//Reduce State normally for the absorbed damage
		ApplyArmorStateReduction(CurrentAbs);
		// the remaining damage lowers the armor state multiplied by a factor. 
		// This factor may vary depending on the armor config / modifiers (item quality) 
		ApplyArmorStateReduction(RemainingDamage * RemainingDmgReductionMult);

		return RemainingDamage;
	}
	return 0.0f;
}

UArmorConfig* AArmor::GetCurrentStatsAsConfig()
{
	UArmorConfig* CurrentConfig = NewObject<UArmorConfig>();

	CurrentConfig->BaseXP = BaseXP;
	CurrentConfig->Absorption = Absorption;
	CurrentConfig->ArmorState = ArmorState;
	CurrentConfig->Durability = Durability;
	CurrentConfig->ChanceToBreak = ChanceToBreak;
	CurrentConfig->Weight = Weight;
	CurrentConfig->StateReductionConst = StateReductionConst;
	CurrentConfig->RemainingDmgReductionMult = RemainingDmgReductionMult;

	return nullptr;
}

void AArmor::OnInteractionColBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasOwner()) return;

	InteractionCollider->SetHiddenInGame(false);
}

void AArmor::OnInteractionColEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasOwner()) return;

	InteractionCollider->SetHiddenInGame(true);
}

float AArmor::GetArmorStateFactor() const
{
	float factor = (FMath::Sqrt(GetDurabilityAsRatio()) + FMath::Square(GetArmorStateAsRatio() * 1.1f) * 1) / 2;
	
	return factor;
}

float AArmor::GetArmorStateReduction() const
{
	float Value = 1 / (FMath::Square((GetDurabilityAsRatio() + GetArmorStateAsRatio()) / 2));

	return  (Value / 10) * StateReductionConst;
}

void AArmor::ApplyArmorStateReduction(float HandledDamage)
{
	SetArmorState(ArmorState - (GetArmorStateReduction() * HandledDamage));
}


void AArmor::SetArmorState(float Value)
{
	ArmorState = FMath::Clamp(Value, 0.f, 100.f);

	if (ArmorState <= 50.f)
	{
		ItemStatus = ECharStatus::DAMAGED;	
	}
	if (ArmorState <= 20.f)
	{
		ItemStatus = ECharStatus::VERY_DAMAGED;
	}
	if (ArmorState <= 0.f)
	{
		bIsBroken = true;
	}
}

void AArmor::HandleEnableCollider(UBoxComponent* Collider, bool bEnable)
{
	FName ColProfileName = bEnable ? FName("Weapon") : FName("NoCollision");
	ECollisionEnabled::Type ColType = bEnable ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;

	Collider->SetCollisionProfileName(ColProfileName);
	Collider->SetCollisionEnabled(ColType);
	Collider->SetActive(bEnable);
}

USkeletalMesh* AArmor::GetSkinMesh(FString Path)
{
	USkeletalMesh* Mesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), nullptr, *Path));

	return Mesh;
}
