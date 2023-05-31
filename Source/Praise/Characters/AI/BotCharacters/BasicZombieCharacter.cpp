// Fill out your copyright notice in the Description page of Project Settings.


#include "BasicZombieCharacter.h"
#include "../../../Enums/AI/EBotClass.h"
#include "../../../Weapons/Fist.h"
#include "../../../Components/Actor/AI/BotBrainComponent.h"
#include "../../../Components/Actor/AI/BTBrainComponent.h"
#include "../../../Components/Actor/WeaponsFactoryComponent.h"

ABasicZombieCharacter::ABasicZombieCharacter() : Super()
{
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Char Audio Component"));
	AudioComp->AttachTo(RootComponent);
	FootstepsAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Footsteps Audio Component"));
	FootstepsAudioComp->AttachTo(RootComponent);
	CharWeaponsFactory = CreateDefaultSubobject<UWeaponsFactoryComponent>(TEXT("WeaponsFactory"));
	BotClass = EBotClass::UNDEAD;
	BotBrainType = EBrainType::BT_BRAIN;

	static ConstructorHelpers::FObjectFinder<USoundCue> MiscQ(TEXT("SoundCue'/Game/Core/Audio/Misc/MiscSoundCue.MiscSoundCue'"));

	if (MiscQ.Succeeded())
		MiscSoundCue = MiscQ.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> CharQ(TEXT("SoundCue'/Game/Core/Audio/Combat/CharFX.CharFX'"));

	if (CharQ.Succeeded())
		CharSoundCue = CharQ.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> CombatQ(TEXT("SoundCue'/Game/Core/Audio/Combat/CombatFX.CombatFX'"));

	if (CombatQ.Succeeded())
		CombatSoundCue = CombatQ.Object;
}

void ABasicZombieCharacter::SetupCharWeaponSlots()
{
	AddDefaultUnarmedWeapon();
}

void ABasicZombieCharacter::TrySetHandColliders()
{
	HandCollider_L = InitHandCollider(FString("Hand Collider L"), FName("Hand_L"));
	HandCollider_L->OnComponentBeginOverlap.AddDynamic(this, &ABasicZombieCharacter::OnBotFistBeginOverlap);

	HandCollider_R = InitHandCollider(FString("Hand Collider R"), FName("Hand_R"));
	HandCollider_R->OnComponentBeginOverlap.AddDynamic(this, &ABasicZombieCharacter::OnBotFistBeginOverlap);
}

void ABasicZombieCharacter::OnBotFistBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this) return;

	if (!IsWeaponEquiped() || CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->IsEmpty()) return;

	if (OtherActor->Implements<UDamageable>() && CurrentWeapons.Contains(EWeaponSlot::PRIMARY_WEAPON) && !CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->IsEmpty())
	{
		if (OtherActor->Implements<UFactioneable>() && !IsEnemyTarget(Cast<IFactioneable>(OtherActor))) return;

		AFist* CharFist = Cast<AFist>(CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon());

		if (!CharFist) return;

		Cast<IDamageable>(OtherActor)->GetWeaponDamage(-CharFist->GetWeaponDamage(), EAttackType::PRIMARY_ATTACK, EWeaponSlot::PRIMARY_WEAPON, this, CharFist);
	}

	PlayCombatSFX(ECombatSFX::WEAPON_HIT, FUtilities::GetCombatSFXParamName(ECombatSFX::WEAPON_HIT), DEFAULT_UNARMED_HIT_SFX);
}

AFist* ABasicZombieCharacter::SpawnDefaultUnarmedWeapon()
{
	if (CharWeaponsFactory)
	{
		AFist* ZombieFist = Cast<AFist>(CharWeaponsFactory->SpawnWeapon<AFist>(EHolsterPosition::UNARMED, EWeaponType::UNARMED, FString("Default"), DEFAULT_ZOMBIE_FIST_CONFIG));
		
		return ZombieFist ? ZombieFist : Super::SpawnDefaultUnarmedWeapon();
	}

	else return Super::SpawnDefaultUnarmedWeapon();
}

void ABasicZombieCharacter::BeginPlay()
{
	Super::BeginPlay();

	FLogger::LogTrace(__FUNCTION__ + FString(" :: ZOMBIE ACCELERATION --> ") + FString::SanitizeFloat(GetCharacterMovement()->MaxAcceleration));
}

