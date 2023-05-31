#pragma once
#include "CoreMinimal.h"
#include "../../Components/Actor/AI/BotBrainComponent.h"
#include "../../Structs/AI/FEnvQueryConfig.h"
#include "Async/AsyncWork.h"
/**
 * 
 */
class PRAISE_API TryEnvQueryAsyncTask : public FNonAbandonableTask
{


public:
	TryEnvQueryAsyncTask();
	~TryEnvQueryAsyncTask();

	TryEnvQueryAsyncTask(class UBotBrainComponent* Brain, class UBotEnvQuerier* EnvQuerierComp, FEnvQueryConfig Config);
	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(TryEnvQueryAsyncTask, STATGROUP_ThreadPoolAsyncTasks); }

	void DoWork();

	class UBotBrainComponent* BotBrain;
	class UBotEnvQuerier* EnvQuerier;
	FEnvQueryConfig QueryConfig;
};
