#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "../../../Enums/CombatSystem/EArmedPoses.h"
#include "../../../Enums/CombatSystem/EWeaponType.h"
#include "FAttackAnimationsDataRow.generated.h"

USTRUCT()
struct FAttackAnimationsDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadonly)
		FString AnimName;
	UPROPERTY(EditAnywhere, BlueprintReadonly)
		EWeaponType WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadonly)
		EArmedPoses CombatPose;
	UPROPERTY(EditAnywhere, BlueprintReadonly)
		bool bIsMontage;
	UPROPERTY(EditAnywhere, BlueprintReadonly)
		FString PathToResource;
};