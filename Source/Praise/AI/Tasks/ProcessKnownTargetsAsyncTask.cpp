#include "ProcessKnownTargetsAsyncTask.h"
#include "../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../Interfaces/CombatSystem/Targeteable.h"
#include "../../Enums/CombatSystem/ETargetType.h"
#include "GameFramework/GameStateBase.h"
#include "../../Components/Actor/AI/BotBrainComponent.h"
#include "kismet/GameplayStatics.h"
#include "../../Characters/BasePraiseCharacter.h"
#include "../../Structs/CommonUtility/FLogger.h"
#include "../../Components/Actor/AI/T200BrainComponent.h"

ProcessKnownTargetsAsyncTask::ProcessKnownTargetsAsyncTask(){

}


ProcessKnownTargetsAsyncTask::ProcessKnownTargetsAsyncTask(ABaseBotCharacter* Bot)
{
	this->Bot = Bot;
	this->BotBrain = Bot->GetBrain();
}

void ProcessKnownTargetsAsyncTask::DoWork()
{
	//TimeCheck?
	ProcessTargets();
}

void ProcessKnownTargetsAsyncTask::ProcessTargets()
{
	//FLogger::LogTrace(__FUNCTION__);

	/*if(BotBrain)
		FLogger::LogTrace(__FUNCTION__ + FString(" :: UNPROCESSED TARGETS NUM --> ") + FString::FromInt(BotBrain->GetUnprocessedTargets().Num()));*/
	//TArray<int32> TargetIds = BotBrain->GetTargetIds();
	//TMap<float, AActor*> KnownTargets = BotBrain->GetKnownTargets();
	
	/*TArray<AActor*> Unprocessed = BotBrain->GetUnprocessedTargets();

	for (int i = 0; i < Unprocessed.Num(); i++){
		if (!Unprocessed[i]->GetClass()->ImplementsInterface(UTargeteable::StaticClass())) continue;
		
		ITargeteable* Target = Cast<ITargeteable>(Unprocessed[i]);

		switch (Target->IsTargetType())
		{
			case(ETargetType::PLAYER):
			case(ETargetType::NPC):
				ProcessCharacter(Cast<ABasePraiseCharacter>(Unprocessed[i]));
				break;
			case(ETargetType::ITEM):
				ProcessItem(Target);
				break;
			case(ETargetType::VIP):
				break;
		}
	}
	*/
	
}
//esto lo paso como ITargeteable porque da igual que sea un arma, una pocion o una piedra del suelo. se añade al array y ya se gestionaran segun la importancia / estado de FSM
void ProcessKnownTargetsAsyncTask::ProcessItem(ITargeteable* Item)
{
	//AActor* LastKnownItem = 
	BotBrain->TryAddKnownItem(Item);
}

void ProcessKnownTargetsAsyncTask::ProcessCharacter(ABasePraiseCharacter* Character)
{
	//Friend Or Foe ?
	//FLogger::LogTrace(__FUNCTION__ + FString(" :: PROCESSING CHAR --> ") + Character->GetName());

	//DEVONLY -- PARA CAMBIAR RAPIDO DE FACCION

	//if (BotBrain->IsA<UT200BrainComponent>()) {
	//	if (BotBrain->TryAddCharTarget(Character, /*bIsEnemy: */ true))
	//		BotBrain->GetUnprocessedTargets().Remove(Character);
	//	return;
	//}
	//switch (Character->FactionID()) {
	//	case(ECharFaction::NONE):
	//		if (BotBrain->TryAddCharTarget(Character, /*bIsEnemy: */ false))
	//			BotBrain->GetUnprocessedTargets().Remove(Character);
	//		break;
	//	case(ECharFaction::PUBLIC_ENEMY):
	//		if (Bot->FactionID() == ECharFaction::PUBLIC_ENEMY) {
	//			if (BotBrain->TryAddCharTarget(Character, /*bIsEnemy: */ false))
	//				BotBrain->GetUnprocessedTargets().Remove(Character);
	//		}
	//		else {
	//			if (BotBrain->TryAddCharTarget(Character, /*bIsEnemy: */ true))
	//				BotBrain->GetUnprocessedTargets().Remove(Character);
	//		}
	//		break;
	//	case(ECharFaction::PROTECTOR):
	//		if (Bot->FactionID() == ECharFaction::PUBLIC_ENEMY) {
	//			if (BotBrain->TryAddCharTarget(Character, /*bIsEnemy: */ true))
	//				BotBrain->GetUnprocessedTargets().Remove(Character);
	//		}
	//		else {
	//			if (BotBrain->TryAddCharTarget(Character, /*bIsEnemy: */ false))
	//				BotBrain->GetUnprocessedTargets().Remove(Character);
	//		}
	//		break;

	//}
		
}