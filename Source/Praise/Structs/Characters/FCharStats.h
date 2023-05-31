#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "../../Enums/Characters/ECharFaction.h"
#include "../../Enums/Characters/ECharClass.h"
#include "Engine/DataTable.h"
#include "FCharStats.generated.h"

USTRUCT(BlueprintType)
struct PRAISE_API FCharStats : public FTableRowBase
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString CharName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ECharClass Class;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Strenght;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Constitution;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Dextrity; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Intelligence;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Luck;

};
