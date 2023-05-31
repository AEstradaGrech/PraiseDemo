#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "../../../Enums/CombatSystem/ECombatSFX.h"
#include "FWeaponFxDTStruct.generated.h"

USTRUCT()
struct PRAISE_API FWeaponFxDTStruct : public  FTableRowBase {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon)
		FString FxSetupName;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon)
		FString WeaponModel;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon)
		TArray<FString> PathToFX;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon)
		TArray<FString> WeaponSFX;
};