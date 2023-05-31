// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePraiseCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "../Characters/AI/BTBotController.h"
#include "../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../Characters/Player/PraisePlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../Components/Actor/AnimInstances/PraiseAnimInstance.h"
#include "../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../Components/Actor/AI/BotBrainComponent.h"
#include "../Components/Actor/AI/BTBrainComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Weapons/Fist.h"
#include "../Weapons/MeleeWeapon.h"
#include "../Weapons/Firearm.h"
#include "Blueprint/UserWidget.h"
#include "../Environment/Buildings/BaseBuilding.h"
#include "../Environment/Buildings/PrivateBuilding.h"
#include "../Components/GameMode/MsgCommandsFactoryComponent.h"
#include "../AI/MsgCommands/BaseMsgCommand.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "../Praise.h"
// Sets default values
ABasePraiseCharacter::ABasePraiseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("Targeting Component"));
	CharStats = CreateDefaultSubobject<UCharStatsComponent>(TEXT("Character Stats"));
	TargetWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Target Marker"));
	TargetWidget->SetupAttachment(RootComponent);
	TargetWidget->SetRelativeLocation(FVector(4.f, 3.f, 106.f));
	InteractionCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Interaction Collider"));
	InteractionCollider->SetupAttachment(RootComponent);
	InteractionCollider->SetSphereRadius(300.f);
	InteractionCollider->SetHiddenInGame(true);
	
	
	InteractionCollider->OnComponentBeginOverlap.AddDynamic(this, &ABasePraiseCharacter::OnInteractionBeginOverlap);
	InteractionCollider->OnComponentEndOverlap.AddDynamic(this, &ABasePraiseCharacter::OnInteractionEndOverlap);
	InteractionCollider->SetCollisionProfileName(FName("Interaction"));
	InteractionCollider->SetCollisionObjectType(ECollisionChannel::ECC_EngineTraceChannel3);
	SpeedTLPartition = 1.f;

	bIsDualEquipAnim = false;
	bShouldLinkEquipAnims = false;
	bCanRequestCombo = false;
	bCanAttack = true;
	AttackCounter = 0;
	DestroyCharCountdown = 5.f;
	MinCombatRange = 75.f;
	MaxCombatRange = 300.f;

	TargetType = ETargetType::NPC;
	CharStatus = ECharStatus::NORMAL;
	bHasBuilding = false;
	NeutralCombatHitsTolerance = 4;
	
	bCanChangeDefaultSkin = false;
	DefaultArmorType = EArmorType::NONE;
	DefaultArmorModel = FString("Default");
	DefaultArmorConfig = -1;
	CurrentArmor = nullptr;

	ConstructorHelpers::FClassFinder<UUserWidget> TargetBP(TEXT("/Game/Core/UI/PlayerHUD/WBP_TargetMarker"));

	if (TargetBP.Succeeded())
	{
		TargetWidgetBP = TargetBP.Class;
		SetupTargetWidget();
	}

	ConstructorHelpers::FObjectFinder<UAnimMontage> HitAnims(TEXT("/Game/Core/Animations/CharAnimations/Hit/HitAnimationsMontage"));

	if (HitAnims.Succeeded())
		HitAnimsMontage = HitAnims.Object;

	ConstructorHelpers::FObjectFinder<UAnimMontage> EvadeAnims(TEXT("/Game/Core/Animations/CharAnimations/Locomotion/Evade/EvadeAnimsMontage"));

	if (EvadeAnims.Succeeded())
		EvadeAnimsMontage = EvadeAnims.Object;

	ConstructorHelpers::FObjectFinder<UAnimMontage> PickupAnims(TEXT("/Game/Core/Animations/CharAnimations/Misc/PickupItemsMontage"));

	if (PickupAnims.Succeeded())
		PickupAnimsMontage = PickupAnims.Object;
}

void ABasePraiseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasePraiseCharacter, CharStats);
	DOREPLIFETIME(ABasePraiseCharacter, bIsEquiping);
	DOREPLIFETIME(ABasePraiseCharacter, bIsAttacking);
	DOREPLIFETIME(ABasePraiseCharacter, bIsBlocking);
	DOREPLIFETIME(ABasePraiseCharacter, bIsParring);
	DOREPLIFETIME(ABasePraiseCharacter, bIsRunning);
	DOREPLIFETIME(ABasePraiseCharacter, bIsJumping);
	DOREPLIFETIME(ABasePraiseCharacter, bIsRolling);
	DOREPLIFETIME(ABasePraiseCharacter, bIsEvading);


}
float ABasePraiseCharacter::GetHealth() const
{
	return CharStats ? CharStats->GetHealth() : 0.f;
}
float ABasePraiseCharacter::GetStamina() const
{
	return CharStats ? CharStats->GetStamina() : 0.f;
}
bool ABasePraiseCharacter::IsDead() const
{
	return CharStats ? CharStats->IsDead() : true;
}

bool ABasePraiseCharacter::IsFriendTarget(IFactioneable* Target) const
{
	if (!Target) return false;

	if (this->IsA<ABaseBotCharacter>()) 
		if (Cast<ABaseBotCharacter>(this)->GetBotBrain<UBotBrainComponent>()->GetKnownAllies().Contains(Cast<AActor>(Target)))
			return true; 

	if (PersonalFriends.Contains(Cast<AActor>(Target))) return true;

	if (CharFaction) 
	{
		if (FactionID() == Target->FactionID())
			return true;

		if (CharFaction->GetFriendlyFactions().Contains(Target->FactionID())) 
			return true;

		if (CharFaction->GetKnownAllies().Contains(Cast<AActor>(Target))) 
			return true;
	}
	else 
	{
		return Target->FactionID() == ECharFaction::PROTECTOR && (FactionID() != ECharFaction::PUBLIC_ENEMY && FactionID() != ECharFaction::CREATURE);
	}

	return false; 
}
bool ABasePraiseCharacter::IsEnemyTarget(IFactioneable* Target) const
{	
	if (this && this->IsA<ABaseBotCharacter>()) 
		if (Cast<ABaseBotCharacter>(this)->GetBotBrain<UBotBrainComponent>()->GetKnownEnemies().Contains(Cast<AActor>(Target)))
			return true;

	if (PersonalEnemies.Contains(Cast<AActor>(Target))) return true;

	if (CharFaction)
	{
		if (Target && (Target->FactionID() == FactionID())) return false;

		if (Target && CharFaction->GetEnemyFactions().Contains(Target->FactionID()))
			return true;

		return CharFaction->GetKnownEnemies().Contains(Cast<AActor>(Target));
	}
	else 
	{
		return true;
	}

	return true;
}

TArray<ECharFaction> ABasePraiseCharacter::GetIgnoredFactions() const
{
	return IgnoredFactions;
}

void ABasePraiseCharacter::AddPersonalFoE(AActor* FoE, bool bIsFriend)
{
	switch (bIsFriend)
	{
	case(true):
		if (PersonalEnemies.Contains(FoE))
			PersonalEnemies.Remove(FoE);

		if (!PersonalFriends.Contains(FoE))
			PersonalFriends.Add(FoE);
		break;
	case(false):
		if (PersonalFriends.Contains(FoE))
			PersonalFriends.Remove(FoE);

		if (!PersonalEnemies.Contains(FoE))
			PersonalEnemies.Add(FoE);
		break;
	}
}

ETargetType ABasePraiseCharacter::IsTargetType() const
{
	return TargetType;
}
ECharStatus ABasePraiseCharacter::GetCharStatus() const
{
	return CharStatus;
}
int32 ABasePraiseCharacter::GetTargetID() const
{
	return GetUniqueID();
}

float ABasePraiseCharacter::GetMeleeDistanceOffset() const
{
	return GetCurrentMainWeapon() && GetCurrentMainWeapon()->GetSlotWeapon() ?
		   GetCurrentMainWeapon()->GetSlotWeapon()->GetMeleeDistanceOffset() : 50.f;
}

void ABasePraiseCharacter::SetCharHome(ABaseBuilding* Building)
{
	if (Building->IsPrivate() && !Building->GetBuildingChars().Contains(this)) return;

	CharHome = Building;

	bHasBuilding = true;
}

bool ABasePraiseCharacter::CanEnterBuilding(ABaseBuilding* Building)
{
	if(Building->IsOnFire()) return false;

	return Building->IsOpen() ? true : Building->GetBuildingChars().Contains(this);
}

void ABasePraiseCharacter::RotateTo(float DeltaTime, FVector TargetPosition)
{
	if (!TargetingComponent) return;

	FVector VectorToTarget = TargetPosition - GetActorLocation();

	VectorToTarget.Z = 0;

	FRotator RotXZ = UKismetMathLibrary::MakeRotFromXZ(VectorToTarget, GetActorUpVector());

	FRotator RotLerp = FMath::RInterpTo(GetActorRotation(), RotXZ, DeltaTime, TargetingComponent->GetLockRotationSpeed());

	SetActorRotation(RotLerp);
}
void ABasePraiseCharacter::RotateTo(AActor* Target, float RotSpeed)
{
	TargetActor = Target;
	RotationSpeed = RotSpeed;
	bIsTurning = true;
}
void ABasePraiseCharacter::RotateTo(FVector TargetPosition, float RotSpeed)
{
	RotationTarget = TargetPosition;
	RotationSpeed = RotSpeed;
	bIsTurning = true;
}

void ABasePraiseCharacter::EnableHandCollider(bool bEnable, bool bIsRightHand)
{
}

void ABasePraiseCharacter::EnableKickCollider(bool bEnable, bool bIsRightLeg)
{
}


void ABasePraiseCharacter::EnableWeaponCollider(EWeaponSlot WeaponHand, EAttackType AttackType, bool bEnable)
{
	if (CurrentWeapons.Contains(WeaponHand) && !CurrentWeapons[WeaponHand]->IsEmpty())
		CurrentWeapons[WeaponHand]->GetSlotWeapon()->EnableWeaponCollider(AttackType, bEnable);
}

void ABasePraiseCharacter::EquipWeapon(EHolsterPosition HolsterPos)
{
	FLogger::LogTrace(__FUNCTION__ + FString(" :: HOLSTER ID --> ") + FString::FromInt((int)HolsterPos));
}

void ABasePraiseCharacter::AttachWeaponToSocket(AWeapon* Weapon, FName SocketName, FVector WeaponLoc, FRotator WeaponRot, bool bPlaySFX)
{
	const FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
	
	bool bEquiping = SocketName.ToString().Contains("Hand");

	Weapon->AttachToComponent(GetMesh(), AttachmentRules, SocketName);

	Weapon->Equip(bEquiping, WeaponLoc, WeaponRot, bPlaySFX);
}
void ABasePraiseCharacter::HandleWeaponSlotEquip(UCharWeaponSlot* Slot, bool bIsEquip, FName WeaponSocket)
{
	if (Slot->IsEmpty()) return;

	if(WeaponSocket.IsNone())
		WeaponSocket = !Slot->IsEquiped() ? FUtilities::GetSocketName(Slot->GetHolsterPosition()) : Slot->GetHolsterEquippingHand() == EWeaponSlot::PRIMARY_WEAPON ? FName("Hand_R_Socket") : FName("Hand_L_Socket");

	FVector Location;
	FRotator Rotation;
	if (bIsEquip)
	{
		Location = Slot->GetSlotWeapon()->GetEquipLocation(WeaponSocket == FName("Hand_R_Socket"));
		Rotation = Slot->GetSlotWeapon()->GetEquipRotation(WeaponSocket == FName("Hand_R_Socket"));
	}

	else Slot->GetSlotWeapon()->GetHolsterCoords(Slot->GetHolsterPosition(), Location, Rotation);

	AttachWeaponToSocket(Slot->GetSlotWeapon(), WeaponSocket, Location, Rotation);
}

void ABasePraiseCharacter::ResetComboStuff()
{	
	bCanRequestCombo = false;
	if (bComboRequested && (CurrentAttackType == EAttackType::PRIMARY_ATTACK))
	{
		AttackCounter = AttackCounter + 1 > 3 ? 0 : AttackCounter + 1;	

		if (AttackCounter == 0 || (AttackCounter == 2 && !bComboRequested)) 
		{
			OnAnimStateUpdate(ECharAnimState::ATTACKING, false);
			bCanAttack = false;
		}

		if (AttackCounter != 0 && CurrentAttackType == EAttackType::PRIMARY_ATTACK)
			HandleActionStaminaDecrease(ECharAction::ATTACK);
		
		bComboRequested = false;
	}
	else 
	{
		OnAnimStateUpdate(ECharAnimState::ATTACKING, false);
	}

}

void ABasePraiseCharacter::GetDamage(float Damage, AActor* Damager, EAttackType AttackType)
{
	if (bIgnoreDamage) return;

	if(!bDamageAnimDisabled)
		PlayHitAnimation(Damager);

	if (!CharStats) return;

	CharStats->ManageHealth(Damage, Damager);

	if (GetHealth() <= 50.f)
		CharStatus = ECharStatus::DAMAGED;

	if (GetHealth() <= 20.f)
		CharStatus = ECharStatus::VERY_DAMAGED;
}

void ABasePraiseCharacter::GetWeaponDamage(float Damage, EAttackType AttackType, EWeaponSlot AttackerHand, ABasePraiseCharacter* Damager, AWeapon* DamagerWeapon)
{
	if (bIgnoreDamage) return;

	if (bIsBlocking && !DamagerWeapon->IsA<AFirearm>())
	{
		if (IsTargetForward(Damager->GetActorLocation()))
		{
			HandleActionStaminaDecrease(ECharAction::BLOCK);

			PlayCharSFX(ECharSFX::CHAR_BLOCK);

			return;
		}
	}
	
	if (!bDamageAnimDisabled)
		PlayHitAnimation(Damager);

	if (!CharStats) return;
	
	// the armor can reduce the received damage up to a 70% of the total damage
	// the absorbed damage will lower the armor state and hence the absorption
	// in case the armor cannot absorbe the total damage, the remaining points 
	// will be added to the remaining damage and the armor state reduction is higher 
	// when computing the remaining points
	// the remaining 30% of the original can only be absorbed depending on the Character's Constitution
	// so, yes: the character always gets damage when is damaged (the same way a kevlar armor would stop a bullet but the impact can hurt the person wearing it)
	// this is done like this because otherwise the Character Constitution would be barely relevant in the game and it would not be realistic
	float AbsDamage = FMath::Abs(Damage);
	float PhysicalDamage = AbsDamage * 0.3;
	float PhysicalReduction = CharStats->GetPhysicalDamageReduction(AbsDamage * 0.3);
	float ArmorDamage = AbsDamage * 0.7;

	if (CurrentArmor && !CurrentArmor->IsBroken())
	{
		float RemainingArmorDamage = CurrentArmor->HandleDamage(AbsDamage * 0.7);
		Damage = (((AbsDamage * 0.3) - CharStats->GetPhysicalDamageReduction(AbsDamage * 0.3)) + RemainingArmorDamage) * -1;
	}

	else Damage -= (AbsDamage * 0.3) - CharStats->GetPhysicalDamageReduction(AbsDamage * 0.3);

	CharStats->ManageHealth(Damage, Damager);

	if (GetHealth() <= 50.f)
		UpdateCharStatus(ECharStatus::DAMAGED);

	if (GetHealth() <= 30.f)
		UpdateCharStatus(ECharStatus::VERY_DAMAGED);
}
void ABasePraiseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!CharBPName.IsEmpty() && !Tags.Contains(*CharBPName))
		Tags.Add(*CharBPName);

	OverrideCharStatsComp();

	CacheDefaultUnarmedWeapon();

	SetupCharWeaponSlots();

	SetupCharDefaultArmor();

	SetupTargetWidget();
}

// Called when the game starts or when spawned
void ABasePraiseCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharSpawnLocation = GetActorLocation();

	if(GetCharacterMovement())
		GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	TrySetHandColliders();

	EnableTargetWidget(false);

	if (CharStats)
		CharStats->OnSetDead.BindUFunction(this, FName("SetCharacterDead"));

	bCanRecoverStamina = true;
	bCanAttack = true;
	bCanMove = true;
	bIsTurning = false;
	TurnDirection = 0;

	
	EquippingHand = -1;
	InteractionCollider->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel3);
}

