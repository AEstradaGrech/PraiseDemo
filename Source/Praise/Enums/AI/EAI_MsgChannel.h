#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "EAI_MsgChannel.generated.h"

UENUM(BlueprintType)
enum class EAI_MsgChannel : uint8 {
	GENERAL,
	FACTION,
	ZONE,
	TEAM
};
