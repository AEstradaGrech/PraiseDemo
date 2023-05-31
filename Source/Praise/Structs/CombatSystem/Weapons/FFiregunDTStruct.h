#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "FWeaponDTStruct.h"
#include "FFiregunDTStruct.generated.h"

USTRUCT()
struct PRAISE_API FFiregunDTStruct : public FWeaponDTStruct
{
	GENERATED_BODY();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float Range;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	int MaxAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	int MaxMagAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float ReloadTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float SpreadCorrectionPercentage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float HorizontalSpreadDegs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float VerticalSpreadDegs;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	FVector AimLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	FRotator AimRotation;
};