UPraiseAnimInstance* ABasePraiseCharacter::GetPraiseAnimInstance() const
{
	if (!GetMesh() || !GetMesh()->GetAnimInstance()) return nullptr;
	
	if (!GetMesh()->GetAnimInstance()->IsA<UPraiseAnimInstance>()) return nullptr;
	
	return Cast<UPraiseAnimInstance>(GetMesh()->GetAnimInstance());
}

bool ABasePraiseCharacter::HasWeapon() const
{
	for (int i = 0; i < CharWeaponSlots.Num(); i++) {
		if (CharWeaponSlots[(EWeaponSlot)i]->GetSlotWeapon() &&
			!CharWeaponSlots[(EWeaponSlot)i]->IsEmpty() &&
			!CharWeaponSlots[(EWeaponSlot)i]->IsLocked())
			return true;
	}
	return false;
}

bool ABasePraiseCharacter::IsWeaponEquiped() const
{
	return CurrentWeapons.Num() > 0;
}

bool ABasePraiseCharacter::IsEquiping() const
{
	return bIsEquiping;
}

bool ABasePraiseCharacter::IsAttacking() const
{
	return bIsAttacking;
}

bool ABasePraiseCharacter::CanAttack() const
{
	return bCanAttack;
}

bool ABasePraiseCharacter::IsBlocking() const
{
	return bIsBlocking;
}

bool ABasePraiseCharacter::IsBeingDamaged() const
{
	return bIsBeingDamaged;
}

bool ABasePraiseCharacter::IsParring() const
{
	return bIsParring;
}

bool ABasePraiseCharacter::IsAiming() const
{
	return bIsAiming;
}

bool ABasePraiseCharacter::IsTargeting() const
{
	return bIsTargeting;
}

bool ABasePraiseCharacter::IsRolling() const
{
	return bIsRolling;
}

bool ABasePraiseCharacter::IsEvading() const
{
	return bIsEvading;
}

bool ABasePraiseCharacter::IsJumping() const
{
	return bIsJumping;
}

bool ABasePraiseCharacter::IsRunning() const
{
	return bIsRunning;
}

bool ABasePraiseCharacter::IsCrouching() const
{
	return bIsCrouched;
}

EArmedPoses ABasePraiseCharacter::GetCurrentPose() const
{
	return CurrentPose;
}

EAttackType ABasePraiseCharacter::GetCurrentAttackType() const
{
	return CurrentAttackType;
}

TMap<EWeaponSlot, UCharWeaponSlot*> ABasePraiseCharacter::GetCharCurrentWeapons() const
{
	return CurrentWeapons;
}

bool ABasePraiseCharacter::IsInCombatRange(AActor* Opponent, float MaxDistanceOffset) const
{
	if (!Opponent) return false;

	FVector ToOpponent = Opponent->GetActorLocation() - GetActorLocation();

	return ToOpponent.Size() >= MinCombatRange && ToOpponent.Size() <= MaxCombatRange + MaxDistanceOffset;
}

ECharVector ABasePraiseCharacter::GetTargetLocationVector(AActor* Target) const
{
	FVector ToTarget = Target->GetActorLocation() - GetActorLocation();

	float DegsToTarg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorForwardVector(), ToTarget.GetSafeNormal())));

	FVector Cross = FVector::CrossProduct(GetActorForwardVector(), ToTarget.GetSafeNormal());

	int SideMult = FMath::Sign(Cross.Z);

	DegsToTarg = DegsToTarg * SideMult;

	if (FMath::Abs(DegsToTarg) <= 25.f)
		return ECharVector::FWD;

	if (FMath::Abs(DegsToTarg) >= 180.f - 25.f)
		return ECharVector::BWD;
	
	return FMath::Sign(DegsToTarg) < 0 ? ECharVector::LEFT : ECharVector::RIGHT;
}

void ABasePraiseCharacter::DisableTargeting()
{
	bIsTargeting = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

UCharWeaponSlot* ABasePraiseCharacter::GetCurrentMainWeapon() const
{
	if (CurrentWeapons.Num() <= 0) return nullptr;

	if (CurrentWeapons.Num() > 1) return !CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON]->IsEmpty() && CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON]->GetSlotWeapon()->GetWeaponClass() == EWeaponClass::SHIELD ? CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON] : CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON];

	return CurrentWeapons.Contains(EWeaponSlot::PRIMARY_WEAPON) ? CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON] : CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON];	
}

UCharWeaponSlot* ABasePraiseCharacter::GetCurrentSecondaryWeapon() const
{
	if (CurrentWeapons.Num() != 2) return nullptr;
	
	if (CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON]->IsEmpty()) return nullptr;
	
	return CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON];
}

bool ABasePraiseCharacter::IsInMeleeRange(AActor* Opponent, float MaxDistanceOffset) const
{
	if (!Opponent) return false;

	FVector ToOpponent = Opponent->GetActorLocation() - GetActorLocation();

	return IsInCombatRange(Opponent, MaxDistanceOffset) && ToOpponent.Size() >= MinCombatRange && ToOpponent.Size() <= (MinCombatRange + MaxDistanceOffset);
}

void ABasePraiseCharacter::GetPlayerViewPoint(FVector& OutLocation, FRotator& OutRotation, bool bIsCameraView) const
{
	OutLocation = GetPawnViewLocation();
	OutRotation = GetViewRotation();
}
int ABasePraiseCharacter::GetScaledSpeed() const
{
	if (!GetCharacterMovement() || ! CharStats) return 0;

	return (GetCharacterMovement()->MaxWalkSpeed / CharStats->GetDefaultWalkSpeed());
}

void ABasePraiseCharacter::OverrideCharStatsComp()
{
	if (!CharStats)
		CharStats = NewObject<UCharStatsComponent>(this);

	if (CharStats) 
	{
		CharStats->RegisterComponent();
		CharStats->InitializeComponent();
	}
}

void ABasePraiseCharacter::SetupCharDefaultArmor()
{
	if (DefaultArmorType == EArmorType::NONE) return;

	if (!CharArmorsFactory) return;

	AArmor* Armor = CharArmorsFactory->SpawnArmor<AArmor>(DefaultArmorType, DefaultArmorModel, DefaultArmorConfig);

	if (!Armor) return;

	Armor->Equip(this);

	CurrentArmor = Armor;
}


void ABasePraiseCharacter::HandleWeaponRotation(float DeltaTime)
{
	if(!CurrentWeapons.Contains(WeaponToRotate)) return;

	if (CurrentWeapons[WeaponToRotate]->IsEmpty()) return;

	AWeapon* Weapon = CurrentWeapons[WeaponToRotate]->GetSlotWeapon();

	float f = FMath::FInterpTo(CurrentWeaponRot.Pitch, WeaponTargetRotation.Pitch, DeltaTime, 4.f);
	
	float Delta = (FMath::Abs(CurrentWeaponRot.Pitch) - FMath::Abs(f)) * -FMath::Sign(CurrentWeaponRot.Pitch);

	CurrentWeaponRot.Pitch += (FMath::Abs(CurrentWeaponRot.Pitch) - FMath::Abs(f)) * -FMath::Sign(CurrentWeaponRot.Pitch);

	if (FMath::Abs(f) <= 0.5)
	{
		bHandleWeaponRot = false;

		return;
	}

	Weapon->AddActorLocalRotation(FRotator(Delta,0, 0));
}

void ABasePraiseCharacter::ResetMainWeaponEquipPosition()
{
	UCharWeaponSlot* MainSlot = nullptr;
	AWeapon* MainWeapon = nullptr;

	MainSlot = GetCurrentMainWeapon();

	if (MainSlot && !MainSlot->IsEmpty())
	{
		MainWeapon = MainSlot->GetSlotWeapon();

		if (IsWeaponEquiped() && MainSlot->IsEquiped())
			AttachWeaponToSocket(MainSlot->GetSlotWeapon(), MainSlot->GetEquipedHand() == EWeaponSlot::PRIMARY_WEAPON ? FName("Hand_R_Socket") : FName("Hand_L_Socket"), MainWeapon->GetEquipLocation(), MainWeapon->GetEquipRotation(), false);
	}
}

void ABasePraiseCharacter::DestroyCharacter()
{
	if (HasWeapon())
	{
		for (int i = 0; i < CharWeaponSlots.Num(); i++)
		{
			if (CharWeaponSlots[(EWeaponSlot)i]->HoldsA<AFist>() && CharWeaponSlots[(EWeaponSlot)i]->GetSlotWeapon()->GetWeaponModel() == FString("Default"))
			{
				AWeapon* DefaultFists = CharWeaponSlots[(EWeaponSlot)i]->ClearSlot();
				DefaultFists->Destroy();
			}
		}

		if (AuxSlot)
			AuxSlot->ClearSlot()->Destroy();
	}

	if (CurrentArmor)
		CurrentArmor->Destroy();

	Destroy();
}

void ABasePraiseCharacter::PlayHitAnimation(AActor* Damager)
{
	if (!HitAnimsMontage) return;

	if (bIsBeingDamaged) return; 

	if (GetController())
		GetController()->StopMovement();
	
	bool bTargetFwd = IsTargetForward(Damager->GetActorLocation());

	if (bIsAttacking)
		OnAnimStateUpdate(ECharAnimState::ATTACKING, false);

	if (this->IsA<ABaseBotCharacter>() && Cast<ABaseBotCharacter>(this)->GetBrain())
		Cast<ABaseBotCharacter>(this)->GetBrain()->PauseBrain(true);
	
	bIsBeingDamaged = true;
	bCanMove = false;
	bCanAttack = false;

	int idx = FMath::RandRange(1, 4);
	
	FString SectionName = FString("HitStart_") + FString::FromInt(idx);
	
	PlayMontage(HitAnimsMontage, bTargetFwd ? *SectionName : FName("HitStart_5"), 1.25f);

	PlayCharSFX(ECharSFX::CHAR_DAMAGED);

}

void ABasePraiseCharacter::PlayEvadeAnimation(ECharVector Direction)
{
	if (!EvadeAnimsMontage) return;

	FString SectionName = FUtilities::EnumToString(*FString("ECharVector"), (int)Direction);

	if (SectionName == FString("None"))
		SectionName = FString("BWD");

	bCanMove = false;
	bIsAttacking = false;
	bIgnoreDamage = true;

	PlayMontage(EvadeAnimsMontage, *SectionName, 1.f);

	HandleActionStaminaDecrease(ECharAction::EVADE);

	PlayCharSFX(ECharSFX::CHAR_EVADE);
}

void ABasePraiseCharacter::PlayPickupAnimation(AActor* PickedItem, EWeaponSlot Hand, bool bPickFromGround)
{
	if (!PickedItem) return;

	if (!PickupAnimsMontage) return;

	FString Section = FString("");
	
	switch (Hand)
	{
		case(EWeaponSlot::PRIMARY_WEAPON):
		case(EWeaponSlot::EXTRA_SLOT_1):
		case(EWeaponSlot::EXTRA_SLOT_2):
			if (bIsCrouched)
			{
				if (bPickFromGround)
				{
					Section = FString("PickupItem_R_Crchd_Grnd");
				}
				else
				{
					Section = FString("PickupItem_R_Crouched");
				}
			}
			else
			{
				if (bPickFromGround)
				{
					Section = FString("PickupItem_R_Standing_Grnd");
				}
				else
				{
					Section = FString("PickupItem_R_Standing");
				}
			}

			break;
		case(EWeaponSlot::SECONDARY_WEAPON):
			
			if (bIsCrouched)
			{
				if (bPickFromGround)
				{
					Section = FString("PickupItem_L_Crchd_Grnd");
				}
				else
				{
					Section = FString("PickupItem_L_Crouched");
				}
			}
			else
			{
				if (bPickFromGround)
				{
					Section = FString("PickupItem_L_Standing_Grnd");
				}
				else
				{
					Section = FString("PickupItem_L_Standing");
				}
			}
			break;
		default:
			break;
	}

	bDamageAnimDisabled = true;
	bCanMove = false;
	bIsAttacking = false;
	bIsPickingupItem = true;
	CurrentlyPickedupItem = PickedItem;

	PlayMontage(PickupAnimsMontage,*Section, 1.25f, false);
	
}

void ABasePraiseCharacter::HandlePickupWeapon(AWeapon* Weapon, EWeaponSlot RequestedSlot, bool bPickFromGround)
{
	TArray<UCharWeaponSlot*> AvailableSlots;
	
	PickWeaponSlot = NewObject<UCharWeaponSlot>(this);
	PickWeaponSlot->SetupSlot(EWeaponSlot::INVALID);

	TArray<EWeaponSlot> Keys;
	CharWeaponSlots.GetKeys(Keys);

	if (RequestedSlot == EWeaponSlot::INVALID)
	{
		for (EWeaponSlot Slot : Keys)
			if (!CharWeaponSlots[Slot]->IsLocked())
			{
				if (!CharWeaponSlots[Slot]->IsEmpty())
				{
					if (CharWeaponSlots[Slot]->HasDefaultFist())
						AvailableSlots.Add(CharWeaponSlots[Slot]);
				}

				else AvailableSlots.Add(CharWeaponSlots[Slot]);
			}

		if (AvailableSlots.Num() > 0)
		{
			PickWeaponForSlot(Weapon, AvailableSlots[0]->GetSlotID(), bPickFromGround);

			return;
		}
	}
	
	switch (RequestedSlot)
	{
		case(EWeaponSlot::INVALID): 
			PickWeaponForSlot(Weapon, EWeaponSlot::PRIMARY_WEAPON, bPickFromGround);
			break;

		case(EWeaponSlot::PRIMARY_WEAPON):
		case(EWeaponSlot::SECONDARY_WEAPON):
			UpdateWeaponSlot(Weapon, RequestedSlot, bPickFromGround);
		case(EWeaponSlot::EXTRA_SLOT_1):
		case(EWeaponSlot::EXTRA_SLOT_2):
		default:
			break;
	}
}

