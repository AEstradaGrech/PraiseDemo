// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponsFactoryComponent.h"
// Sets default values for this component's properties
UWeaponsFactoryComponent::UWeaponsFactoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	WeaponsDBComp = CreateDefaultSubobject<UWeaponsDBComponent>(TEXT("Weapons DB"));
}


// Called when the game starts
void UWeaponsFactoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	RegisterWeapons();
}


FWeaponDTStruct* UWeaponsFactoryComponent::GetWeaponStruct(EWeaponClass Class, int32 ModelConfig)
{
	if(Class != EWeaponClass::UNARMED && ModelConfig > 0)
		return WeaponsDBComp->GetWeaponsConfigDTByClassAndId(Class, ModelConfig);


	if (Class == EWeaponClass::UNARMED)
	{
		int maxIdx = FUtilities::GetEnumCount(TEXT("EWeaponType"));
		int rndIdx = FMath::RoundToInt(FMath::RandRange(0, maxIdx /* - 1 ??*/));

		TArray<FWeaponDTStruct*> Configs = WeaponsDBComp->GetWeaponsConfigDTByClass((EWeaponClass)rndIdx);

		rndIdx = ModelConfig <= 0 ? FMath::RoundToInt(FMath::RandRange(0, Configs.Num() - 1)) : ModelConfig;

		return Configs[rndIdx];
	}
	else
	{
		TArray<FWeaponDTStruct*> Configs = WeaponsDBComp->GetWeaponsConfigDTByClass(Class);

		int rndIdx = ModelConfig <= 0 ? FMath::RoundToInt(FMath::RandRange(0, Configs.Num() - 1)) : ModelConfig;

		return Configs[rndIdx];
	}
	
	return nullptr;
}

FWeaponDTStruct* UWeaponsFactoryComponent::GetRandomStructWithConfig(EWeaponType Type, EWeaponClass Class, FString Model)
{
	if (Model == "")
	{
		if (Class == EWeaponClass::UNARMED)
		{
			if (Type == EWeaponType::UNARMED)
			{
				int32 MaxIdx = FUtilities::GetEnumCount(TEXT("EWeaponType"));

				uint8 rndIdx = FMath::RoundToInt(FMath::RandRange(1, MaxIdx));

				EWeaponType type = (EWeaponType)rndIdx;

				return GetRandomWeaponStructWithConfig(type);
			}

			else return GetRandomWeaponStructWithConfig(Type);
		}

		else return GetRandomWeaponStructWithConfig(Class);
	}
	else
	{
		if (Class == EWeaponClass::UNARMED) return nullptr;

		TArray<FWeaponDTStruct*> WeaponConfig = WeaponsDBComp->GetWeaponsConfigDTByModel(Class, Model);

		if (WeaponConfig .Num() <= 0) return nullptr;

		int32 rndConfigId = FMath::RoundToInt(FMath::RandRange(0, WeaponConfig.Num() - 1));
		
		return WeaponConfig[rndConfigId];
	}
	
	return nullptr;
}

FWeaponDTStruct* UWeaponsFactoryComponent::GetRandomWeaponStructWithConfig(EWeaponType Type)
{
	if (Type == EWeaponType::UNARMED) return nullptr;

	TArray<FWeaponDTStruct*> Weapons = WeaponsDBComp->GetWeaponsDTByType(Type);

	if (Weapons.Num() <= 0) return nullptr;

	int rndIdx = FMath::RoundToInt(FMath::RandRange(0, Weapons.Num() - 1));

	FWeaponDTStruct* Weapon = Weapons[rndIdx];

	TArray<FWeaponDTStruct*> Configs = WeaponsDBComp->GetWeaponsConfigDTByModel((EWeaponClass)Weapon->WeaponClass, Weapon->WeaponModel);

	if (Configs.Num() <= 0) return  Weapons[rndIdx];

	rndIdx = FMath::RoundToInt(FMath::RandRange(0, Configs.Num() - 1));

	return Configs[rndIdx];
}

FWeaponDTStruct* UWeaponsFactoryComponent::GetRandomWeaponStructWithConfig(EWeaponClass Class)
{
	if (Class == EWeaponClass::UNARMED) return nullptr;

	TArray<FWeaponDTStruct*> Weapons = WeaponsDBComp->GetWeaponsConfigDTByClass(Class);

	if (Weapons.Num() <= 0) return nullptr;

	int rndIdx = FMath::RoundToInt(FMath::RandRange(0, Weapons.Num() - 1));

	return Weapons[rndIdx];
}

FWeaponDTStruct* UWeaponsFactoryComponent::GetTemplateForWeaponType(EWeaponType Type)
{
	return WeaponsDBComp ? WeaponsDBComp->GetTemplateForWeaponType(Type) : nullptr;
}

AWeapon* UWeaponsFactoryComponent::GetDefaultFistWeapon()
{
	FWeaponDTStruct* UnarmedData = WeaponsDBComp->GetDefaultFistWeapon();

	if (!UnarmedData) return nullptr;

	return nullptr;
}


void UWeaponsFactoryComponent::RegisterWeapons()
{
	RegisterWeapon<AWeapon>(EWeaponType::UNARMED);
}


template<typename T>
AWeapon* UWeaponsFactoryComponent::CreateWeapon()
{
	if (!ensure(GetWorld() != nullptr)) return nullptr;

	return GetWorld()->SpawnActor<T>();
}

