// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Structs/CombatSystem/Weapons/FFiregunDTStruct.h"
#include "../../Structs/CombatSystem/Weapons/FWeaponDTStruct.h"
#include "../../Structs/CombatSystem/Weapons/FWeaponArtDTStruct.h"
#include "../../Enums/CombatSystem/EWeaponType.h"
#include "../../Enums/CombatSystem/EWeaponClass.h"
#include "../../Enums/Animation/EHolsterPosition.h"
#include "../../Weapons/Weapon.h"
#include "../../Structs/CombatSystem/Weapons/FWeaponAbilitiesDTStruct.h"
#include "WeaponsFactoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRAISE_API UWeaponsFactoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponsFactoryComponent();
	typedef AWeapon* (UWeaponsFactoryComponent::* pCreateWeaponFn)();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	FWeaponDTStruct* GetWeaponStruct(EWeaponClass Class = EWeaponClass::UNARMED, int32 ModelConfig = 0);
	FWeaponDTStruct* GetRandomStructWithConfig(EWeaponType Type = EWeaponType::UNARMED, EWeaponClass Class = EWeaponClass::UNARMED, FString Model = "");
	FWeaponDTStruct* GetRandomWeaponStructWithConfig(EWeaponType Type);
	FWeaponDTStruct* GetRandomWeaponStructWithConfig(EWeaponClass Class);
	FWeaponDTStruct* GetTemplateForWeaponType(EWeaponType Type);
	AWeapon* GetDefaultFistWeapon();
	

	template<class T>
	AWeapon* SpawnWeapon(EHolsterPosition HolsterPosition, EWeaponType NewWeaponType, FString WeaponModel = FString("Default"), int ConfigID = -1);
	// Called when the game starts or when spawned
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWeaponsDBComponent* WeaponsDBComp;

	typedef TMap<uint8, pCreateWeaponFn> WeaponsMap;
	WeaponsMap CreateWeaponsMap;

	template<typename T>
	void RegisterWeapon(EWeaponType Key) { CreateWeaponsMap.Add((uint8)Key, &UWeaponsFactoryComponent::CreateWeapon<T>); };

	template<typename T>
	AWeapon* CreateWeapon();

	 void RegisterWeapons();
};

template<class T>
inline AWeapon* UWeaponsFactoryComponent::SpawnWeapon(EHolsterPosition HolsterPosition, EWeaponType NewWeaponType, FString WeaponModel, int ConfigID)
{
	if (!ensure(GetWorld() != nullptr)) return nullptr;

	FVector SpawnLoc = FVector(0, 0, 0);
	FRotator SpawnRot = FRotator(0, 0, 0);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AWeapon* Weapon = GetWorld()->SpawnActor<T>(SpawnParams);

	if (!Weapon) return nullptr;

	Weapon->SetStatsConfigID(ConfigID);

	if (!Weapon->InitWeapon(WeaponModel, NewWeaponType))
	{
		Weapon->Destroy();
		return nullptr;
	}
	
	return Weapon;
}
