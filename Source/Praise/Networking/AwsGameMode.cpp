// Fill out your copyright notice in the Description page of Project Settings.


#include "AwsGameMode.h"

AAwsGameMode::AAwsGameMode() : Super()
{

}

void AAwsGameMode::BeginPlay()
{
	Super::BeginPlay();

#if WITH_GAMELIFT

	auto InitSDKOutcome = Aws::GameLift::Server::InitSDK();

	if (InitSDKOutcome.IsSuccess())
	{
		//Gamelift invoca OnStartGameSession para informar a nuestro proceso de que alguien quiere iniciar una sesion
		// GameSession lo pasa SDK, Params yo.
		auto OnStartGameSession = [](Aws::GameLift::Server::Model::GameSession GameSessionObj, void* Params)
		{
			FStartGameSessionState* State = (FStartGameSessionState*)Params;

			// cuando se invoca el callback "OnStartGameSession" el proceso del servidor tiene que informar a GameLift de que ha empezado una sesion y esta listo para recibir conexiones
			State->Status = Aws::GameLift::Server::ActivateGameSession().IsSuccess();
		}

		// callback para actualizar la partida. por ejemplo, añadiendo jugadores a una partida ya empezada
		auto OnUpdateGameSession = [](void* Params)
		{
			FUpdateGameSessionState = (FUpdateGameSessionState*)Params;
		}

		// callback que se invoca manual o automaticamente
		auto OnProcessTerminate = [](void* Params)
		{
			FProcessTerminateState* State = (FProcessTerminateState*)Params;

			// se puede sacar el limite de tiempo de la sesion con este metodo
			auto TerminationTimeOutcome = Aws::GameLift:Server::GetTerminationTime();
			// guardarlo en el struct que hace de puente entre AWS y UE4
			if (TerminationTimeOutcome.IsSuccess())
			{
				// segun el tiempo que quede y las caracteristicas del juego este seria el lugar para 
				// gestionar cualquier tipo de server cleanup etc (Flopperarm dice: 'no lo voy a usar mucho en el tuto, pero se puede hacer'. simplemente ilustrativo)
				State->TerminationTime = TerminationTimeOutcome.GetResult();
			}

			auto ProcessTerminateOutcome = Aws::GameLift::Server::ProcessEnding();

			if (ProcessTerminateOutcome.IsSuccess())
			{
				State->Status = true;

				FGenericPlatformMisc::RequestExit(false);
			}
		}

		auto OnHealthCheck = [](void* Params)
		{
			FHealthCheckState* State = (FHealthCheckState*)Params;
			State->Status = true;

			return State->Status;
		}

		// recuperar datos de los cli commands para configurar el server
		TArray<FString> CommandTokens;
		TArray<FString> CommandSwitches; // plain switch: '-port'. kvp switch: '-port=X'
		int Port = FURL::UrlConfig.DefaultPort;

		FCommandLine::Parse(FCommandLine.Get(), CommandTokens, CommandSwitches);

		for (FString CmdSwitch : CommandSwitches)
		{
			FString Key;
			FString Value;

			if (CmdSwitch.Split('='), &Key, &Value)
				if (Key.Equals('port'))
					Port = FCString::Atoi(Value);

		}

		// expulsar los logs que se generan al empezar una sesion
		const char* LogFile = "aLogFile.txt";
		const char* LogFiles = &LogFile;

		auto LogParams = new Aws::GameLift::Server::LogParameters(LogFiles, 1);

		//añadir params del proceso
		auto Params = new Aws::GameLift::Server::ProcessParameters(
			OnStartGameSession,
			&StartGameSessionState,
			OnUpdateGameSession,
			&UpdateGameSessionState,
			OnProcessTerminate,
			&ProcessTerminateState,
			OnHealthCheck,
			&HealthCheckState,
			Port,
			*LogParams);
		
		// iniciar proceso
		auto ProcessReadyOutcome = Aws::GameLift::Server::ProcessReady(*Params);

		//if (!ProcessReadyOutcome.IsSuccess())
		//{
		//	//gestion de error
		//}
	}
#endif
}
