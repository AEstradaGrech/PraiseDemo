#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "FWeaponAbilitiesDTStruct.generated.h"

USTRUCT()
struct PRAISE_API FWeaponAbilitiesDTStruct : public FTableRowBase
{
	GENERATED_BODY();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString AbilityName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		uint8 WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		uint8 WeaponClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString WeaponModel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString AbilityDesc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		int32 RequiredLevel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		bool IsPermanent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		bool IsConfigBased;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		bool AppliesOnConfig;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		int32 MaxNumber;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float DurationSeconds;
};