#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "../../../Enums/CombatSystem/EArmedPoses.h"
#include "../../../Enums/Animation/EHolsterPosition.h"
#include "FEquipAnimationsDataRow.generated.h"

USTRUCT()
struct FEquipAnimationsDataRow : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadonly)
			FString AnimationName;
		UPROPERTY(EditAnywhere, BlueprintReadonly)
			EHolsterPosition HolsterPosition;
		UPROPERTY(EditAnywhere, BlueprintReadonly)
			EArmedPoses ArmedPose;
		UPROPERTY(EditAnywhere, BlueprintReadonly)
			bool IsMontage;
		UPROPERTY(EditAnywhere, BlueprintReadonly)
			float TimePartition;
	
};