void ABasePraiseCharacter::PickWeaponForSlot(AWeapon* Weapon, EWeaponSlot RequestedSlot, bool bPickFromGround, bool bForceHandUpdate)
{
	if (!CharWeaponSlots.Contains(RequestedSlot)) return;

	EWeaponSlot CurrentHand = IsWeaponEquiped() ? GetCurrentMainWeapon()->GetHolsterEquippingHand() : EWeaponSlot::PRIMARY_WEAPON;

	EWeaponSlot OtherHand = CurrentHand == EWeaponSlot::PRIMARY_WEAPON ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON;

	EWeaponSlot PickHand = Weapon->GetWeaponClass() == EWeaponClass::SHIELD ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON;

	EHolsterPosition ComplementaryHolster = Weapon->GetHolsterForHand(OtherHand);
	
	if (IsWeaponEquiped())
	{
		EWeaponSlot DroppedSlot = RequestedSlot;
		EWeaponSlot MainHandSlot = GetCurrentMainWeapon()->GetSlotID();
		EWeaponSlot OtherSlot = MainHandSlot == EWeaponSlot::PRIMARY_WEAPON ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON;
		EHolsterPosition CurrentHandWeaponHolster = Weapon->GetHolsterForHand(CurrentHand);
		if (CurrentPose == EArmedPoses::DUALS)
		{
			if (Weapon->IsTwoHand())
			{
				if (CurrentHand == EWeaponSlot::SECONDARY_WEAPON)
				{
					if (CharWeaponSlots[OtherSlot]->IsEmpty() || CharWeaponSlots[OtherSlot]->HasDefaultFist())
					{
						PickWeaponSlot->SetupSlot(OtherSlot, Weapon, false, OtherHand);

						PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);

						DroppedSlot = OtherSlot;
					}
					else
					{ 
						PickWeaponSlot->SetupSlot(MainHandSlot, Weapon, false, OtherHand, CurrentHandWeaponHolster);

						PlayPickupAnimation(Weapon, CurrentHand, bPickFromGround);

						DroppedSlot = MainHandSlot;
					}
						
				}
				else
				{
					PickWeaponSlot->SetupSlot(OtherSlot, Weapon, false, OtherHand);

					PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);

					DroppedSlot = OtherSlot;
				}
			}
			else
			{
				if (!bForceHandUpdate)
				{
					if (ComplementaryHolster != EHolsterPosition::UNARMED)
					{
						DroppedSlot = OtherSlot;
						PickWeaponSlot->SetupSlot(DroppedSlot, Weapon, true, OtherHand, ComplementaryHolster);

						PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);
					}
					else
					{
						DroppedSlot = OtherSlot;
						PickWeaponSlot->SetupSlot(DroppedSlot, Weapon, false, OtherHand, CurrentHandWeaponHolster);

						PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);
					}
				}
				else
				{
					bool bUpdatingMainHandSlot = RequestedSlot == MainHandSlot;

					if (CurrentHand == EWeaponSlot::PRIMARY_WEAPON)
					{
						if (CurrentHandWeaponHolster != EHolsterPosition::UNARMED)
						{
							PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, !Weapon->IsTwoHand(), CurrentHand);

							PlayPickupAnimation(Weapon, CurrentHand, bPickFromGround);

							DroppedSlot = MainHandSlot;
						}
						else
						{
							PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, true, OtherHand);

							PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);

							DroppedSlot = RequestedSlot;
						}
					}
					if (CurrentHand == EWeaponSlot::SECONDARY_WEAPON)
					{
						if (CurrentHandWeaponHolster != EHolsterPosition::UNARMED)
						{
							PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, true, CurrentHand);

							PlayPickupAnimation(Weapon, CurrentHand, bPickFromGround);
								
							DroppedSlot = RequestedSlot;
						}
						else
						{
							PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, false, OtherHand);

							PlayPickupAnimation(Weapon, CurrentHand, bPickFromGround);

							DroppedSlot = RequestedSlot;
						}
					}
				}
			}

			if (!CharWeaponSlots[DroppedSlot]->IsEmpty() && !CharWeaponSlots[DroppedSlot]->HasDefaultFist())
				DropWeapon(DroppedSlot, false);
		}
		else
		{
			if (CurrentPose == EArmedPoses::UNARMED)
			{
				CurrentWeapons.Empty();

				PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, true, PickHand);

				PlayPickupAnimation(Weapon, PickHand, bPickFromGround);
			}
			else
			{
				if (CurrentWeapons[CurrentHand]->GetSlotWeapon()->IsTwoHand() || Weapon->IsTwoHand())
				{
					if (ComplementaryHolster != EHolsterPosition::UNARMED)
					{
						if (CurrentHandWeaponHolster != EHolsterPosition::UNARMED) 
						{
							PickWeaponSlot->SetupSlot(OtherSlot, Weapon, false, CurrentHand, CurrentHandWeaponHolster);

							PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);
						}
						else 
						{
							PickWeaponSlot->SetupSlot(OtherSlot, Weapon, false, OtherHand, ComplementaryHolster);

							PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);
						}

						if(!CharWeaponSlots[OtherSlot]->IsEmpty() && !CharWeaponSlots[OtherSlot]->HasDefaultFist())
							DropWeapon(OtherSlot, false);
					}
					else
					{		
						if (CurrentHand == EWeaponSlot::SECONDARY_WEAPON)
						{
							PickWeaponSlot->SetupSlot(OtherSlot, Weapon, false, OtherHand, ComplementaryHolster);
						}
						else
						{
							PickWeaponSlot->SetupSlot(OtherSlot, Weapon, false, CurrentHand, CurrentHandWeaponHolster);
						}
							
						PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);

						if (!CharWeaponSlots[OtherSlot]->IsEmpty() && !CharWeaponSlots[OtherSlot]->HasDefaultFist())
							DropWeapon(OtherSlot, false);
					}
				}
				else
				{
					if (ComplementaryHolster != EHolsterPosition::UNARMED /*&& !bForceHandUpdate*/)
					{
						PickWeaponSlot->SetupSlot(OtherSlot, Weapon, true, OtherHand, ComplementaryHolster);

						PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);

						if (!CharWeaponSlots[OtherSlot]->IsEmpty() && !CharWeaponSlots[OtherSlot]->HasDefaultFist())
							DropWeapon(OtherSlot, false);
					}
					else
					{
						PickWeaponSlot->SetupSlot(OtherSlot, Weapon, false, OtherHand, CurrentHandWeaponHolster);

						PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);

						if (!CharWeaponSlots[RequestedSlot]->IsEmpty() && !CharWeaponSlots[RequestedSlot]->HasDefaultFist())
							DropWeapon(RequestedSlot, false);
					}

				}					
			}
		}
	}
	else
	{
		PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, false, PickHand);
		PlayPickupAnimation(Weapon, PickHand, bPickFromGround);

		if (!CharWeaponSlots[RequestedSlot]->IsEmpty() && !CharWeaponSlots[RequestedSlot]->HasDefaultFist())
			DropWeapon(RequestedSlot, false);
	}

	
}

void ABasePraiseCharacter::UpdateWeaponSlot(AWeapon* Weapon, EWeaponSlot RequestedSlot, bool bPickFromGround)
{
	if (!CharWeaponSlots.Contains(RequestedSlot)) return;

	EWeaponSlot CurrentHand = IsWeaponEquiped() ? GetCurrentMainWeapon()->GetHolsterEquippingHand() : EWeaponSlot::PRIMARY_WEAPON;

	EWeaponSlot OtherHand = CurrentHand == EWeaponSlot::PRIMARY_WEAPON ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON;
	
	EWeaponSlot PickHand = Weapon->GetWeaponClass() == EWeaponClass::SHIELD ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON;

	EHolsterPosition ComplementaryHolster = Weapon->GetHolsterForHand(OtherHand);
	
	if (IsWeaponEquiped())
	{
		EWeaponSlot MainHandSlot = GetCurrentMainWeapon()->GetSlotID();
		
		EWeaponSlot OtherSlot = MainHandSlot == EWeaponSlot::PRIMARY_WEAPON ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON;
		
		EHolsterPosition CurrentHandWeaponHolster = Weapon->GetHolsterForHand(CurrentHand);
		
		bool bUpdatingMainHandSlot = RequestedSlot == MainHandSlot;
		
		if (CurrentPose == EArmedPoses::DUALS)
		{
			if (bUpdatingMainHandSlot)
			{
				if (Weapon->IsTwoHand())
				{
					if (CurrentHand == EWeaponSlot::SECONDARY_WEAPON)
					{
						if (CharWeaponSlots[OtherSlot]->IsEmpty() || CharWeaponSlots[OtherSlot]->HasDefaultFist() || CurrentWeapons[OtherHand]->HoldsA<AFist>())
						{
							PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, true, OtherHand);

							PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);

						}
						else 
						{
							PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, false, CurrentHand);

							PlayPickupAnimation(Weapon, CurrentHand, bPickFromGround);
						}
					}
					else
					{
						PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, true, CurrentHand);

						PlayPickupAnimation(Weapon, CurrentHand, bPickFromGround);
					}

				}
				else 
				{
					if (CurrentHandWeaponHolster != EHolsterPosition::UNARMED)
					{
						PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, true, CurrentHand, CurrentHandWeaponHolster);

						PlayPickupAnimation(Weapon, CurrentHand, bPickFromGround);
					}
					else
					{
						PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, false, CurrentHand, ComplementaryHolster);

						PlayPickupAnimation(Weapon, CurrentHand, bPickFromGround);
					}
				}

			}
			else
			{
				if (Weapon->IsTwoHand())
				{
					if (CurrentHand == EWeaponSlot::SECONDARY_WEAPON)
					{
						PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, false, OtherHand, CurrentHandWeaponHolster);

						PlayPickupAnimation(Weapon, CurrentHand, bPickFromGround);
					}
					else
					{
						PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, false, CurrentHand, CurrentHandWeaponHolster);

						PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);
					}
				}
				else
				{
					if (CurrentHand == EWeaponSlot::SECONDARY_WEAPON)
					{
						if (ComplementaryHolster != EHolsterPosition::UNARMED)
						{
							PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, true, OtherHand, ComplementaryHolster);

							PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);
						}
						else
						{
							PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, false, CurrentHand, CurrentHandWeaponHolster);

							PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);
						}
						
					}
					else
					{
						if (ComplementaryHolster != EHolsterPosition::UNARMED)
						{
							PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, true, OtherHand, ComplementaryHolster);

							PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);
						}
						else
						{
							PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, false, CurrentHand, CurrentHandWeaponHolster);

							PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);
						}
						
					}
				}
			}
		
			if (!CharWeaponSlots[RequestedSlot]->IsEmpty() && !CharWeaponSlots[RequestedSlot]->HasDefaultFist())
				DropWeapon(RequestedSlot, false);
		}
		else
		{
			if (CurrentPose == EArmedPoses::UNARMED)
			{
				if (bUpdatingMainHandSlot)
				{
					CurrentWeapons.Empty();

					PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, true, PickHand);

					PlayPickupAnimation(Weapon, PickHand, bPickFromGround);
				}
				else
				{
					PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, false, PickHand);

					PlayPickupAnimation(Weapon, PickHand, bPickFromGround);

					if (!CharWeaponSlots[RequestedSlot]->IsEmpty() && !CharWeaponSlots[RequestedSlot]->HasDefaultFist())
						DropWeapon(RequestedSlot, false);
				}
			}
			else
			{
				if (CurrentWeapons[CurrentHand]->GetSlotWeapon()->IsTwoHand())
				{
					if (bUpdatingMainHandSlot)
					{
						if (ComplementaryHolster != EHolsterPosition::UNARMED)
						{
							if (CurrentHandWeaponHolster != EHolsterPosition::UNARMED)
							{
								PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, true, CurrentHand, CurrentHandWeaponHolster);

								PlayPickupAnimation(Weapon, CurrentHand, bPickFromGround);
							}
							else
							{
								PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, true, OtherHand, ComplementaryHolster);

								PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);
							}
						}
						else
						{
							PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, true, CurrentHand, CurrentHandWeaponHolster);

							PlayPickupAnimation(Weapon, CurrentHand, bPickFromGround);
						}
					}
					else
					{
						if (CurrentHandWeaponHolster != EHolsterPosition::UNARMED)
						{
							PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, false, OtherHand, CurrentHandWeaponHolster);

							PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);
						}
						else
						{
							PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, false, OtherHand, ComplementaryHolster);

							PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);
						}
						
					}
					
					if (!CharWeaponSlots[RequestedSlot]->IsEmpty() && !CharWeaponSlots[RequestedSlot]->HasDefaultFist())
						DropWeapon(RequestedSlot, false);
		
				}
				else
				{
					if (ComplementaryHolster != EHolsterPosition::UNARMED)
					{
						PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, !Weapon->IsTwoHand(), OtherHand, ComplementaryHolster);

						PlayPickupAnimation(Weapon, OtherHand, bPickFromGround);
					}
					else
					{
						PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, !Weapon->IsTwoHand() && bUpdatingMainHandSlot, CurrentHand, CurrentHandWeaponHolster);

						PlayPickupAnimation(Weapon, CurrentHand == EWeaponSlot::SECONDARY_WEAPON && bUpdatingMainHandSlot ? CurrentHand : OtherHand, bPickFromGround);
					}

					if (!CharWeaponSlots[RequestedSlot]->IsEmpty() && !CharWeaponSlots[RequestedSlot]->HasDefaultFist())
						DropWeapon(RequestedSlot, false);
				}
			}
		}
	}
	else
	{


	PickWeaponSlot->SetupSlot(RequestedSlot, Weapon, false, PickHand);
		PlayPickupAnimation(Weapon, PickHand, bPickFromGround);

		if (!CharWeaponSlots[RequestedSlot]->IsEmpty() && !CharWeaponSlots[RequestedSlot]->HasDefaultFist())
			DropWeapon(RequestedSlot, false);
	}
}

void ABasePraiseCharacter::HandleSprint()
{
	ECurveType Curve = ECurveType::ANTILOG;

	if (bIsRunning)
		UpdateWalkSpeed(Curve);
	
	else RestoreWalkSpeed(this->IsA<APraisePlayerCharacter>(), Curve);
}

void ABasePraiseCharacter::HandleRotateTo(float DeltaTime)
{
	if (RotationTarget == FVector::ZeroVector && !TargetActor) return;

	if (RotationTarget != FVector::ZeroVector) {
		FVector ToRotTarget = (RotationTarget - GetActorLocation()).GetSafeNormal();
		ToRotTarget.Z = 0;
		float AngleToRotTarget = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorForwardVector(), ToRotTarget)));

		FVector Cross = FVector::CrossProduct(ToRotTarget, GetActorForwardVector());
		FRotator RotTarget = FRotator(GetActorRotation().Pitch, (AngleToRotTarget * FMath::Sign(Cross.Z)), GetActorRotation().Roll);
		FRotator NewRot = FMath::RInterpTo(GetActorRotation(), RotTarget, DeltaTime, RotationSpeed);

		if (FMath::Abs(AngleToRotTarget) <= 40.f) 
		{
			bIsTurning = false;
			TurnDirection = 0;
			RotationSpeed = 1.f;
			TargetActor = nullptr;
		}

		else 
		{
			bIsTurning = true;
			RotationSpeed = 3.f;
			TurnDirection = FMath::Sign(Cross.Z);
		}

		return;
	}

	if (TargetActor) 
	{	
		FVector ToTarget = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		
		float DegsToTarg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ToTarget, GetActorForwardVector())));

		FVector Cross = FVector::CrossProduct(ToTarget, GetActorForwardVector());
		
		FRotator RotTarget = FRotator(GetActorRotation().Pitch, (DegsToTarg * FMath::Sign(Cross.Z)), GetActorRotation().Roll);
		FRotator NewRot = FMath::RInterpTo(GetActorRotation(), RotTarget, DeltaTime, RotationSpeed);

		if (FMath::Abs(DegsToTarg) <= 20.f) 
		{		
			bIsTurning = false;
			TurnDirection = 0;
			RotationSpeed = 1.f;
			TargetActor = nullptr;
		}
		else 
		{
			bIsTurning = true;
			RotationSpeed = 3.f;
			TurnDirection = FMath::Sign(Cross.Z);
		}

		return;
	}
}

UBoxComponent* ABasePraiseCharacter::InitHandCollider(FString ColName, FName SocketName, FVector Size)
{
	UBoxComponent* Collider = NewObject<UBoxComponent>(this, *ColName);
	
	Collider->RegisterComponent();
	Collider->InitializeComponent();
	
	const FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);

	Collider->AttachToComponent(GetMesh(), AttachmentRules, SocketName);
	
	Collider->SetRelativeScale3D(FVector(0.25f, 0.25f, 0.25f));
	
	Collider->SetNotifyRigidBodyCollision(true);
	
	Collider->SetVisibility(false);

	Collider->SetHiddenInGame(true);
	
	Collider->SetCollisionProfileName(FName("NoCollision"));
	
	Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	return Collider;
}


void ABasePraiseCharacter::TrySetHandColliders()
{
	
}

