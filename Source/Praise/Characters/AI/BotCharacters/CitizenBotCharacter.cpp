// Fill out your copyright notice in the Description page of Project Settings.



#include "CitizenBotCharacter.h"
#include "../../../Components/Actor/AI/BotBrainComponent.h"
#include "../../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../../../Components/Actor/AI/T200BrainComponent.h"
#include "../../../Components/Actor/CharStats/BotStatsComponent.h"
#include "../../../Interfaces/CombatSystem/Targeteable.h"
#include "../../../Networking/PraiseGameState.h"
#include "../../../Components/GameMode/MachineStatesFactoryComponent.h"
#include "../../../Components/Actor/AnimInstances/PraiseAnimInstance.h"
#include "../../../Weapons/Fist.h"
#include "../../../Weapons/MeleeWeapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../../Weapons/Firearm.h"

ACitizenBotCharacter::ACitizenBotCharacter() : Super()
{
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Char Audio Component"));
	AudioComp->AttachTo(RootComponent);
	FootstepsAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Footsteps Audio Component"));
	FootstepsAudioComp->AttachTo(RootComponent);

	BotClass = EBotClass::CITIZEN;
	BotBrainType = EBrainType::DEFAULT;

	static ConstructorHelpers::FObjectFinder<USoundCue> MiscQ(TEXT("SoundCue'/Game/Core/Audio/Misc/MiscSoundCue.MiscSoundCue'"));

	if (MiscQ.Succeeded())
		MiscSoundCue = MiscQ.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> CharQ(TEXT("SoundCue'/Game/Core/Audio/Combat/CharFX.CharFX'"));

	if (CharQ.Succeeded())
		CharSoundCue = CharQ.Object;

}



bool ACitizenBotCharacter::TryInitBrain()
{
	if (!Super::TryInitBrain()) return false;

	if (!GetBotBrain<UBotBrainComponent>()->IsA<UStateMachineBrainComponent>()) return false;

	GetBotBrain<UStateMachineBrainComponent>()->OnStateUpdate.AddUFunction(this, FName("OnAIStateUpdate"));

	return GetBotBrain<UStateMachineBrainComponent>()->DidInit();
}

void ACitizenBotCharacter::TrySetHandColliders()
{
	HandCollider_L = InitHandCollider(FString("Hand Collider L"), FName("Hand_L"));
	HandCollider_L->OnComponentBeginOverlap.AddDynamic(this, &ACitizenBotCharacter::OnBotFistBeginOverlap);

	HandCollider_R = InitHandCollider(FString("Hand Collider R"), FName("Hand_R"));
	HandCollider_R->OnComponentBeginOverlap.AddDynamic(this, &ACitizenBotCharacter::OnBotFistBeginOverlap);

	KickCollider_R = InitHandCollider(FString("Kick Collider R"), FName("Foot_R"));
	KickCollider_R->OnComponentBeginOverlap.AddDynamic(this, &ACitizenBotCharacter::OnBotKickBeginOverlap);

	KickCollider_L = InitHandCollider(FString("Kick Collider L"), FName("Foot_L"));
	KickCollider_L->OnComponentBeginOverlap.AddDynamic(this, &ACitizenBotCharacter::OnBotKickBeginOverlap);
}

void ACitizenBotCharacter::OnBotFistBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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


void ACitizenBotCharacter::SetupCharWeaponSlots()
{
	AddDefaultUnarmedWeapon();
}

void ACitizenBotCharacter::OnHandColliderHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& HitResult)
{

}

void ACitizenBotCharacter::OnBotKickBeginOverlap(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this) return;

	if (!IsWeaponEquiped() || CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->IsEmpty()) return;

	if (OtherActor->Implements<UDamageable>())
	{
		if (OtherActor->Implements<UFactioneable>() && !IsEnemyTarget(Cast<IFactioneable>(OtherActor))) return;

		Cast<IDamageable>(OtherActor)->GetDamage(-10.f, this, EAttackType::KICK);
	}

	PlayCombatSFX(ECombatSFX::WEAPON_HIT, FUtilities::GetCombatSFXParamName(ECombatSFX::WEAPON_HIT), DEFAULT_UNARMED_HIT_SFX);
}


