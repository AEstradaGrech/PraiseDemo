#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"

#include "FArmorArtDTStruct.generated.h"

USTRUCT()
struct PRAISE_API FArmorArtDTStruct : public FTableRowBase
{
	GENERATED_BODY();
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString Model;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		int32 ArmorType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString SkinName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString SkinDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		TArray<FString> PathToMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FString DroppedArmorMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		float InteractionColRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FVector DroppedArmorBoxExtents;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
		FVector DroppedMeshScale;
};
