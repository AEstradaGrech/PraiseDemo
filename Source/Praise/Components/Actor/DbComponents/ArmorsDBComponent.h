// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../../Structs/CombatSystem/Armors/FArmorsDTStruct.h"
#include "../../../Structs/CombatSystem/Armors/FArmorArtDTStruct.h"
#include "../../../Enums/CombatSystem/EArmorType.h"
#include "ArmorsDBComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRAISE_API UArmorsDBComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UArmorsDBComponent();

	FArmorsDTStruct* GetArmorByTypeAndModel(EArmorType Type, FString Model);
	TArray<FArmorsDTStruct*> GetArmorConfigsByTypeAndModel(EArmorType Type, FString Model);
	TArray<FArmorsDTStruct*> GetArmorModsByTypeAndModel(EArmorType Type, FString Model);

	FArmorsDTStruct* GetArmorConfigByID(int ConfigID);
	FArmorsDTStruct* GetArmorModByID(int ModID);

	FArmorArtDTStruct* GetArmorArtByID(int ArtDataID);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	class UDataTable* ArmorsDT;
	class UDataTable* ArmorConfigsDT;
	class UDataTable* ArmorModsDT;
	class UDataTable* ArmorArtDT;
		
};