void ABasePraiseCharacter::OnInteractionBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void ABasePraiseCharacter::OnInteractionEndOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void ABasePraiseCharacter::OnAnimStateUpdate(ECharAnimState State, bool bIsEnabled)
{
	UCharWeaponSlot* MainSlot = nullptr;
	AWeapon* MainWeapon = nullptr;

	switch (State)
	{
		case(ECharAnimState::IDLE):
			//ResetAll();
			break;
		case(ECharAnimState::ATTACKING):
		case(ECharAnimState::PARRING):
			bIsAttacking = bIsEnabled;
			bIsParring = false;
			bIsCombo = false;
			if (!bIsEnabled) 
			{
				AttackCounter = 0;
				bIsPlayingAnimation = false;
				CurrentAttackType = EAttackType::NONE;		
				RequestedAttackType = EAttackType::NONE;
				bCanMove = true;
				if (!ensure(GetWorld() != nullptr)) return;
				GetWorld()->GetTimerManager().SetTimer(AttackResetTimerHandle, this, &ABasePraiseCharacter::ResetAttackStuff, AttackResetTime, false);
			}
			break;

		case(ECharAnimState::EQUIPING):

			bIsEquiping = bIsEnabled;
			
			if (!bIsEquiping)
			{
				bIsEquippingWeapon = false;
				
				if (bIsDualEquipAnim && bShouldLinkEquipAnims)
				{
					bIsDualEquipAnim = false;
					bShouldLinkEquipAnims = true;
					bIsEquiping = true;
					bIsEquippingWeapon = IsWeaponEquiped();
					return;
				}
				
				else bShouldLinkEquipAnims = false;

				if (!IsWeaponEquiped())
				{
					if (bIsSwitchingWeapons && NextSlotToEquip) 
					{ 
						bIsSwitchingWeapons = false;
						EquipWeapons(NextSlotToEquip->GetSlotID(), false);
					}

					else
					{
						SetNextSlotToEquip(EWeaponSlot::INVALID);
						
						CurrentPose = EArmedPoses::UNARMED;
					}
				}
				else
				{
					if (CurrentWeapons.Num() == 1)
					{
						if (CurrentWeapons.Contains(EWeaponSlot::SECONDARY_WEAPON) && CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON]->GetSlotWeapon()->GetWeaponClass() == EWeaponClass::SHIELD)
						{
							UCharWeaponSlot* DefaultFistSlot = nullptr;

							if (HasSlotWeapon<AFist>(FString("Default"), DefaultFistSlot))
							{
								CurrentWeapons.Add(EWeaponSlot::PRIMARY_WEAPON, DefaultFistSlot);
								DefaultFistSlot->SetIsEquiped(true, EWeaponSlot::PRIMARY_WEAPON);
							}
							else
							{
								if (AuxSlot)
								{
									CurrentWeapons.Add(EWeaponSlot::PRIMARY_WEAPON, AuxSlot);
									AuxSlot->SetIsEquiped(true, EWeaponSlot::PRIMARY_WEAPON);
								}
							}

							CurrentPose = EArmedPoses::DUALS;
						}

						else CurrentPose = CurrentWeapons.Contains(EWeaponSlot::PRIMARY_WEAPON) ? CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->GetCombatPose() : CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON]->GetSlotWeapon()->GetCombatPose();
					}

					if (CurrentWeapons.Num() == 2)
					{
						CurrentPose = CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->HoldsA<AFist>() && CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON]->HoldsA<AFist>() ? EArmedPoses::UNARMED : EArmedPoses::DUALS;
					}

					SetNextSlotToEquip(EWeaponSlot::INVALID);

					bIsSwitchingWeapons = false;
				}

				bDamageAnimDisabled = false;
			}

			break;
		case(ECharAnimState::JUMPING):
			bIsJumping = bIsEnabled;
			break;
		case(ECharAnimState::ROLLING):
			bIsRolling = bIsEnabled;
			break;
		case(ECharAnimState::RELOADING):
			bIsReloading = bIsEnabled;
			break;
		case(ECharAnimState::EVADING):
			bIsEvading = bIsEnabled;
			EvadeDirection = ECharVector::NONE;
			bCanMove = true;
			bCanAttack = true;
			bIgnoreDamage = false;
			break;
		case(ECharAnimState::PICKING_ITEM):
			bDamageAnimDisabled = false;
			bIsPickingupItem = false;
			CurrentlyPickedupItem = nullptr;
			bCanMove = true;
			PickWeaponSlot = nullptr;
			
			ResetDefaultWeapon();
			break;
		default:
				break;
	}
}

void ABasePraiseCharacter::SetBlendspaceTL(ECurveType BlendCurve, float TimePartition, bool bLooping)
{
	UCurveFloat* Curve = GetCurve(BlendCurve);

	if (!Curve) return;

	SetBlendspaceTL(Curve, TimePartition, bLooping);
}

void ABasePraiseCharacter::SetBlendspaceTL(UCurveFloat* BlendCurve, float TimePartition, bool bLooping)
{
	FTimeline tl;
	BlendSpace1DTL = tl;
	BlendSpace1DTL.SetNewTime(0.f);
	FOnTimelineFloat OnBlendSpaceTimelineFloat;
	FOnTimelineEventStatic OnBlendSpaceTLFinished;
	OnBlendSpaceTimelineFloat.BindUFunction(this, FName("HandleBlendspaceTL"));
	OnBlendSpaceTLFinished.BindUFunction(this, FName("OnBlendSpaceTLFinished"));

	BlendSpace1DTL.SetTimelineFinishedFunc(OnBlendSpaceTLFinished);
	BlendSpace1DTL.AddInterpFloat(BlendCurve, OnBlendSpaceTimelineFloat);
	BlendSpace1DTL.SetLooping(bLooping);
	BlendSpace1DTL.PlayFromStart();
	BlendSpace1DTL.SetPlayRate(1 / TimePartition);
	bHandleBlendspaceTL = true;
}

void ABasePraiseCharacter::HandleBlendspaceTL(float DeltaTime)
{
	if (!GetPraiseAnimInstance()) return;

	GetPraiseAnimInstance()->EquipAnimLerpVal = DeltaTime;
}

void ABasePraiseCharacter::OnBlendSpaceTLFinished()
{
	bHandleBlendspaceTL = false;

	if (!GetMesh()) return;

	OnAnimStateUpdate(ECharAnimState::EQUIPING, false);
}

float ABasePraiseCharacter::ExecuteBlendspace(UBlendSpace1D* BS, UCurveFloat* AnimCurve, float TimelinePartition, bool bWithRootMot, bool bNetCast)
{
	if (!GetMesh() || !GetMesh()->GetAnimInstance()) return -1;

	return BS->AnimLength;
}

void ABasePraiseCharacter::OnMontageSectionPlayed()
{
	bIsBeingDamaged = false;
	bCanMove = true;
	bCanAttack = true;
	
	if (bIsEvading)
		OnAnimStateUpdate(ECharAnimState::EVADING, false);

	if (this->IsA<ABaseBotCharacter>() && Cast<ABaseBotCharacter>(this)->GetBrain())
	{
		Cast<ABaseBotCharacter>(this)->GetBrain()->PauseBrain(false);
	}
}

void ABasePraiseCharacter::SetPlayMontageRestoreTimer(float CurrentAnimLenght, float PlayRate)
{
	FTimerDelegate Del;
	Del.BindUFunction(this, FName("OnMontageSectionPlayed"));

	if (GetWorld()->GetTimerManager().IsTimerActive(MontagePlayedRestoreTimer))
		GetWorld()->GetTimerManager().ClearTimer(MontagePlayedRestoreTimer);

	GetWorld()->GetTimerManager().SetTimer(MontagePlayedRestoreTimer, Del, CurrentAnimLenght/*(CurrentAnimLenght / PlayRate)*/, false);
}

void ABasePraiseCharacter::TriggerPickupItem(EWeaponSlot Slot)
{
	if (!CanExecuteAction(ECharAction::PICKUP_ITEM)) return;

	TArray<ITargeteable*> ClosestItems = GetClosestTargeteables(ETargetType::ITEM, true, 120.f);

	if (ClosestItems.Num() <= 0) return;

	OnItemCollected(Cast<AActor>(ClosestItems[0]), Slot);
}

void ABasePraiseCharacter::DropWeapon(EWeaponSlot WeaponSlot, bool bAddDefaultWeapon)
{
	if (!CanExecuteAction(ECharAction::DROP_WEAPON)) return;

	if (!CharWeaponSlots.Contains(WeaponSlot)) return;

	if (CharWeaponSlots[WeaponSlot]->HasDefaultFist()) return;

	AWeapon* DroppedWeapon = nullptr;

	if (!CharWeaponSlots[WeaponSlot]->IsEmpty())
	{
		EWeaponSlot SecondSlot = WeaponSlot == EWeaponSlot::PRIMARY_WEAPON ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON;

		EWeaponSlot DroppedHand = CharWeaponSlots[WeaponSlot]->GetEquipedHand();

		EWeaponSlot OtherHand = DroppedHand == EWeaponSlot::INVALID ? EWeaponSlot::INVALID : DroppedHand == EWeaponSlot::PRIMARY_WEAPON ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON;

		bool bWasEquiped = CharWeaponSlots[WeaponSlot]->IsEquiped();

		DroppedWeapon = CharWeaponSlots[WeaponSlot]->ClearSlot();

		DroppedWeapon->OnWeaponDropped();

		if (!DefaultUnarmedWeapon)
			CacheDefaultUnarmedWeapon();

		UCharWeaponSlot* DefaultFistSlot = nullptr;
		if (!HasSlotWeapon<AFist>(FString("Default"), DefaultFistSlot))
		{
			DefaultFistSlot = AuxSlot;
		}

		
			
		if (bWasEquiped && CurrentWeapons.Contains(DroppedHand))
		{
			if(!CurrentWeapons[DroppedHand]->HasDefaultFist())
				CurrentWeapons.Remove(DroppedHand);

			if (CurrentWeapons.Num() <= 0 && !bIsPickingupItem)
			{
				if (DefaultFistSlot)
				{
					CurrentWeapons.Add(EWeaponSlot::PRIMARY_WEAPON, DefaultFistSlot);
					CurrentWeapons.Add(EWeaponSlot::SECONDARY_WEAPON, DefaultFistSlot);

					DefaultFistSlot->SetIsEquiped(true, EWeaponSlot::PRIMARY_WEAPON);

					CurrentPose = EArmedPoses::UNARMED;
				}
			}
			else
			{
				if (CurrentWeapons.Contains(OtherHand) && !CurrentWeapons[OtherHand]->IsEmpty())
				{
					if (CurrentWeapons[OtherHand]->HoldsA<AFist>() || CurrentWeapons[OtherHand]->GetSlotWeapon()->GetWeaponClass() == EWeaponClass::SHIELD)
					{
						CurrentWeapons.Add(DroppedHand, DefaultFistSlot);

						if (!CurrentWeapons[OtherHand]->HoldsA<AFist>()) 
							DefaultFistSlot->SetIsEquiped(true, WeaponSlot);
					}

					CurrentPose = CurrentWeapons[OtherHand]->GetSlotWeapon()->GetCombatPose();
				}
			}
		}

		if (DroppedHand != EWeaponSlot::INVALID && PreviousWeapons.Contains(DroppedHand))
		{
			if (!PreviousWeapons[DroppedHand]->HasDefaultFist())
			{
				PreviousWeapons.Remove(DroppedHand);
				PreviousWeapons.Add(DroppedHand, DefaultFistSlot);
			}

			if (PreviousWeapons.Contains(OtherHand) && PreviousWeapons[OtherHand]->HoldsA<AFist>())
				PreviousWeapons.Add(OtherHand, PreviousWeapons[OtherHand]);
		}

		if (DefaultFistSlot && bAddDefaultWeapon && !CharWeaponSlots[WeaponSlot]->HasDefaultFist())
		{
			CharWeaponSlots[WeaponSlot]->SetSlotWeapon(DefaultFistSlot->GetSlotWeapon(), EHolsterPosition::UNARMED);
			DefaultFistSlot = CharWeaponSlots[WeaponSlot];
		}

		if ((CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->IsEmpty() || CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->HasDefaultFist()) && !CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->IsEmpty()  && !bIsPickingupItem)
		{
			EWeaponSlot SecondSlotHand = CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->GetEquipedHand();
			EHolsterPosition SecondSlotHolster = CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->GetHolsterPosition();
			
			if (IsWeaponEquiped())
			{
				CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->SetSlotWeapon(CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->GetSlotWeapon(), SecondSlotHolster);
				CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->SetIsEquiped(CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->IsEquiped(), SecondSlotHand);

				if (CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->IsEquiped())
				{
					if (CurrentWeapons.Contains(EWeaponSlot::PRIMARY_WEAPON))
					{
						if (CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->GetWeaponClass() != EWeaponClass::SHIELD)
							CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON] = CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON];
					}

					if (CurrentWeapons.Contains(EWeaponSlot::SECONDARY_WEAPON))
						CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON] = CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON];

					CurrentPose = GetCurrentMainWeapon()->GetSlotWeapon()->GetCombatPose();
				}
			}
			else
			{
				CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->SetSlotWeapon(CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->GetSlotWeapon(), SecondSlotHolster);
				CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->SetIsEquiped(false, SecondSlotHand);
			}

			if (AuxSlot)
			{
				if (!AuxSlot->HasDefaultFist())
					CacheDefaultUnarmedWeapon();

				if (!CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->HasDefaultFist())
				{
					CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->SetupSlot(EWeaponSlot::SECONDARY_WEAPON, AuxSlot->GetSlotWeapon());
					
					if (CurrentPose == EArmedPoses::UNARMED && IsWeaponEquiped())
					{
						CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->SetIsEquiped(true, EWeaponSlot::PRIMARY_WEAPON);

						if (CurrentWeapons.Contains(EWeaponSlot::PRIMARY_WEAPON))
							CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON] = CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON];

						else CurrentWeapons.Add(EWeaponSlot::PRIMARY_WEAPON, CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]);

						if (CurrentWeapons.Contains(EWeaponSlot::SECONDARY_WEAPON))
							CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON] = CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON];

						else CurrentWeapons.Add(EWeaponSlot::PRIMARY_WEAPON, CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]);
							
					}
					
					if (CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->IsEquiped() && CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->GetWeaponClass() == EWeaponClass::SHIELD)
					{
						CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->SetIsEquiped(true, EWeaponSlot::PRIMARY_WEAPON);
							
						if (CurrentWeapons.Contains(EWeaponSlot::PRIMARY_WEAPON))
						{
							CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON] = CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON];
						}

						else CurrentWeapons.Add(EWeaponSlot::PRIMARY_WEAPON, CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON]);
					}
				}
					
			}
			
			if (CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->HasDefaultFist() && CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->HasDefaultFist())
				CharWeaponSlots[EWeaponSlot::SECONDARY_WEAPON]->ClearSlot();
		}

		bIsDroppingWeapon = false;
	}
}

void ABasePraiseCharacter::PickWeapon(EWeaponSlot PickHand)
{
	if (!bIsPickingupItem) return;

	if (!CurrentlyPickedupItem) return;

	if (!CurrentlyPickedupItem->IsA<AWeapon>()) return;

	if (!PickWeaponSlot) return;

	if (PickWeaponSlot->IsLocked()) return;

	if (PickWeaponSlot->GetSlotWeapon() != CurrentlyPickedupItem) return;

	if (!CharWeaponSlots.Contains(AuxSlot->GetSlotID())) return;
		
	UCharWeaponSlot* SlotToUpdate = CharWeaponSlots[PickWeaponSlot->GetSlotID()];

	AWeapon* NewWeapon = Cast<AWeapon>(CurrentlyPickedupItem);

	AWeapon* DroppedWeapon = SlotToUpdate->ClearSlot();

	SlotToUpdate->SetupSlot(PickWeaponSlot->GetSlotID(), PickWeaponSlot->GetSlotWeapon(), PickWeaponSlot->IsEquiped(), PickWeaponSlot->GetEquipedHand());

	if (SlotToUpdate->IsEquiped())
	{
		if (PreviousWeapons.Contains(SlotToUpdate->GetEquipedHand()))
			PreviousWeapons.Remove(SlotToUpdate->GetEquipedHand());

		if (!CurrentWeapons.Contains(SlotToUpdate->GetEquipedHand()))
			CurrentWeapons.Add(SlotToUpdate->GetEquipedHand(), SlotToUpdate);

		else CurrentWeapons[SlotToUpdate->GetEquipedHand()] = SlotToUpdate;

		

		if (SlotToUpdate->GetSlotWeapon()->GetWeaponClass() == EWeaponClass::SHIELD)
		{
			if (!CurrentWeapons.Contains(EWeaponSlot::PRIMARY_WEAPON) && !NextSlotToEquip)
			{
				if (!DefaultUnarmedWeapon)
					CacheDefaultUnarmedWeapon();

				if (DefaultUnarmedWeapon)
				{
					AuxSlot->SetupSlot(EWeaponSlot::PRIMARY_WEAPON, DefaultUnarmedWeapon, true, EWeaponSlot::PRIMARY_WEAPON);
					CurrentWeapons.Add(EWeaponSlot::PRIMARY_WEAPON, AuxSlot);
				}
			}
		}
		else 
		{
			if (SlotToUpdate->IsEquiped())
			{
				EWeaponSlot OtherHand = SlotToUpdate->GetEquipedHand() == EWeaponSlot::PRIMARY_WEAPON ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON;

				if (CurrentWeapons.Contains(OtherHand) && CurrentWeapons[OtherHand]->HoldsA<AFist>())
					CurrentWeapons.Remove(OtherHand);
			}
			
		}
	}

	UCharWeaponSlot* MainWeaponSlot = GetCurrentMainWeapon();

	if (CurrentWeapons.Num() == 2) 
	{
		CurrentPose = MainWeaponSlot->HoldsA<AFist>() && CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON]->HoldsA<AFist>() ? EArmedPoses::UNARMED : EArmedPoses::DUALS;

		if (MainWeaponSlot->GetEquipedHand() == EWeaponSlot::SECONDARY_WEAPON && SlotToUpdate->GetSlotWeapon()->IsTwoHand())
		{
			UCharWeaponSlot* DefaultFist = nullptr;
			if (!HasSlotWeapon<AFist>(FString("Default"), DefaultFist))
			{
				ResetDefaultWeapon();

				DefaultFist = AuxSlot;
			}
				
			DefaultFist->SetIsEquiped(true, EWeaponSlot::PRIMARY_WEAPON);

			if (CurrentWeapons.Contains(EWeaponSlot::PRIMARY_WEAPON))
				CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON] = DefaultFist;

			else CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON] = DefaultFist;
		}
	}

	if (CurrentWeapons.Num() == 1)
	{
		CurrentPose = MainWeaponSlot->GetSlotWeapon()->GetCombatPose();
	}

	if (CurrentWeapons.Num() == 0)
	{
		CurrentPose = EArmedPoses::UNARMED;
	}

	SlotToUpdate->GetSlotWeapon()->OnWeaponPickup(this, PickHand);
	
	HandleWeaponSlotEquip(SlotToUpdate, SlotToUpdate->IsEquiped());
}

