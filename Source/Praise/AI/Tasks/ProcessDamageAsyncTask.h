#pragma once
#include "CoreMinimal.h"
#include "../../Components/Actor/AI/BotBrainComponent.h"

#include "Async/AsyncWork.h"

class PRAISE_API ProcessDamageAsyncTask : public FNonAbandonableTask // si se le quita esto pide que se implemente Abandon(). 
																	// Pone que se llama siempre a ese metodo cada vez que se quita una tarea sin empezar de la cola
																	// se podria usar para las tareas de ProcessRecentDamage que se disparan cada vez que recibe un golpe: TODO investigar
{

public:
	ProcessDamageAsyncTask();
	ProcessDamageAsyncTask(class ABaseBotCharacter* Bot);
	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(ProcessDamageAsyncTask, STATGROUP_ThreadPoolAsyncTasks); }

	void DoWork();

private:
	class ABaseBotCharacter* Bot;
};