void ACitizenBotCharacter::StartPrimaryAttack()
{
	if (!IsWeaponEquiped()) return;

	int rnd = FMath::RoundToInt(FMath::RandRange(0, 100));

	if (rnd > 50 && rnd <= 80)
	{
		StartSecondaryAttack();
		return;
	}
	if (rnd > 80)
	{
		DropKick();
		return;
	}

	GetController()->StopMovement();
	HandleAnimatorAttackRequest(CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]); 
	HandleComboStuff(CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon());
}
void ACitizenBotCharacter::EndPrimaryAttack()
{
	Super::EndPrimaryAttack();
}

void ACitizenBotCharacter::StartSecondaryAttack()
{
	if (CurrentPose == EArmedPoses::RIFLE) 
	{
		bIsAiming = true;					
	}
	else {
		if (!IsWeaponEquiped() || bIsEquiping) return;

		UCharWeaponSlot* WeaponSlot = CurrentWeapons.Num() == 1 ? GetCurrentMainWeapon() : GetCurrentSecondaryWeapon();

		if (!WeaponSlot) return;

		if (WeaponSlot->GetSlotWeapon()->GetIsBroken()) return;

		RequestedAttackType = EAttackType::SECONDARY_ATTACK;

		if (!CanExecuteAction(ECharAction::ATTACK)) return;

		CurrentAttackWeapon = WeaponSlot->GetSlotWeapon();
		CurrentAttackType = EAttackType::SECONDARY_ATTACK;
		bIsAttacking = true;
		bCanAttack = false;
		HandleActionStaminaDecrease(ECharAction::ATTACK);
	}
}

void ACitizenBotCharacter::EndSecondaryAttack()
{
	Super::EndSecondaryAttack();
}

void ACitizenBotCharacter::StartBlocking()
{
	Super::StartBlocking();

	if (!HasMinActionStam(ECharAction::BLOCK)) return;
	if (!IsWeaponEquiped()) return;

	if (!bIsBlocking)
		bIsBlocking = true;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	if (CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->HoldsA<AFirearm>() &&
		CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->IsEquiped()) {
		CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->SetActorRelativeRotation(FRotator(76.2, 172.8, 187.2));
		CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->SetActorRelativeLocation(FVector(4, 3, 8));
	}
}

void ACitizenBotCharacter::EndBlocking()
{
	Super::EndBlocking();

	bIsBlocking = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	if (IsWeaponEquiped() && CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->HoldsA<AFirearm>() && CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->IsEquiped())
		AttachWeaponToSocket(CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon(), 
			FName("Hand_R_Socket"), 
			CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->GetEquipLocation(), 
			CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->GetEquipRotation());
}

void ACitizenBotCharacter::StartParry()
{
	if (!IsWeaponEquiped() || bIsEquiping) return;

	UCharWeaponSlot* WeaponSlot = CurrentWeapons.Num() == 1 ? GetCurrentMainWeapon() : GetCurrentSecondaryWeapon();

	if (!WeaponSlot) return;

	if (WeaponSlot->GetSlotWeapon()->GetIsBroken()) return;

	RequestedAttackType = EAttackType::PARRY;

	if (!CanExecuteAction(ECharAction::PARRY)) return;
	
	bIsParring = true;
	CurrentAttackType = EAttackType::PARRY;

	HandleActionStaminaDecrease(ECharAction::PARRY);
}

void ACitizenBotCharacter::EndParry()
{
}

void ACitizenBotCharacter::TryLockTarget()
{
	Super::TryLockTarget();
}