void ABasePraiseCharacter::StartSprint()
{
	if (!CanExecuteAction(ECharAction::RUN)) return;

	bIsRunning = true;

	HandleSprint();
}

void ABasePraiseCharacter::EndSprint()
{
	if (bIsRunning)
	{
		bIsRunning = false;

		HandleSprint();
	}
	
}

void ABasePraiseCharacter::StartJump()
{
	if (!IsInAir()) {
		bIsJumping = true;
		Jump();
	}
}

void ABasePraiseCharacter::EndJump()
{
	bIsJumping = false;
	StopJumping();
}

void ABasePraiseCharacter::StartCrouch()
{
	if (!IsDead())
		Crouch();
}

void ABasePraiseCharacter::EndCrouch()
{
	if(!IsDead())
		UnCrouch();
}

UCurveFloat* ABasePraiseCharacter::GetCurve(FString PathToCurve)
{
	UCurveFloat* Curve = Cast<UCurveFloat>(StaticLoadObject(UCurveFloat::StaticClass(), NULL, *PathToCurve));

	return Curve ? Curve : Cast<UCurveFloat>(StaticLoadObject(UCurveFloat::StaticClass(), NULL, *CURVE_PATH_LINEAR));
}

UCurveFloat* ABasePraiseCharacter::GetCurve(ECurveType Type)
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
bool ABasePraiseCharacter::TryLoadSpeedChangeCurve(ECurveType Type, bool bForceUpdate)
{
	if (SpeedChangeCurve && !bForceUpdate) return true;

	if (Type == CurrentSpeedChangeCurve) return true;

	UCurveFloat* Curve = GetCurve(Type);

	if (!Curve) return false;

	SpeedChangeCurve = Curve;

	CurrentSpeedChangeCurve = Type;

	return true;
}
void ABasePraiseCharacter::SetupCharArmor()
{
	if (DefaultArmorType == EArmorType::NONE) return;


}

void ABasePraiseCharacter::ResetAttackStuff()
{
	bCanAttack = true;
	bComboRequested = false;
	CurrentAttackWeapon = nullptr;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	if (AttackCounter > 1) {
		AttackCounter = 0;
		bIsCombo = false;
	}
}

void ABasePraiseCharacter::HandleAnimatorAttackRequest(UCharWeaponSlot* WeaponSlot)
{
	if (!IsWeaponEquiped() || bIsEquiping) return;

	if (!WeaponSlot->IsEquiped()) return;

	CurrentAttackWeapon = WeaponSlot->GetSlotWeapon();

	if (WeaponSlot->HoldsA<AFirearm>() && bIsAiming)
	{ 
	
		if (!WeaponSlot->IsEquiped()) return;

		bIsAttacking = true;
		bCanAttack = false;
		
		CurrentAttackWeapon->Attack(true);

		if (Cast<AFirearm>(CurrentAttackWeapon)->GetAmmo() <= 0 && Cast<AFirearm>(CurrentAttackWeapon)->GetTotalAmmo() > 0)
		{
			bIsAttacking = false;
			bIsReloading = true;

			return;
		}

		CurrentAttackType = EAttackType::SECONDARY_ATTACK;

		return;
	}
	else 
	{
		if (!bIsAttacking && bCanAttack) 
		{ 
			if (!bIsPlayingAnimation) 
			{
				bIsCombo = false;
				AttackCounter = 0;
				CurrentAttackType = EAttackType::PRIMARY_ATTACK;
				bIsAttacking = true;
				bCanAttack = false;
				bCanMove = false;
				bIsPlayingAnimation = true;

				HandleActionStaminaDecrease(ECharAction::ATTACK);				
			}
		}
	}
	
}
void ABasePraiseCharacter::HandleComboStuff(AWeapon* CurrentWeapon)
{
	if (this->IsA<ABaseBotCharacter>()) return;

	switch (CurrentWeapon->GetWeaponType()) 
	{
		case(EWeaponType::UNARMED):
		case(EWeaponType::POINTED):
		case(EWeaponType::BLUNT):
		case(EWeaponType::SHARP):
		case(EWeaponType::SHIELD):
		case(EWeaponType::FIREARM):
			if (bCanRequestCombo && AttackCounter <= 3) 
			{
				if (!CanExecuteAction(ECharAction::ATTACK))  return;

				bComboRequested = AttackCounter < 3;
				bIsCombo = bComboRequested;
			}

			break;
		default:
			break;
	}
}
void ABasePraiseCharacter::DropKick()
{
	if (!IsWeaponEquiped()) return;
	
	RequestedAttackType = EAttackType::KICK;

	if (!CanExecuteAction(ECharAction::ATTACK)) return;
	
	bCanMove = false;

	CurrentAttackType = EAttackType::KICK;

	bIsAttacking = true;

	bCanAttack = false;

	HandleActionStaminaDecrease(ECharAction::ATTACK);
}
void ABasePraiseCharacter::EquipSlotWeapon(UCharWeaponSlot* Slot, bool bIsEquip, EWeaponSlot Hand, bool bIsDualEquip, UCharWeaponSlot* SecondSlot, bool bShouldLinkAnims)
{
	if (!(Hand == EWeaponSlot::PRIMARY_WEAPON || Hand == EWeaponSlot::SECONDARY_WEAPON)) return; 

	if (Slot->IsEmpty()) return;

	if (!Slot->GetSlotWeapon()) return;

	EWeaponSlot OtherHand = Hand == EWeaponSlot::PRIMARY_WEAPON ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON;
	
	if (bIsEquip)
	{
		if (CurrentWeapons.Num() == 0) 
		{
			CurrentWeapons.Add(Hand, Slot);

			if (bIsDualEquip)
			{
				if ((SecondSlot && SecondSlot->GetSlotWeapon())) 
					CurrentWeapons.Add(OtherHand, SecondSlot);
			}
			else
			{
				if (Slot->HoldsA<AFist>())
					CurrentWeapons.Add(OtherHand, Slot);
				
				if (Slot->GetSlotWeapon()->GetWeaponClass() == EWeaponClass::SHIELD)
				{
					UCharWeaponSlot* DefaultFistSlot = nullptr;

					if (HasSlotWeapon<AFist>(FString("Default"), DefaultFistSlot))
					{
						CurrentWeapons.Add(OtherHand, DefaultFistSlot);
						SecondSlot = DefaultFistSlot;
					}
					else
					{
						if (AuxSlot)
						{
							CurrentWeapons.Add(OtherHand, AuxSlot);
							AuxSlot->SetIsEquiped(true, EWeaponSlot::PRIMARY_WEAPON);
						}
					}
				
				}
			}
		}
		else
		{
			if (CurrentWeapons.Contains(Hand))
				CurrentWeapons[Hand] = Slot;

			else CurrentWeapons.Add(Hand, Slot);

			if (bIsDualEquip)
			{
				if(SecondSlot && SecondSlot->GetSlotWeapon()) 
				{
					if (CurrentWeapons.Contains(OtherHand))
						CurrentWeapons[OtherHand] = SecondSlot;

					else CurrentWeapons.Add(OtherHand, SecondSlot);
				}
			}
			else
			{
				if (Slot->HoldsA<AFist>())
				{
					if (CurrentWeapons.Contains(OtherHand))
						CurrentWeapons[OtherHand] = Slot;

					else CurrentWeapons.Add(OtherHand, Slot);
				}
			}	
		}
	
		if (bIsDualEquip || CurrentWeapons.Num() == 2)
		{
			if (CurrentWeapons.Contains(EWeaponSlot::PRIMARY_WEAPON) && CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->HoldsA<AFist>() &&
				CurrentWeapons.Contains(EWeaponSlot::SECONDARY_WEAPON) && CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON]->HoldsA<AFist>())
			{
				CurrentPose = EArmedPoses::UNARMED;
			}

			else CurrentPose = EArmedPoses::DUALS;
		}

		else CurrentPose = Slot->GetSlotWeapon()->GetCombatPose();
	}
	else 
	{ 

		if ((bIsDualEquip && SecondSlot && SecondSlot->GetSlotWeapon()) || Slot->HoldsA<AFist>()) 
		{
			for (int i = 0; i < CurrentWeapons.Num(); i++) 
				CurrentWeapons[(EWeaponSlot)i]->SetIsEquiped(false, EWeaponSlot::INVALID);

			CurrentWeapons.Empty();
		}
		
		else
		{
			CurrentWeapons.Remove(Hand);
			if (Slot->GetSlotWeapon()->GetWeaponClass() == EWeaponClass::SHIELD)
			{
				if (CurrentWeapons.Contains(EWeaponSlot::PRIMARY_WEAPON) && CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->HoldsA<AFist>() && CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->GetWeaponModel() == FString("Default"))
				{
					SecondSlot = CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON];
					
					CurrentWeapons.Remove(EWeaponSlot::PRIMARY_WEAPON);
					
					SecondSlot->SetIsEquiped(false, EWeaponSlot::INVALID);
				}

				if (!bIsDualEquip)
				{
					UCharWeaponSlot* DefaultFist = nullptr;

					SecondSlot = HasSlotWeapon<AFist>(FString("Default"), DefaultFist) ? DefaultFist : AuxSlot;
				}
			}
		}
		
		PreviousWeapons.Empty();

		PreviousWeapons.Add(Hand, Slot);

		if (Slot->HoldsA<AFist>() && !bIsDualEquip)
			PreviousWeapons.Add(OtherHand, Slot);

		if (/*bIsDualEquip && */SecondSlot && SecondSlot->GetSlotWeapon())
			PreviousWeapons.Add(OtherHand, SecondSlot);
	}
	
	Slot->SetIsEquiped(bIsEquip, bIsEquip ? Hand : EWeaponSlot::INVALID);
	
	if (SecondSlot)
		SecondSlot->SetIsEquiped(bIsEquip, bIsEquip ? OtherHand : EWeaponSlot::INVALID);

	bIsEquippingWeapon = bIsEquip;
	EquippingHand = bIsDualEquip ? 2 : (int)Hand;
	bIsDualEquipAnim = bIsDualEquip; 
	bShouldLinkEquipAnims = bShouldLinkAnims;
	bIsEquiping = true;	
	bDamageAnimDisabled = true;
}
void ABasePraiseCharacter::AttachWeapon(AWeapon* Weapon, FName Socket, bool bIsVisible)
{
	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
	Weapon->AttachToComponent(GetMesh(), AttachmentRules, Socket);
	Weapon->SetActorRelativeRotation(Weapon->GetEquipRotation());
	Weapon->SetActorRelativeLocation(Weapon->GetEquipLocation());
	Weapon->WeaponMesh->SetVisibility(false, true);
	Weapon->SetOwner(this);
}

void ABasePraiseCharacter::AttachWeapon(AWeapon* Weapon, EHolsterPosition Holster)
{
	const FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);

	Weapon->AttachToComponent(GetMesh(), AttachmentRules, FUtilities::GetSocketName(Holster));
	Weapon->SetOwner(this);

	FVector BackLoc;
	FRotator BackRot;
	Weapon->GetHolsterCoords(Holster, BackLoc, BackRot);
	Weapon->SetActorRelativeLocation(BackLoc);
	Weapon->SetActorRelativeRotation(BackRot);
	Weapon->SetActorRelativeScale3D(Weapon->GetWeaponScale());
	Weapon->WeaponMesh->SetVisibility(true, true);
	Weapon->ReattachAudioComp();
}

void ABasePraiseCharacter::TrySwitchWeapon(EWeaponSlot RequestedSlotID)
{
	if (RequestedSlotID == EWeaponSlot::INVALID) return;

	if (!CharWeaponSlots.Contains(RequestedSlotID)) return;

	UCharWeaponSlot* RequestedSlot = CharWeaponSlots[RequestedSlotID];
	
	if (RequestedSlot->IsLocked() || RequestedSlot->IsEmpty()) return;
	
	if (IsWeaponEquiped()) 
	{
		EWeaponSlot SlotHand = RequestedSlot->GetHolsterEquippingHand();

		EWeaponSlot OtherHand = SlotHand == EWeaponSlot::PRIMARY_WEAPON ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON;

		if (CurrentWeapons.Contains(SlotHand) && CurrentWeapons[SlotHand] == RequestedSlot)
		{
			if (CurrentPose == EArmedPoses::DUALS)
			{
				if (RequestedSlot->HoldsA<AFist>())
				{
					SetNextSlotToEquip(RequestedSlotID, RequestedSlot);
					bIsSwitchingWeapons = true;
					EquipWeapons();
				}

				else EquipSlotWeapon(RequestedSlot, false, RequestedSlot->GetEquipedHand(), false);
				
			}

			else EquipWeapons(RequestedSlot->GetEquipedHand());
		}
		else 
		{	
			if (CurrentPose == EArmedPoses::DUALS)
			{
				if (RequestedSlot->HoldsA<AFist>())
				{
					SetNextSlotToEquip(RequestedSlotID, RequestedSlot);
					bIsSwitchingWeapons = true;

					if (CurrentWeapons.Contains(EWeaponSlot::SECONDARY_WEAPON) && CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON]->GetSlotWeapon()->GetWeaponClass() == EWeaponClass::SHIELD)
						EquipSlotWeapon(CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON], false, EWeaponSlot::SECONDARY_WEAPON, true, CharWeaponSlots[RequestedSlotID]);
					
					else EquipWeapons();
				}
				else
				{
					if (!RequestedSlot->GetSlotWeapon()->IsTwoHand())
					{
						if (CurrentWeapons.Contains(OtherHand) && !CurrentWeapons[OtherHand]->GetSlotWeapon()->IsTwoHand() && !CurrentWeapons[OtherHand]->HoldsA<AFist>())
							EquipSlotWeapon(RequestedSlot, true, /*EWeaponSlot::SECONDARY_WEAPON*/SlotHand, false);
						else
						{
							SetNextSlotToEquip(RequestedSlotID, RequestedSlot);
							bIsSwitchingWeapons = true;
							EquipWeapons();
						}
					}
					else
					{
						SetNextSlotToEquip(RequestedSlotID, RequestedSlot);
						bIsSwitchingWeapons = true;
						EquipWeapons();
					}
				}
			}
			else
			{
				if (CurrentWeapons.Contains(SlotHand))
				{
					SetNextSlotToEquip(RequestedSlotID, RequestedSlot);
					bIsSwitchingWeapons = true;
					EquipWeapons();
				}
				else
				{
					if (!RequestedSlot->GetSlotWeapon()->IsTwoHand())
					{
						if (RequestedSlot->HoldsA<AFist>())
						{
							SetNextSlotToEquip(RequestedSlotID, RequestedSlot);
							bIsSwitchingWeapons = true;
							EquipWeapons();
						}
						else
						{
							if (!RequestedSlot->GetSlotWeapon()->IsTwoHand() && CurrentWeapons.Contains(OtherHand) && !CurrentWeapons[OtherHand]->GetSlotWeapon()->IsTwoHand())
							{
								EquipSlotWeapon(RequestedSlot, true, SlotHand, false);
							}
							else
							{
								SetNextSlotToEquip(RequestedSlotID, RequestedSlot);
								bIsSwitchingWeapons = true;
								EquipWeapons();
							}
						}
					}
					else
					{
						SetNextSlotToEquip(RequestedSlotID, RequestedSlot);
						bIsSwitchingWeapons = true;
						EquipWeapons();
					}
				}
			}
			
			return;
		}
	}

	else EquipWeapons(RequestedSlotID, /*bTryDualEquip:*/ false); 
}

