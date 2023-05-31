#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"

#include "Engine/DataTable.h"
#include "FCharStats.h"
#include "FPlayerStats.generated.h"

USTRUCT(BlueprintType)
struct PRAISE_API FPlayerStats : public FCharStats 
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Wisdom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Charisma;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float CurrentXP;
};