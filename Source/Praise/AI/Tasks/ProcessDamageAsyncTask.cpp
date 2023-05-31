#include "ProcessDamageAsyncTask.h"
#include "../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../Components/Actor/AI/BotBrainComponent.h"

ProcessDamageAsyncTask::ProcessDamageAsyncTask()
{

}

ProcessDamageAsyncTask::ProcessDamageAsyncTask(ABaseBotCharacter* ProcessedBot) 
{
	Bot = ProcessedBot;
}

void ProcessDamageAsyncTask::DoWork()
{
	//TimeCheck?
	if (!Bot || !Bot->GetBrain()) return;

	Bot->GetBrain()->ProcessRecentDamage();
	Bot->GetBrain()->ProcessTotalDamage();
}