void ABasicZombieCharacter::OnAnimStateUpdate(ECharAnimState State, bool bIsEnabled)
{
	Super::OnAnimStateUpdate(State, bIsEnabled);

	switch (State)
	{
		case(ECharAnimState::ROAR):
			bIsRoaring = false;
			bCanMove = true;
			bDamageAnimDisabled = false;
			
			if(GetBrain() && GetBrain()->GetBotBB())
				GetBrain()->GetBotBB()->SetValueAsBool(BB_IS_ROARING, false);
			break;
		case(ECharAnimState::ATTACKING):
			if (GetBrain() && GetBrain()->GetBotBB())
			{
				GetBrain()->GetBotBB()->SetValueAsBool(BB_IS_ATTACKING, false);
				GetBrain()->GetBotBB()->SetValueAsBool(BB_CAN_MOVE, true);
				GetBrain()->GetBotBB()->SetValueAsBool(BB_CAN_ATTACK, true);
			}

			break;
	}
}

void ABasicZombieCharacter::TriggerRoar()
{
	bCanMove = false;
	bDamageAnimDisabled = true;
	bIsRoaring = true;
	GetBrain()->GetBotBB()->SetValueAsBool(BB_IS_ROARING, true);
	//PlaySFX();
}

void ABasicZombieCharacter::StartPrimaryAttack()
{
	if (!IsWeaponEquiped()) return;

	EquippingHand = FMath::RandRange(0, 1);
	GetController()->StopMovement();
	
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
			GetBrain()->GetBotBB()->SetValueAsBool(BB_IS_ATTACKING, true);
			GetBrain()->GetBotBB()->SetValueAsBool(BB_CAN_MOVE, false);
			GetBrain()->GetBotBB()->SetValueAsBool(BB_CAN_ATTACK, false);
			HandleActionStaminaDecrease(ECharAction::ATTACK);
		}
	}
	
}

void ABasicZombieCharacter::StartSecondaryAttack()
{
	if (!IsWeaponEquiped() || bIsEquiping) return;

	UCharWeaponSlot* WeaponSlot = CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON];

	RequestedAttackType = EAttackType::SECONDARY_ATTACK;

	if (!CanExecuteAction(ECharAction::ATTACK)) return;

	CurrentAttackWeapon = WeaponSlot->GetSlotWeapon();
	CurrentAttackType = EAttackType::SECONDARY_ATTACK;
	bIsAttacking = true;
	GetBrain()->GetBotBB()->SetValueAsBool(BB_IS_ATTACKING, true);
	GetBrain()->GetBotBB()->SetValueAsBool(BB_CAN_MOVE, false);
	GetBrain()->GetBotBB()->SetValueAsBool(BB_CAN_ATTACK, false);
	//bCanAttack = false;
	HandleActionStaminaDecrease(ECharAction::ATTACK);
}

void ABasicZombieCharacter::EnableHandCollider(bool bEnable, bool bIsRightHand)
{
	if (!IsWeaponEquiped()) return;

	FName ColProfileName = bEnable ? FName("Weapon") : FName("NoCollision");
	ECollisionEnabled::Type ColType = bEnable ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;

	switch (bIsRightHand)
	{
	case(true):
		if (!HandCollider_R) return;
		HandCollider_R->SetCollisionProfileName(ColProfileName);
		HandCollider_R->SetCollisionEnabled(ColType);
		HandCollider_R->SetActive(bEnable);
		break;
	case(false):
		if (!HandCollider_L) return;
		HandCollider_L->SetCollisionProfileName(ColProfileName);
		HandCollider_L->SetCollisionEnabled(ColType);
		HandCollider_L->SetActive(bEnable);
		break;
	}

	if (bEnable && bIsAttacking)
		PlayCharSFX(ECharSFX::CHAR_ATTACK);

}

void ABasicZombieCharacter::EquipWeapons(EWeaponSlot Slot, bool bTryDualEquip)
{
	AWeapon* PrimaryWeapon = nullptr;
	AWeapon* SecondaryWeapon = nullptr;

	if (CharWeaponSlots.Num() <= 0) return;

	if (!CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]->HoldsA<AFist>()) return;

	if (!IsWeaponEquiped())
	{
		CurrentWeapons.Add(EWeaponSlot::PRIMARY_WEAPON, CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]);
		CurrentWeapons.Add(EWeaponSlot::SECONDARY_WEAPON, CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON]);
	}

	else CurrentWeapons.Empty();
}

void ABasicZombieCharacter::SetCharacterDead(AActor* Killer)
{
	if (GetBrain())
		GetBrain()->SetBrainEnabled(false);

	Super::SetCharacterDead(Killer);
}
