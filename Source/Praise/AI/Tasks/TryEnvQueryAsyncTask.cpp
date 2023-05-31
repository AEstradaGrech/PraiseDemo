// Fill out your copyright notice in the Description page of Project Settings.


#include "TryEnvQueryAsyncTask.h"
#include "../CommonUtility/BotEnvQuery/BotEnvQuerier.h"
#include "../../Characters/AI/BotCharacters/BaseBotCharacter.h"


TryEnvQueryAsyncTask::TryEnvQueryAsyncTask()
{
}

TryEnvQueryAsyncTask::~TryEnvQueryAsyncTask()
{
}

TryEnvQueryAsyncTask::TryEnvQueryAsyncTask(UBotBrainComponent* Brain, UBotEnvQuerier* EnvQueryComp, FEnvQueryConfig Config)
{
	BotBrain = Brain;
	EnvQuerier = EnvQueryComp;
	QueryConfig = Config;
}

void TryEnvQueryAsyncTask::DoWork()
{
	if (!(BotBrain && EnvQuerier)) return;

	BotBrain->SetIsQueryingEnv(true);

	EnvQuerier->TryQuery(QueryConfig);

	TArray<FVector> BestLocations = EnvQuerier->GetBestResults(3);
	FColor SphereCol = FColor::Green;
	for (int i = 0; i < BestLocations.Num(); i++)
	{
		if (i == 1)
			SphereCol = FColor::Cyan;

		if (i == 2)
			SphereCol = FColor::White;
	}

	BotBrain->SetIsQueryingEnv(false);
}