void ABasePraiseCharacter::SetNextSlotToEquip(EWeaponSlot RequestedSlot, UCharWeaponSlot* Slot)
{
	if (RequestedSlot == EWeaponSlot::INVALID || !Slot) 
	{
		RequestedSlot = EWeaponSlot::INVALID;
		NextSlotToEquip = nullptr;
	}
	else 
	{
		RequestedWeaponSlotID = RequestedSlot;
		NextSlotToEquip = Slot;
	}
}

void ABasePraiseCharacter::PlaySoundQ(USoundCue* SoundQ, FName SwitchParamName, int ParamValue, FName SubParamName, int SubParamValue)
{
	if (!AudioComp) return;

	if (!SoundQ->IsPlayable()) return;

	AudioComp->SetSound(SoundQ);
	
	AudioComp->SetIntParameter(SwitchParamName, ParamValue);

	if (!SubParamName.IsNone() && SubParamValue >= 0)
		AudioComp->SetIntParameter(SubParamName, SubParamValue);

	AudioComp->Play();
}

// Called every frame
void ABasePraiseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHandleBlendspaceTL)
		BlendSpace1DTL.TickTimeline(DeltaTime);

	if (bHandleWeaponRot)
		HandleWeaponRotation(DeltaTime);

	ManageStamina(DeltaTime);
}

float ABasePraiseCharacter::PlayMontage(UAnimMontage* Montage, FName Section, float Speed, bool bAddAnimPlayedTimer, bool bWithRootMot, bool bNetCast)
{
	int32 SectionIdx = Montage->GetSectionIndex(Section);

	if (!Montage) return 0.f;

	if (!GetMesh()) return 0.f;

	if (!GetMesh()->GetAnimInstance()) return 0.0f;

	float AnimDuration = Montage->GetSectionLength(SectionIdx);

	if(bAddAnimPlayedTimer)
		SetPlayMontageRestoreTimer(AnimDuration, Speed);

	if (bNetCast)
		Server_PlayMontage(Montage, Section, Speed, bAddAnimPlayedTimer);

	else PlayAnimMontage(Montage, Speed, Section);

	return Montage->GetSectionLength(SectionIdx);
}

void ABasePraiseCharacter::Server_PlayMontage_Implementation(UAnimMontage* Montage, FName Section, float Speed, bool bAddAnimPlayedTimer)
{
	NetCast_PlayMontage(Montage, Section, Speed, bAddAnimPlayedTimer);
}

void ABasePraiseCharacter::NetCast_PlayMontage_Implementation(UAnimMontage* Montage, FName Section, float Speed, bool bAddAnimPlayedTimer)
{
	PlayAnimMontage(Montage, Speed, Section);
}



void ABasePraiseCharacter::UpdateWalkSpeed(ECurveType CurveType)
{
	if (bHandleSpeedChange) return;

	if (!GetCharacterMovement() || !CharStats) return;

	if (!TryLoadSpeedChangeCurve(CurveType)) return;

	SetSpeedChangeTL();

	bHandleSpeedChange = true;
}

void ABasePraiseCharacter::HandleWalkSpeed(float Value, bool bIsMultiplier, bool bSetTimeline, ECurveType CurveType, float SpeedTLPlayRate)
{
	if (Value == 0) return;
	
	if (!GetCharacterMovement() || !CharStats) return;

	if (!TryLoadSpeedChangeCurve(CurveType)) return;

	switch (bIsMultiplier) {
		case(true):
			CharStats->IncreaseSpeed(Value);
			break;
		case(false):
			CharStats->SetWalkSpeed(Value);
			break;
	}

	SetSpeedChangeTL(SpeedTLPlayRate);

	bHandleSpeedChange = true;
}

void ABasePraiseCharacter::EnableStatsBar(bool bEnable)
{
	if (!CharStatsBarWidget) return;

	ESlateVisibility Visibility = bEnable ? ESlateVisibility::Visible : ESlateVisibility::Hidden;

	CharStatsBarWidget->GetWidget()->SetVisibility(Visibility);

	switch (CharStatsBarWidget->GetWidget()->IsVisible())
	{
		case(true):
			if(!bEnable)
				CharStatsBarWidget->GetWidget()->SetVisibility(Visibility);
			break;
		case(false):
			if(bEnable)
				CharStatsBarWidget->GetWidget()->SetVisibility(Visibility);
			break;
	}
}

void ABasePraiseCharacter::SetupCharStatsBar()
{
	if (!this->Implements<UCharStatsInterface>()) return;

	if (!CharStatsBarWidget) return;

	if (!CharStatsBarBP.Get()) return;

	CharStatsBarWidget->SetWidgetClass(CharStatsBarBP);
	CharStatsBarWidget->InitWidget();
	
	if(!CharStatsBarWidget->GetWidget()->IsA<UCharStatsBarWidget>()) return;

	UCharStatsBarWidget* StatsWidget = Cast<UCharStatsBarWidget>(CharStatsBarWidget->GetWidget());

	if (!StatsWidget) return;

	StatsWidget->SetWidgetOwner(Cast<ICharStatsInterface>(this));

	EnableStatsBar(false);
}

void ABasePraiseCharacter::SetupTargetWidget()
{
	if (!TargetWidgetBP) return;

	if (!TargetWidgetBP.Get()) return;
	
	TargetWidget->SetWidgetClass(TargetWidgetBP.Get());
	
	TargetWidget->InitWidget();

	TargetWidget->SetVisibility(true, true);

	TargetWidget->SetWidgetSpace(EWidgetSpace::Screen);

	EnableTargetWidget(true);
}

void ABasePraiseCharacter::EnableTargetWidget(bool bEnable)
{
	if (!TargetWidget) return;

	if (TargetWidget->GetWidget())
	{
		ESlateVisibility Visibility = bEnable ? ESlateVisibility::Visible : ESlateVisibility::Hidden;

		TargetWidget->GetWidget()->SetVisibility(Visibility);
	}
}

void ABasePraiseCharacter::PlayCombatSFX(ECombatSFX FX)
{
	if (!AudioComp) return;

	if (!CombatSoundCue) return;

	if (FX == ECombatSFX::NONE) return;

	PlaySoundQ(CombatSoundCue, FName("CombatFX"), (int)FX);

}

void ABasePraiseCharacter::PlayCombatSFX(ECombatSFX FX, FName SubParamName, int SubParamValue)
{
	if (FX == ECombatSFX::NONE) return;

	if (SubParamValue < 0) return;

	PlaySoundQ(CombatSoundCue, FName("CombatFX"), (int)FX, SubParamName, SubParamValue);
}

void ABasePraiseCharacter::PlayMiscSFX(EMiscSFX FX)
{
	if (!AudioComp) return;

	if (!MiscSoundCue) return;

	if (FX == EMiscSFX::NONE) return;

	PlaySoundQ(MiscSoundCue, FName("MiscFX"), (int)FX);
}

void ABasePraiseCharacter::PlayMiscSFX(EMiscSFX FX, FName SubParamName, int SubParamValue)
{
	if (!AudioComp) return;

	if (!MiscSoundCue) return;

	if (FX == EMiscSFX::NONE) return;

	PlaySoundQ(MiscSoundCue, FName("MiscFX"), (int)FX, SubParamName, SubParamValue);
}

void ABasePraiseCharacter::PlayCharSFX(ECharSFX FX, FName SubParamName, int SubParamValue)
{
	if (!AudioComp) return;

	if (!CharSoundCue) return;

	if (FX == ECharSFX::NONE) return;

	PlaySoundQ(CharSoundCue, FName("CharFX"), (int)FX, SubParamName, SubParamValue);
}

void ABasePraiseCharacter::PlayCharSFX(ECharSFX FX)
{
	if (!AudioComp) return;

	if (!CharSoundCue) return;

	if (FX == ECharSFX::NONE) return;

	FName ParamName = FUtilities::GetCharSFXParamName(FX);

	int SampleIdx = FMath::RandRange(0, 5);

	AudioComp->SetIntParameter(FName("VoiceSample"), (int)CharType);

	PlaySoundQ(CharSoundCue, FName("CharFX"), (int)FX, ParamName, SampleIdx);
}

void ABasePraiseCharacter::PlayFootstepSFX()
{
	if (!FootstepsAudioComp) return;

	if (!CharSoundCue) return;

	FName ParamName = FUtilities::GetMiscSFXParamName(EMiscSFX::FOOTSTEPS);

	int SampleIdx = FMath::RandRange(0, 3);

	if (!AudioComp) return;

	if (!MiscSoundCue->IsPlayable()) return;

	FootstepsAudioComp->SetSound(MiscSoundCue);

	FootstepsAudioComp->SetIntParameter(FName("MiscFX"), (int)EMiscSFX::FOOTSTEPS);

	if (!ParamName.IsNone() && SampleIdx >= 0)
		FootstepsAudioComp->SetIntParameter(ParamName, SampleIdx);

	FootstepsAudioComp->Play();
}


void ABasePraiseCharacter::RestoreWalkSpeed(bool bSetTimeline, ECurveType CurveType)
{
	if (!GetCharacterMovement() || !CharStats) return;

	if (!TryLoadSpeedChangeCurve(CurveType)) return;

	CharStats->RestoreDefaultSpeed();

	if (bSetTimeline)
		UpdateWalkSpeed(CurveType);

	else bHandleSpeedChange = true;
}

void ABasePraiseCharacter::ResetEquipingStuff()
{
	OnAnimStateUpdate(ECharAnimState::EQUIPING, false);
}

void ABasePraiseCharacter::RotateWeapon(EWeaponSlot WeaponHand, FRotator DesiredRot)
{
	if (CurrentWeapons[WeaponHand]->IsEmpty()) return;

	if (WeaponTargetRotation != FRotator::ZeroRotator)
		CurrentWeaponRot = WeaponTargetRotation;

	else CurrentWeaponRot = DesiredRot * -1;
	
	WeaponTargetRotation = DesiredRot;

	WeaponToRotate = WeaponHand;

	bHandleWeaponRot = true;
}

void ABasePraiseCharacter::HandleMessage(FTelegram& Msg)
{
	if (!Msg.IsValidMsg()) return;

	if (Msg.Sender == GetUniqueID()) return;

	if (Msg.IsDirectMsg() && Msg.Cmd->GetReceiver()->GetUniqueID() != GetUniqueID()) return; 

	if (!CharFaction) return;

	UBaseMsgCommand* Clone = CharFaction->GetMsgComandsFactory()->CloneCmd(Msg.Cmd);

	if (!Clone) return;

	Clone->SetReceiver(this);

	Clone->ExecuteCommand();
}

void ABasePraiseCharacter::SendMessage(EAI_MsgChannel Channel, FTelegram& Msg)
{
	if (!Msg.IsValidMsg()) return; 

	UMsgDispatcherComponent* Dispatcher = nullptr;

	switch (Channel)
	{
		case(EAI_MsgChannel::GENERAL):
			break;
		case(EAI_MsgChannel::FACTION):
			if (CharFaction)
				Dispatcher = CharFaction->GetMsgDispatcher();
			break;
		case(EAI_MsgChannel::ZONE):
			if (CharSpawnZone)
				Dispatcher = CharSpawnZone->GetMsgDispatcher();
			break;
		case(EAI_MsgChannel::TEAM):
			break;
		default:
			break;
	}

	if (!Dispatcher) return;

	Dispatcher->SendMsg(Msg);	
}

void ABasePraiseCharacter::PingMessage(UBaseMsgCommand* MsgCommand)
{
	MsgCommand->SetSender(this);

	FTelegram Msg = FTelegram(MsgCommand->GetCmdMsg(), GetUniqueID());

	Msg.SetMsgCmd(MsgCommand, MsgCommand->GetMsgChannel());

	SendMessage(MsgCommand->GetMsgChannel(), Msg);
}

float ABasePraiseCharacter::GetDirectionDegrees()
{
	if (GetVelocity().IsNearlyZero()) return 0;

	FRotationMatrix RotMat = FRotationMatrix(GetActorRotation());
	FVector Fwd = RotMat.GetScaledAxis(EAxis::X);
	FVector Right = RotMat.GetScaledAxis(EAxis::Y);
	FVector VelNormal = GetVelocity().GetSafeNormal2D();

	float FwdCos = FVector::DotProduct(Fwd, VelNormal);
	float FwdDegs = FMath::RadiansToDegrees(FMath::Acos(FwdCos));
	float RightCos = FVector::DotProduct(Right, VelNormal);

	if (FwdDegs > 175)
		return FwdDegs * -1;

	return RightCos < 0 ? FwdDegs * -1 : FwdDegs;
}

ECharVector ABasePraiseCharacter::GetDirectionVector(float DirDegrees)
{
	if (FMath::Abs(DirDegrees) < 15)
		return ECharVector::FWD;
	if (FMath::Abs(DirDegrees) > 160)
		return ECharVector::BWD;
	if (FMath::Abs(DirDegrees) >= 15 && FMath::Abs(DirDegrees) < 80)
		return FMath::Sign(DirDegrees) < 0 ? ECharVector::FWD_L : ECharVector::FWD_R;
	if (FMath::Abs(DirDegrees) >= 80 && FMath::Abs(DirDegrees) < 110)
		return FMath::Sign(DirDegrees) < 0 ? ECharVector::LEFT : ECharVector::RIGHT;
	if (FMath::Abs(DirDegrees) >= 110 && FMath::Abs(DirDegrees) < 160)
		return FMath::Sign(DirDegrees) < 0 ? ECharVector::BWD_L : ECharVector::BWD_R;

	return ECharVector::FWD;
}


void ABasePraiseCharacter::SetCharacterDead(AActor* Killer)
{
	PlayCharSFX(ECharSFX::CHAR_DEAD);

	if (GetMesh()) 
	{
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DetachFromControllerPendingDestroy();
		GetMesh()->SetCollisionProfileName("Ragdoll");
		GetMesh()->SetSimulatePhysics(true);
	}

	if (!HasAuthority()) 
		Server_HandleCharDeadNotify(this, Killer);
	
	else HandleCharDeadNotify(this, Killer);
	
	GetWorld()->GetTimerManager().SetTimer(DestroyCharTimerHandle, this, &ABasePraiseCharacter::DestroyCharacter, DestroyCharCountdown, false);
}

void ABasePraiseCharacter::UpdateCharStatus(ECharStatus NewStatus)
{
	if (IsDead()) return;

	CharStatus = NewStatus;
	FTimerDelegate Del;
	switch (CharStatus)
	{
		case(ECharStatus::ALERTED):

			if (!ensure(GetWorld() != nullptr)) return;

			Del.BindUFunction(this, FName("UpdateCharStatus"), ECharStatus::NORMAL);
			
			if (GetWorld()->GetTimerManager().IsTimerActive(AlertedStateTimerHandle)) 
			{
				GetWorld()->GetTimerManager().ClearTimer(AlertedStateTimerHandle);
			}

			GetWorld()->GetTimerManager().SetTimer(AlertedStateTimerHandle, Del, AlertedStateSeconds, false);
			
			break;
		case(ECharStatus::BLINDED):
			break;
		case(ECharStatus::STUNNED):

			break;
		case(ECharStatus::DAMAGED):
			break;
		case(ECharStatus::VERY_DAMAGED):
			break;
		case(ECharStatus::SCARED):
			break;
		case(ECharStatus::IN_PANIC):
			break;
		case(ECharStatus::NORMAL):
		default:

			if (this->IsA<ABaseBotCharacter>())
			{
				ABaseBotCharacter* Bot = Cast<ABaseBotCharacter>(this);

				if (Bot->GetBrain() && Bot->GetBrain()->GetKnownEnemies().Num() > 0)
					UpdateCharStatus(ECharStatus::ALERTED);
			}
			
			if (GetHealth() < 50.f)
				UpdateCharStatus(ECharStatus::DAMAGED);
			if (GetHealth() < 20.f)
				UpdateCharStatus(ECharStatus::VERY_DAMAGED);
			break;
	}

}

void ABasePraiseCharacter::HandleCharDeadNotify(AActor* Victim, AActor* Killer)
{
	if (Victim->GetUniqueID() == GetUniqueID())
	{
		if (bHasBuilding)
			CharHome->OnBuildingCharDead(this);

		if (OnNotifyDead.IsBound())
			OnNotifyDead.Broadcast(Victim, Killer);
		
		return;
	}

	if (TargetingComponent && bIsTargeting && TargetingComponent->IsTargetLocked())
	{
		if (GetLockedTarget() && GetLockedTarget() == Victim) {
			TargetingComponent->ClearTarget();
			bIsTargeting = false;
		}
	}
}

