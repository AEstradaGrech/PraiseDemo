
#pragma once
#include "CoreMinimal.h"
#include "../../Components/Actor/AI/BotBrainComponent.h"

#include "Async/AsyncWork.h"

class PRAISE_API ProcessKnownTargetsAsyncTask : public FNonAbandonableTask
{
public:
	ProcessKnownTargetsAsyncTask();
	ProcessKnownTargetsAsyncTask(class ABaseBotCharacter* Bot);
	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(ProcessKnownTargetsAsyncTask, STATGROUP_ThreadPoolAsyncTasks); }

	void DoWork();
private:
	
	class ABaseBotCharacter* Bot;
	class AGameStateBase* GameState;
	UBotBrainComponent* BotBrain;
	void ProcessTargets();
	void ProcessItem(class ITargeteable* Item);
	void ProcessCharacter(class ABasePraiseCharacter* Character);
};