void ACitizenBotCharacter::EquipWeapons(EWeaponSlot Slot, bool bTryDualEquip)
{
	if (bIsEquiping) return;

	EWeaponSlot SecondSlot = Slot != EWeaponSlot::INVALID ? Slot == EWeaponSlot::PRIMARY_WEAPON ? EWeaponSlot::SECONDARY_WEAPON : EWeaponSlot::PRIMARY_WEAPON : EWeaponSlot::SECONDARY_WEAPON;

	if (Slot == EWeaponSlot::INVALID)
		Slot = EWeaponSlot::PRIMARY_WEAPON;

	bool bIsDualWeapon = false; 
	bIsDualEquipAnim = false;
	AWeapon* PrimaryWeapon = nullptr;
	AWeapon* SecondaryWeapon = nullptr;

	if (CharWeaponSlots.Num() <= 0) return;

	if (IsWeaponEquiped())
	{
		if (!CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->IsEmpty() && CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->IsEquiped())
		{
			if (CurrentWeapons.Contains(EWeaponSlot::PRIMARY_WEAPON)) 
			{
				if (CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON]->GetSlotWeapon()->IsTwoHand()) 
				{ 
					EquipSlotWeapon(CharWeaponSlots[EWeaponSlot::PRIMARY_WEAPON], /*bIsEquip*/ false, EWeaponSlot::PRIMARY_WEAPON);
				}
			}
			else
			{
				if (CurrentWeapons[EWeaponSlot::SECONDARY_WEAPON]->GetSlotWeapon()->IsTwoHand()) 
				{ 
					EquipSlotWeapon(CurrentWeapons[EWeaponSlot::PRIMARY_WEAPON], /*bIsEquip*/ false, EWeaponSlot::PRIMARY_WEAPON);
				}
			}
		}
	}
	else
	{
		if (!CharWeaponSlots[Slot]->IsEmpty()) 
		{
			PrimaryWeapon = CharWeaponSlots[Slot]->GetSlotWeapon();

			if (PrimaryWeapon->IsTwoHand()) {
				EquipSlotWeapon(CharWeaponSlots[Slot], true, EWeaponSlot::PRIMARY_WEAPON);
				return;
			}
		}
	}
}

void ACitizenBotCharacter::SetCharacterDead(AActor* Killer)
{
	if(HandCollider_R)
		HandCollider_R->OnComponentBeginOverlap.RemoveDynamic(this, &ACitizenBotCharacter::OnBotFistBeginOverlap);
	if(HandCollider_L)
		HandCollider_L->OnComponentBeginOverlap.RemoveDynamic(this, &ACitizenBotCharacter::OnBotFistBeginOverlap);
	if(KickCollider_R)
		KickCollider_R->OnComponentBeginOverlap.RemoveDynamic(this, &ACitizenBotCharacter::OnBotKickBeginOverlap);
	if (KickCollider_L)
		KickCollider_L->OnComponentBeginOverlap.RemoveDynamic(this, &ACitizenBotCharacter::OnBotKickBeginOverlap);

	EnableHandCollider(false, true);
	EnableHandCollider(false, false);
	EnableKickCollider(false, true);
	EnableKickCollider(false, false);

	Super::SetCharacterDead(Killer); 
}

void ACitizenBotCharacter::EnableHandCollider(bool bEnable, bool bIsRightHand)
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

void ACitizenBotCharacter::EnableKickCollider(bool bEnable, bool bIsRightLeg)
{	
	if (!IsWeaponEquiped()) return; 

	FName ColProfileName = bEnable ? FName("Weapon") : FName("NoCollision");
	ECollisionEnabled::Type ColType = bEnable ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;

	switch (bIsRightLeg)
	{
	case(true):
		if (!KickCollider_R) return;
		KickCollider_R->SetCollisionProfileName(ColProfileName);
		KickCollider_R->SetCollisionEnabled(ColType);
		KickCollider_R->SetActive(bEnable);
		break;
	case(false):
		if (!KickCollider_L) return;
		KickCollider_L->SetCollisionProfileName(ColProfileName);
		KickCollider_L->SetCollisionEnabled(ColType);
		KickCollider_L->SetActive(bEnable);
		break;
	}

	if (bEnable && bIsAttacking)
		PlayCharSFX(ECharSFX::CHAR_ATTACK);
}