void ABasePraiseCharacter::Server_HandleCharDeadNotify_Implementation(AActor* Victim, AActor* Killer)
{


	if (OnNotifyDead.IsBound())
		OnNotifyDead.Broadcast(this, Killer);

	NetCast_HandleCharDeadNotify(Victim, Killer);
}

void ABasePraiseCharacter::NetCast_HandleCharDeadNotify_Implementation(AActor* Victim, AActor* Killer)
{
	//TODO: networking features
}


bool ABasePraiseCharacter::HasTargetsAtSight(FVector OriginLoc, bool bCheckOnlyEnemies, float SweepRange)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	
	if (bCheckOnlyEnemies && this->IsA<ABaseBotCharacter>()) {
		QueryParams.AddIgnoredActors((Cast<ABaseBotCharacter>(this))->GetBrain()->GetKnownAllies());
		QueryParams.AddIgnoredActors((Cast<ABaseBotCharacter>(this))->GetBrain()->GetKnownItems());
	}

	FCollisionShape ColShape = FCollisionShape::MakeSphere(SweepRange);
	TArray<FHitResult> OutHits;
	if (GetWorld()->SweepMultiByChannel(OutHits, OriginLoc, OriginLoc, FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel3, ColShape, QueryParams))
	{
		for (int i = 0; i < OutHits.Num(); i++)
		{
			if (!OutHits[i].Actor.IsValid()) continue;

			if (!OutHits[i].Actor.Get()->IsA<ABasePraiseCharacter>()) continue;

			ABasePraiseCharacter* Target = Cast<ABasePraiseCharacter>(OutHits[i].Actor.Get());

			if (bCheckOnlyEnemies && !IsEnemyTarget(Cast<IFactioneable>(Target))) continue;

			if (CanSeeTarget(Target, true))
			{	
				if (this->IsA<ABaseBotCharacter>())
					Cast<ABaseBotCharacter>(this)->GetBrain()->TryAddNewTarget(Cast<ITargeteable>(OutHits[i].Actor.Get()));

				return true;
			}
		}
	}

	return false;
}

bool ABasePraiseCharacter::CanSeeTarget(AActor* Target, bool bWithSightRangeCheck, bool bOnlyCharCollisions)
{
	if (!ensure(GetWorld() != nullptr)) return false;

	if (!Target) return false;

	FVector EyesLoc;
	FRotator ViewDir;

	GetActorEyesViewPoint(EyesLoc, ViewDir);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	FHitResult OutHit;

	bool bCanSeeTarget = false;
	
	if (GetWorld()->LineTraceSingleByChannel(OutHit, EyesLoc, Target->GetActorLocation(), ECollisionChannel::ECC_GameTraceChannel4, QueryParams)) {
		if (OutHit.Actor != nullptr) 
		{
			if (bOnlyCharCollisions && !OutHit.Actor.Get()->IsA<ABasePraiseCharacter>()) return false;

			if (OutHit.Actor->GetUniqueID() == Target->GetUniqueID())
			{
				if (bWithSightRangeCheck)
					return (OutHit.Actor->GetActorLocation() - GetActorLocation()).Size() <= CharStats->GetSightRange();

				else return true;
			}

			return false;
		}
	}
		
	return false;
}


bool ABasePraiseCharacter::HasObstacleInBetween(AActor* Target)
{
	if (!ensure(GetWorld() != nullptr)) return false;

	if (!Target) return false;

	FVector EyesLoc;
	FRotator ViewDir;

	GetActorEyesViewPoint(EyesLoc, ViewDir);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	FHitResult OutHit;

	bool bCanSeeTarget = false;

	if (GetWorld()->LineTraceSingleByChannel(OutHit, EyesLoc, Target->GetActorLocation(), ECollisionChannel::ECC_GameTraceChannel5, QueryParams)) 
	{
		return true;
	}

	return false;
}

bool ABasePraiseCharacter::IsFacingObstacle()
{
	FVector EyesLoc;
	FRotator ViewDir;

	GetActorEyesViewPoint(EyesLoc, ViewDir);
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AllStaticObjects;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	FColor TraceColor = FColor::Red;
	FHitResult OutHit;

	if (GetWorld()->LineTraceSingleByChannel(OutHit, EyesLoc, EyesLoc + (ViewDir.Vector() * 125.f), ECollisionChannel::ECC_GameTraceChannel4, QueryParams))
		return !OutHit.Actor.Get()->IsA<APawn>();

	return false;
}

void ABasePraiseCharacter::SetSpeedChangeTL(float SpeedTLPlayRate)
{
	FTimeline tl;
	SpeedChangeTL = tl;
	SpeedChangeTL.SetNewTime(0.f);
	FOnTimelineFloat OnSpeedChangeTimelineFloat;
	FOnTimelineEventStatic OnTimelineFinished;
	OnSpeedChangeTimelineFloat.BindUFunction(this, FName("HandleSpeedChangeTL"));
	OnTimelineFinished.BindUFunction(this, FName("OnTimelineFinished"));
	SpeedChangeTL.SetTimelineFinishedFunc(OnTimelineFinished);
	SpeedChangeTL.AddInterpFloat(SpeedChangeCurve, OnSpeedChangeTimelineFloat);
	SpeedChangeTL.SetPlayRate(SpeedTLPlayRate);
	SpeedChangeTL.SetLooping(false);
	SpeedChangeTL.PlayFromStart();
}

void ABasePraiseCharacter::HandleSpeedChangeTL(float CurveValue)
{
	if (!GetCharacterMovement() || !CharStats) return;

	float Speed = FMath::Lerp(GetCharacterMovement()->MaxWalkSpeed, CharStats->GetWalkSpeed(), CurveValue);

	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void ABasePraiseCharacter::OnTimelineFinished()
{
	bHandleSpeedChange = false;
}

bool ABasePraiseCharacter::IsTargetAside(FVector TargLoc, ECharVector Side, float FwdAxisToleranceDegs) const
{
	FVector ToTarg = (TargLoc - GetActorLocation()).GetSafeNormal();
	
	ToTarg.Z = 0;
	
	float Degs = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorForwardVector(), ToTarg)));

	FVector Cross = FVector::CrossProduct(GetActorForwardVector(), ToTarg);

	if (Degs > FwdAxisToleranceDegs && Degs < 180 - FwdAxisToleranceDegs)
	{
		if (Side == ECharVector::LEFT)
			return FMath::Sign(Cross.Z) < 0;
		if(Side == ECharVector::RIGHT)
			return FMath::Sign(Cross.Z) > 0;
	}

	return false;
}

bool ABasePraiseCharacter::IsTargetBackward(FVector TargLoc, float ToleranceDegs) const
{
	FVector Dir = (TargLoc - GetActorLocation()).GetSafeNormal();

	float AngleDegs = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorForwardVector(), Dir)));

	return FMath::Abs(AngleDegs) >= 180 - ToleranceDegs;
}

bool ABasePraiseCharacter::IsTargetForward(FVector TargLoc, float ToleranceDegs) const
{
	FVector Dir = (TargLoc - GetActorLocation()).GetSafeNormal();

	float AngleDegs = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorForwardVector(), Dir)));

	return FMath::Abs(AngleDegs) <= ToleranceDegs;
}


bool ABasePraiseCharacter::IsTargetForward(AActor* Target, float ToleranceDegs)
{	
	return IsTargetForward(Target->GetActorLocation(), ToleranceDegs);
}



/*
* https://www.reddit.com/r/unrealengine/comments/93th8y/help_trying_myself_at_coneshaped_linetrace_casting/
*
	- One thing you might try is a simpler calculation, such as treating the problem as a 2D triangle:

		FinalPos = StartPos + ForwardDir * Cos(Angle) * TraceLength + RightDir * Sin(Angle) * TraceLength

	- Another approach would be to simply check if the player is within the cone angle without doing a bunch of line traces:

		VecToPlayer = PlayerPos - EnemyPos
		if VecToPlayer.Length <= TraceLength:
			Angle = ACos(DotProduct(EnemyForwardDir, Normalize(VecToPlayer)))
			if Angle <= 90 and LightOfSightTo(Player):
				Successful Hit
*/
bool ABasePraiseCharacter::IsTargetInsideFOV(AActor* Target, bool bCheckCol, float MaxAngle)
{
	if (bCheckCol)
		if (!CanSeeTarget(Target)) 
			return false;

	FVector VectorToTarget = Target->GetActorLocation() - GetActorLocation();

	FVector Dir = VectorToTarget.GetSafeNormal();

	float Angle = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), Dir));

	float Degrees = FMath::RadiansToDegrees(Angle);
	
	return Degrees <= (MaxAngle == 0.f ? GetFieldOfView() : MaxAngle);
}

TArray<ITargeteable*> ABasePraiseCharacter::GetClosestTargeteables(ETargetType Type, bool bGetClosest, float SweepRadius, float MinDistance)
{
	TArray<ITargeteable*> Results = TArray<ITargeteable*>();

	TArray<AActor*> ClosestPawns = GetSweepResult(GetActorLocation(), SweepRadius, true, ECollisionChannel::ECC_GameTraceChannel3, TArray<AActor*>{ this });

	float Distance = 0;
	AActor* Closest = nullptr;
	for (AActor* Pawn : ClosestPawns)
	{
		if (!Pawn->Implements<UTargeteable>()) continue;

		FVector ToPawn = Pawn->GetActorLocation() - GetActorLocation();

		if (MinDistance > 0 && ToPawn.Size() > MinDistance) continue;

		if (Distance == 0 || ToPawn.Size() < Distance)
		{
			Closest = Pawn;
			Distance = ToPawn.Size();

			Results.Add(Cast<ITargeteable>(Pawn));
		}
	}

	return Closest == nullptr || !bGetClosest ? Results : TArray<ITargeteable*>{ Cast<ITargeteable>(Closest) };
}

void ABasePraiseCharacter::OnItemCollected(AActor* Item, EWeaponSlot RequestedSlot)
{
	FVector ToItem = Item->GetActorLocation() - GetActorLocation();

	float Degs = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorForwardVector(), ToItem.GetSafeNormal())));

	if (Degs > 20.f)
	{
		//RotateToItem
		//TODO
	}

	bool bPickFromGround = ToItem.Z < 60.f;

	if (Item->IsA<AWeapon>()) //
	{
		HandlePickupWeapon(Cast<AWeapon>(Item), RequestedSlot, bPickFromGround);

		return;
	}
}


bool ABasePraiseCharacter::CheckBodyCollisionToByChannel(AActor* Target, TArray<AActor*>& Hitten, bool bOverlap, FVector Origin, TArray<AActor*> IgnoredActors, float ShapeMult, ECollisionChannel Channel)
{
	return CheckBodyCollisionToByChannel(Target->GetActorLocation(), Hitten, bOverlap, Origin, IgnoredActors, ShapeMult, Channel);
}

bool ABasePraiseCharacter::CheckBodyCollisionToByChannel(FVector Target, TArray<AActor*>& Hitten, bool bOverlap, FVector Origin, TArray<AActor*> IgnoredActors, float ShapeMult, ECollisionChannel Channel)
{
	FVector TargetVector = Target - GetActorLocation();

	FVector SweepOrigin = Origin == FVector::ZeroVector ? GetActorLocation() : Origin;

	FVector SweepEnd = SweepOrigin + TargetVector.GetSafeNormal() * TargetVector.Size();

	Hitten = GetBodySweepResult(SweepOrigin, SweepEnd, bOverlap, Channel, IgnoredActors, ShapeMult);

	return Hitten.Num() > 0;
}

bool ABasePraiseCharacter::CheckBodyCollisionToByObject(FVector Target, TArray<AActor*>& Hitten, bool bOverlap, FVector Origin, TArray<AActor*> IgnoredActors, float ShapeMult, bool bWorldStatic)
{
	FVector TargetVector = Target - GetActorLocation();

	FVector SweepOrigin = Origin == FVector::ZeroVector ? GetActorLocation() : Origin;

	FVector SweepEnd = SweepOrigin + TargetVector.GetSafeNormal() * TargetVector.Size();

	Hitten = GetBodySweepResult(SweepOrigin, SweepEnd, bWorldStatic, bOverlap, IgnoredActors, ShapeMult);

	return Hitten.Num() > 0;
}

bool ABasePraiseCharacter::CheckBodyCollisionToByProfile(FVector Target, TArray<AActor*>& Hitten, FName Profile, bool bOverlap, FVector Origin, TArray<AActor*> IgnoredActors, float ShapeMult)
{
	FVector TargetVector = Target - GetActorLocation();

	FVector SweepOrigin = Origin == FVector::ZeroVector ? GetActorLocation() : Origin;

	FVector SweepEnd = SweepOrigin + TargetVector.GetSafeNormal() * TargetVector.Size();

	Hitten = GetBodySweepResult(SweepOrigin, SweepEnd, Profile, bOverlap, IgnoredActors, ShapeMult);

	return Hitten.Num() > 0;
}


TArray<AActor*> ABasePraiseCharacter::GetSweepResult(FVector Origin, float SphereRadius, bool bOverlap, ECollisionChannel Channel, TArray<AActor*> IgnoredActors)
{
	TArray<AActor*> Hitten;
	FCollisionQueryParams QueryParams;

	QueryParams.AddIgnoredActors(IgnoredActors);

	FCollisionShape ColShape = FCollisionShape::MakeSphere(SphereRadius);
	FHitResult SingleHit;
	TArray<FHitResult> MultiHits;
	switch (bOverlap) {
		case(false):
			if (GetWorld()->SweepSingleByChannel(SingleHit, Origin, Origin, FQuat::Identity, Channel, ColShape, QueryParams))
				Hitten.Add(SingleHit.Actor.Get());
			break;
		case(true):
			if (GetWorld()->SweepMultiByChannel(MultiHits, Origin, Origin, FQuat::Identity, Channel, ColShape, QueryParams))
				for (FHitResult& Hit : MultiHits)
					if(!Hitten.Contains(Hit.Actor.Get()))
						Hitten.Add(Hit.Actor.Get());
			break;
		}
	
	return Hitten;
}

TArray<AActor*> ABasePraiseCharacter::GetBodySweepResult(FVector Origin, FVector End, bool bOverlap, ECollisionChannel Channel, TArray<AActor*> IgnoredActors, float ShapeMult)
{
	TArray<AActor*> Hitten;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (IgnoredActors.Num() > 0)
		QueryParams.AddIgnoredActors(IgnoredActors);

	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(GetCapsuleComponent()->GetScaledCapsuleRadius() * ShapeMult, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * ShapeMult);

	FHitResult SingleHit;
	TArray<FHitResult> MultiHits;
	switch (bOverlap) {
		case(false):
			if (GetWorld()->SweepSingleByChannel(SingleHit, Origin, End, FQuat::Identity, Channel, CapsuleShape, QueryParams))
				Hitten.Add(SingleHit.Actor.Get());
			break;
		case(true):
			if (GetWorld()->SweepMultiByChannel(MultiHits, Origin, End, FQuat::Identity, Channel, CapsuleShape, QueryParams))
				for (FHitResult& Hit : MultiHits)
					Hitten.Add(Hit.Actor.Get());
			break;
	}

	return Hitten;
}

TArray<AActor*> ABasePraiseCharacter::GetBodySweepResult(FVector Origin, FVector End, bool bWorldStatic, bool bOverlap, TArray<AActor*> IgnoredActors, float ShapeMult)
{
	TArray<AActor*> Hitten;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FCollisionObjectQueryParams Objects;
	Objects.ObjectTypesToQuery = bWorldStatic ? Objects.AllStaticObjects : Objects.AllDynamicObjects;

	if (IgnoredActors.Num() > 0)
		QueryParams.AddIgnoredActors(IgnoredActors);

	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(GetCapsuleComponent()->GetScaledCapsuleRadius() * ShapeMult, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * ShapeMult);

	FHitResult SingleHit;
	TArray<FHitResult> MultiHits;
	switch (bOverlap) {
	case(false):
		if (GetWorld()->SweepSingleByObjectType(SingleHit, Origin, End, FQuat::Identity, Objects, CapsuleShape, QueryParams))
			Hitten.Add(SingleHit.Actor.Get());
		break;
	case(true):
		if (GetWorld()->SweepMultiByObjectType(MultiHits, Origin, End, FQuat::Identity, Objects, CapsuleShape, QueryParams))
			for (FHitResult& Hit : MultiHits)
				Hitten.Add(Hit.Actor.Get());
		break;
	}

	return Hitten;
}

