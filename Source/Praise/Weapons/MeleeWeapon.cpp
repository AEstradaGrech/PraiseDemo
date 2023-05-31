// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeapon.h"
#include "../Characters/BasePraiseCharacter.h"
AMeleeWeapon::AMeleeWeapon() :Super()
{
	WeaponType = EWeaponType::POINTED;

	BreakGuardCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Break Guard Collider"));
	BreakGuardCollider->AttachTo(RootComponent);
	BreakGuardCollider->SetBoxExtent(FVector(10, 10, 10));
	BreakGuardCollider->SetVisibility(true);
	BreakGuardCollider->SetCollisionProfileName(FName("NoCollision"));
	BreakGuardCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}


bool AMeleeWeapon::TrySetupWeaponColliders(FWeaponArtDTStruct* ArtData)
{
	if (!Super::TrySetupWeaponColliders(ArtData)) return false;
	 
	if (!CurrentSkin) return false;

	if (!BreakGuardCollider) return false;

	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
	
	BreakGuardCollider->AttachToComponent(WeaponMesh, AttachmentRules);
	BreakGuardCollider->SetRelativeLocation(CurrentSkin->BreakGuardLocation);
	BreakGuardCollider->SetBoxExtent(CurrentSkin->BreakGuardExtents);
	BreakGuardCollider->OnComponentBeginOverlap.AddDynamic(this, &AMeleeWeapon::OnAttackBoxBeginOverlap);

	HandleEnableCollider(BreakGuardCollider, false);

	return true;
}

void AMeleeWeapon::Attack(AActor* Target)
{
	if (!Cast<ABasePraiseCharacter>(GetOwner())) return;

	if (CurrentAttackHits.Contains(Target)) return;

	ABasePraiseCharacter* WeaponOwner = Cast<ABasePraiseCharacter>(GetOwner());

	if (!WeaponOwner->IsAttacking()) return;

	if (Target->Implements<UDamageable>())
	{
		IDamageable* Damageable = Cast<IDamageable>(Target);

		if (Damageable->IsDead()) return;

		Damageable->GetWeaponDamage(-GetAttackDamage(), WeaponOwner->GetCurrentAttackType(), EWeaponSlot::PRIMARY_WEAPON, WeaponOwner, this);

		CurrentAttackHits.Add(Target);

		ApplyWeaponStateReduction();
	}

}

