#pragma once
#include "Engine/Engine.h"
#include "../../Enums/CommonUtility/ELogEnums.h"
#include "FLogger.generated.h"

USTRUCT()
struct PRAISE_API FLogger
{
	GENERATED_BODY()

	static void LogTrace(FString Trace) { Log(ELogOutput::OUTPUT_LOG, ELogLevel::WARNING, Trace); }
	static void LogTrace(FString LogMssg, FColor LogColor) { LogToScreen(LogMssg, LogColor); }
	static void LogNumber(int Trace, FString FuncName = "") { Log(ELogOutput::OUTPUT_LOG, ELogLevel::WARNING, (FuncName == "" ? FString("TRACE INT ") : FuncName) + FString(" :: ") + FString::FromInt(Trace)); }
	static void Log(ELogLevel logLevel, FString LogMssg) { Log(ELogOutput::ALL, logLevel, LogMssg); }

	static void Log(ELogOutput LogOutput, ELogLevel logLevel, FString LogMssg)
	{
		switch (LogOutput)
		{
			case ELogOutput::ALL:
				LogToConsole(logLevel, LogMssg);
				LogToScreen(logLevel, LogMssg);
				break;
			case ELogOutput::SCREEN:
				LogToScreen(logLevel, LogMssg);
				break;
			case ELogOutput::OUTPUT_LOG:
				LogToConsole(logLevel, LogMssg);
				break;
			default:
				LogToConsole(logLevel, LogMssg);
				break;
		}
	}

private:

	static void LogToScreen(ELogLevel logLevel, FString LogMssg)
	{
		FColor LogColor = FColor::Red;

		switch (logLevel)
		{
		case ELogLevel::TRACE:
			LogColor = FColor::Green;
			break;
		case ELogLevel::DEBUG:
			LogColor = FColor::Cyan;
			break;
		case ELogLevel::INFO:
			LogColor = FColor::White;
			break;
		case ELogLevel::WARNING:
			LogColor = FColor::Yellow;
			break;
		case ELogLevel::ERROR:
			LogColor = FColor::Red;
			break;
		default:
			LogColor = FColor::Red;
			break;
		}

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, LogColor, LogMssg, true);
	}

	static void LogToScreen(FString LogMssg, FColor LogColor)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, LogColor, LogMssg, true);
	}

	static void LogToConsole(ELogLevel logLevel, FString LogMssg)
	{
		switch (logLevel)
		{
		case ELogLevel::TRACE:
			UE_LOG(LogTemp, VeryVerbose, TEXT("%s"), *LogMssg);
			break;
		case ELogLevel::DEBUG:
			UE_LOG(LogTemp, Verbose, TEXT("%s"), *LogMssg);
			break;
		case ELogLevel::INFO:
			UE_LOG(LogTemp, Log, TEXT("%s"), *LogMssg);
			break;
		case ELogLevel::WARNING:
			UE_LOG(LogTemp, Warning, TEXT("%s"), *LogMssg);
			break;
		case ELogLevel::ERROR:
			UE_LOG(LogTemp, Error, TEXT("%s"), *LogMssg);
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("%s"), *LogMssg);
			break;
		}
	}
};