TArray<AActor*> ABasePraiseCharacter::GetBodySweepResult(FVector Origin, FVector End, FName Profile, bool bOverlap, TArray<AActor*> IgnoredActors, float ShapeMult)
{
	TArray<AActor*> Hitten;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (IgnoredActors.Num() > 0)
		QueryParams.AddIgnoredActors(IgnoredActors);

	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(GetCapsuleComponent()->GetScaledCapsuleRadius() * ShapeMult, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * ShapeMult);

	FHitResult SingleHit;
	TArray<FHitResult> MultiHits;
	switch (bOverlap) {
	case(false):
		if (GetWorld()->SweepSingleByProfile(SingleHit, Origin, End, FQuat::Identity, Profile, CapsuleShape, QueryParams))
			Hitten.Add(SingleHit.Actor.Get());
		break;
	case(true):
		if (GetWorld()->SweepMultiByProfile(MultiHits, Origin, End, FQuat::Identity, Profile, CapsuleShape, QueryParams))
			for (FHitResult& Hit : MultiHits)
				Hitten.Add(Hit.Actor.Get());
		break;
	}

	return Hitten;
}

bool ABasePraiseCharacter::CanExecuteAction(ECharAction CharAction)
{

	switch (CharAction) {
	case(ECharAction::RUN):
		return !bIsAiming && !bIsBlocking && !bIsJumping && !bIsEvading && !IsInAir() && !bIsAttacking;
		case(ECharAction::JUMP):
			return !(bIsAttacking || bIsRolling || bIsEvading || bIsBeingDamaged) && HasMinActionStam(CharAction) && !bIsReloading && !bIsBlocking && !bIsAiming;
		case(ECharAction::ROLL):
		case(ECharAction::EVADE):
			return !GetCharacterMovement()->IsFalling() && !bIsAttacking && !bIsRolling && !bIsEvading && !bIsBeingDamaged && IsWeaponEquiped() && HasMinActionStam(CharAction) && !bIsReloading && !bIsBlocking && !bIsAiming;
		case(ECharAction::ATTACK):
			return !bIsEquiping && !bIsDroppingWeapon && !bIsEvading && !bIsPickingupItem && !bIsReloading && CanExecuteAttack(RequestedAttackType) && HasMinActionStam(CharAction);
		case(ECharAction::BLOCK):
			return !(GetCharacterMovement()->IsFalling() /* || bIsAttacking*/ || bIsRolling) && !bIsPickingupItem && IsWeaponEquiped() && HasMinActionStam(CharAction) && !bIsReloading;
		case(ECharAction::PARRY):
			return !(GetCharacterMovement()->IsFalling() || bIsAttacking || bIsRolling || bIsCrouched || bIsBeingDamaged) && !bIsPickingupItem && IsWeaponEquiped() && HasMinActionStam(CharAction) && !bIsReloading;
		case(ECharAction::TRIGGER_ABILITY):
			return !(GetCharacterMovement()->IsFalling() || bIsAttacking || bIsBlocking || bIsRolling) && !bIsPickingupItem && IsWeaponEquiped() && !bIsReloading;
		case(ECharAction::DROP_WEAPON):
			return HasWeapon() && !bIsAttacking && !bIsBlocking && !bIsEquiping && !bIsBeingDamaged && !bIsRolling && !bIsEvading && !bIsReloading;
		case(ECharAction::EQUIP_WEAPON):
			return HasWeapon() && !bIsAttacking && !bIsBlocking && !bIsEquiping && !bIsDroppingWeapon && !bIsPickingupItem && !bIsBeingDamaged && !bIsRolling && !bIsEvading && !bIsReloading;
		case(ECharAction::PICKUP_ITEM):
			return !bIsAttacking && !bIsBlocking && !bIsEquiping && !bIsBeingDamaged && !bIsRolling && !bIsEvading && !bIsDroppingWeapon && !bIsReloading;
	}

	return true;
}

bool ABasePraiseCharacter::CanExecuteAttack(EAttackType AttackType)
{
	if (!GetCharStats()) return false;

	UCharStatsComponent* Stats = GetCharStats();

	switch (AttackType) {
		case(EAttackType::PRIMARY_ATTACK):
		case(EAttackType::SECONDARY_ATTACK):
			return IsWeaponEquiped() && !GetCharacterMovement()->IsFalling() && !bIsBlocking && !bIsRolling;
		case(EAttackType::SPRINT_ATTACK):
			return IsWeaponEquiped() && bIsRunning && !GetCharacterMovement()->IsFalling() && !bIsBlocking && !bIsRolling;
		case(EAttackType::KICK):
			return !GetCharacterMovement()->IsFalling() && !bIsBlocking && !bIsRolling;
		case(EAttackType::EXECUTION):
			return true; 
		case(EAttackType::BREAK_GUARD):
		case(EAttackType::NONE):
		default:
			return false;
	}
	return true;
}

bool ABasePraiseCharacter::HasMinActionStam(ECharAction CharAction)
{
	
	if (!GetCharStats()) return false;

	UCharStatsComponent* Stats = GetCharStats();

	switch (CharAction) {
	case(ECharAction::RUN):
		if(this->IsA<APraisePlayerCharacter>())
			return Stats->GetStamina() > Stats->GetMaxStamina() * DEFAULT_STAM_DECREASE_PERCENT / 100.f;

		else return Stats->GetStamina() > Stats->GetMaxStamina() * DEFAULT_BOT_STAM_MIN_STAM / 100.f;
		break;
	case(ECharAction::JUMP):
		return Stats->GetStamina() > Stats->GetMaxStamina() * DEFAULT_STAM_DECREASE_PERCENT / 100.f;
	case(ECharAction::ROLL):
	case(ECharAction::EVADE):
		return Stats->GetStamina() > Stats->GetMaxStamina() * DEFAULT_STAM_DECREASE_PERCENT / 100.f;
	case(ECharAction::ATTACK):
		return HasMinAttackStam(RequestedAttackType);
	case(ECharAction::BLOCK):
		return Stats->GetStamina() > Stats->GetMaxStamina() * (DEFAULT_STAM_DECREASE_PERCENT * 0.75f) / 100.f;
	case(ECharAction::PARRY):
		return Stats->GetStamina() > Stats->GetMaxStamina() * (DEFAULT_STAM_DECREASE_PERCENT * 0.85f) / 100.f;
	default:
		return true;
	}

	return true;
}

bool ABasePraiseCharacter::HasMinAttackStam(EAttackType AttackType)
{
	if (!GetCharStats()) return false;

	UCharStatsComponent* Stats = GetCharStats();

	return Stats->GetStamina() > GetAttackStaminaDecrease(AttackType);
}

AFist* ABasePraiseCharacter::SpawnDefaultUnarmedWeapon()
{
	if (!ensure(GetWorld() != nullptr)) return nullptr;

	FVector SpawnLocation = FVector(0, 0, 0);
	FRotator SpawnRotation = FRotator(0, 0, 0);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	return GetWorld()->SpawnActor<AFist>(SpawnParameters);
}

void ABasePraiseCharacter::CacheDefaultUnarmedWeapon()
{
	if (!DefaultUnarmedWeapon)
	{
		AWeapon* DefaultFist = SpawnDefaultUnarmedWeapon();

		if (DefaultFist)
			DefaultUnarmedWeapon = DefaultFist;
	}
	
	if (!AuxSlot)
		AuxSlot = NewObject<UCharWeaponSlot>(this);

	AuxSlot->SetupSlot(EWeaponSlot::PRIMARY_WEAPON, DefaultUnarmedWeapon);
}

void ABasePraiseCharacter::ResetDefaultWeapon()
{
	if (!AuxSlot || !DefaultUnarmedWeapon)
		CacheDefaultUnarmedWeapon();

	TArray<EWeaponSlot> Keys;
	CharWeaponSlots.GetKeys(Keys);

	for (EWeaponSlot Slot : Keys)
	{
		if (CharWeaponSlots[Slot]->IsEmpty() && !CharWeaponSlots[Slot]->IsLocked())
		{
			CharWeaponSlots[Slot]->SetupSlot(Slot, DefaultUnarmedWeapon, false);
			break;
		}
	}		
}

void ABasePraiseCharacter::ManageStamina(float DeltaTime)
{
	HandleSprintStamina(DeltaTime);
	HandleBlockingStamina(DeltaTime);
	HandleStaminaRecovery(DeltaTime);
}

void ABasePraiseCharacter::HandleStaminaRecovery(float DeltaTime)
{
	if (!bCanRecoverStamina) return;

	if (!CharStats) return;

	if (CharStats->IsDead() || (bIsRunning && !GetVelocity().IsNearlyZero()) || IsBlocking()) return;

	float DeltaStam = (DEFAULT_STAM_DECREASE_PERCENT * 2.f) * CharStats->GetMaxStamina() /** CharStats->GetResistanceRatio()*/ / 100; 

	if (CharStats->GetStamina() <= CharStats->GetMaxStamina())
		UpdateStamina(DeltaStam * DeltaTime);

	if (this->IsA<ABaseBotCharacter>() && Cast<ABaseBotCharacter>(this)->GetBrain() && Cast<ABaseBotCharacter>(this)->GetBrain()->GetBotBB())
	{
		if(HasMinActionStam(ECharAction::RUN))
			Cast<ABaseBotCharacter>(this)->GetBrain()->GetBotBB()->SetValueAsBool(BB_CAN_RUN, true);
	}
		
}

void ABasePraiseCharacter::HandleSprintStamina(float DeltaTime)
{
	if (this->IsA<ABaseBotCharacter>() && Cast<ABaseBotCharacter>(this)->GetBrain() && !Cast<ABaseBotCharacter>(this)->GetBrain()->IsSprintStamEnabled()) return;
	
	if (!CharStats) return;

	if (!bIsRunning || bIsRolling) return;

	if (GetMovementComponent() && GetMovementComponent()->IsFalling()) return;

	if (GetVelocity().IsNearlyZero()) return;

	float DeltaStam = GetActionStaminaDecrease(ECharAction::RUN);

	if (DeltaStam > 0.f) return;

	UpdateStamina(DeltaStam * DeltaTime);

	if (CharStats->GetStamina() <= 0.f) 
	{
		RestoreWalkSpeed(this->IsA<APraisePlayerCharacter>());

		bIsRunning = false;
		
		if (this->IsA<ABaseBotCharacter>() && HasMinActionStam(ECharAction::RUN))
			Cast<ABaseBotCharacter>(this)->GetBrain()->GetBotBB()->SetValueAsBool(BB_CAN_RUN, false);

		return;
	}

}

void ABasePraiseCharacter::HandleBlockingStamina(float DeltaTime)
{
	if (!IsBlocking()) return;

	if (!HasMinActionStam(ECharAction::BLOCK))
	{
		EndBlocking();
		return;
	}

	float DeltaStam = (DEFAULT_STAM_DECREASE_PERCENT / 4.f) * CharStats->GetMaxStamina() / 100;

	UpdateStamina(-DeltaStam * DeltaTime);
}

float ABasePraiseCharacter::GetActionStaminaDecrease(ECharAction Action)
{
	if (!CharStats) return 0.0f;

	float DeltaStam = 0.f;

	switch (Action) {
	case(ECharAction::RUN):
		DeltaStam = (DEFAULT_STAM_DECREASE_PERCENT * CharStats->GetMaxStamina() / 100) * (1 / CharStats->GetResistanceRatio());
		break;
	case(ECharAction::ROLL):
		DeltaStam = (DEFAULT_STAM_DECREASE_PERCENT * 1.f) * CharStats->GetMaxStamina() / 100.f;
		break;
	case(ECharAction::ATTACK):
		DeltaStam = GetAttackStaminaDecrease(CurrentAttackType);
		break;
	case(ECharAction::BLOCK):
		DeltaStam = (DEFAULT_STAM_DECREASE_PERCENT * 0.75f) * CharStats->GetMaxStamina() / 100.f;
		break;
	case(ECharAction::PARRY):
		DeltaStam = (DEFAULT_STAM_DECREASE_PERCENT * 0.85f) * CharStats->GetMaxStamina() / 100.f;
		break;
	case(ECharAction::JUMP):
	case(ECharAction::EVADE):
		DeltaStam = (DEFAULT_STAM_DECREASE_PERCENT * CharStats->GetMaxStamina()) / 100.f;
		break;
	default: break;
	}

	return -DeltaStam;
}

float ABasePraiseCharacter::GetAttackStaminaDecrease(EAttackType AttackType)
{
	if (!GetCharStats()) return false;

	UCharStatsComponent* Stats = GetCharStats();

	switch (AttackType) {
	case(EAttackType::PRIMARY_ATTACK):
		return Stats->GetMaxStamina() * (DEFAULT_STAM_DECREASE_PERCENT * 1.f) / 100.f;
	case(EAttackType::SECONDARY_ATTACK):
		return Stats->GetMaxStamina() * (DEFAULT_STAM_DECREASE_PERCENT * 1.5f) / 100.f;
	case(EAttackType::SPRINT_ATTACK):
		return Stats->GetMaxStamina() * (DEFAULT_STAM_DECREASE_PERCENT * 1.75f) / 100.f;
	case(EAttackType::KICK):
		return Stats->GetMaxStamina() * (DEFAULT_STAM_DECREASE_PERCENT * 1.5f) / 100.f;
	case(EAttackType::EXECUTION):
	case(EAttackType::BREAK_GUARD):
	case(EAttackType::NONE):
	default:
		return 0.0f;
	}
	return 0.0f;
}

void ABasePraiseCharacter::EnableStamRecovery()
{
	bCanRecoverStamina = true;
}

void ABasePraiseCharacter::HandleActionStaminaDecrease(ECharAction CharAction, bool bRunOnServer)
{
	if (bRunOnServer)
	{
		if (GetLocalRole() == ENetRole::ROLE_Authority)
			HandleActionStaminaDecrease(CharAction, /*bRunOnServer:*/ false);

		else Server_HandleActionStaminaDecrease(CharAction);
	}
	else
	{
		if (!ensure(GetWorld() != nullptr)) return;

		float DeltaStam = GetActionStaminaDecrease(CharAction);

		if (DeltaStam >= 0.f) return;

		UpdateStamina(DeltaStam);

		bCanRecoverStamina = false;
		FTimerDelegate Del;
		Del.BindUFunction(this, FName("EnableStamRecovery"));

		GetWorld()->GetTimerManager().SetTimer(EnableStaminaRecoveryTimer, Del, 1.5f, false);
	}
}

void ABasePraiseCharacter::Server_HandleActionStaminaDecrease_Implementation(ECharAction CharAction)
{
	if (GetLocalRole() != ENetRole::ROLE_Authority) return;

	HandleActionStaminaDecrease(CharAction, /*bRunOnServer:*/ false);
}

void ABasePraiseCharacter::UpdateStamina(float Value, bool bRunOnServer)
{
	if (bRunOnServer) {
		if (GetLocalRole() == ENetRole::ROLE_Authority)
			UpdateStamina(Value, /*bRunOnServer:*/ false);

		else Server_UpdateStamina(Value);
	}
	else {
		if (!CharStats) return;

		CharStats->UpdateStamina(Value);
	}
}


void ABasePraiseCharacter::Server_UpdateStamina_Implementation(float Value)
{
	if (GetLocalRole() != ENetRole::ROLE_Authority) return;

	UpdateStamina(Value, /*bRunOnServer:*/ false);
}

void ABasePraiseCharacter::UpdateHealth(float Value, bool bRunOnServer)
{
	if (bRunOnServer) {
		if (GetLocalRole() == ENetRole::ROLE_Authority)
			UpdateHealth(Value, /*bRunOnServer:*/ false);

		else Server_UpdateHealth(Value);
	}
	else {
		if (!CharStats) return;

		CharStats->UpdateHealth(Value);
	}
}

void ABasePraiseCharacter::Server_UpdateHealth_Implementation(float Value)
{
	if (GetLocalRole() != ENetRole::ROLE_Authority) return;

	UpdateHealth(Value, /*bRunOnServer:*/ false);
}



