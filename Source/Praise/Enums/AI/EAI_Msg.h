#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EAI_Msg.generated.h"

UENUM(BlueprintType)
enum class EAI_Msg : uint8 {
	STAY_IN_PLACE,
	GO_TO,
	FIND_TARGET,
	INSPECT_LOCATION_AT,
	TARGET_SEEN_AT,
	FIND_HELP,
	BACKUP_REQ,
	FOLLOW_TARGET,
	RETREAT,
	GET_LOCATION,
	GET_ACTOR_REF
};
