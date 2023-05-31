// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Armors/Armor.h"
#include "../../Enums/CombatSystem/EArmorType.h"
#include "ArmorsFactoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRAISE_API UArmorsFactoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UArmorsFactoryComponent();

	template<class T>
	AArmor* SpawnArmor(EArmorType Type, FString Model = FString("Default"), int ConfigID = -1);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UArmorsDBComponent* ArmorsDB;
	
};

template<class T>
inline AArmor* UArmorsFactoryComponent::SpawnArmor(EArmorType Type, FString Model, int ConfigID)
{
	if (!ensure(GetWorld() != nullptr)) return nullptr;

	FVector SpawnLoc = FVector(0, 0, 0);
	FRotator SpawnRot = FRotator(0, 0, 0);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AArmor* Armor = GetWorld()->SpawnActor<T>(SpawnParams);

	if (!Armor) return nullptr;

	if (!Armor->DidInit())
	{
		Armor->SetStatsConfigID(ConfigID);

		if (!Armor->InitArmor(Type, Model))
		{
			Armor->Destroy();
			return nullptr;
		}
	}

	return Armor;
}
