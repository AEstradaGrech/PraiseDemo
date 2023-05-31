#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "../../../Enums/Animation/EHolsterPosition.h"
#include "FWeaponArtDTStruct.generated.h"

USTRUCT()
struct PRAISE_API FWeaponArtDTStruct : public FTableRowBase
{
	GENERATED_BODY();
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString Model;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		int32 WeaponClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString SkinName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString SkinDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString PathToMesh;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon)
		FVector MeshScale;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon)
		FVector DroppedWeaponBoxExtents;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon)
		FVector WeaponBoxExtents;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon)
		FVector WeaponBoxLocation;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon)
		FVector DamageBoxLocation;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon)
		FVector DamageBoxExtents;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon)
		FVector BreakGuardExtents;
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = Weapon)
		FVector BreakGuardLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FVector LeftHandLocation; // 08/04/23 --> no se esta usando. en principio es para TwoHandedWeapons.IK
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FVector EquipLocationR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FRotator EquipRotationR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FVector EquipLocationL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FRotator EquipRotationL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FVector BlockLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FRotator BlockRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float InteractionColRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		TArray<FString> PathToMaterials;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		TMap<EHolsterPosition, FVector> HolsterLocations;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		TMap<EHolsterPosition, FRotator> HolsterRotations;
};
