#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EWaypointType.generated.h"

UENUM(BlueprintType)
enum class EWaypointType : uint8
{
	PATH_POINT = 0			UMETA(DisplayName = "Path Point"), 
	PATH_END = 1			UMETA(DisplayName = "Path End"),   
	PATH_NODE = 2			UMETA(DisplayName = "Path Node"),	
	ZONE_ENTRY = 3			UMETA(DisplayName = "Zone Entry")	
};
