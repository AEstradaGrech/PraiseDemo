// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_TriggerRoar.h"
#include "../../../Characters/AI/BotCharacters/BasicZombieCharacter.h"
#include "../../../Characters/AI/BTBotController.h"
#include "../../../Components/Actor/AI/BTBrainComponent.h"
#include "../../../Enums/AI/EBrainType.h"
#include "../../../Structs/CommonUtility/FUtilities.h"


EBTNodeResult::Type UBTTaskNode_TriggerRoar::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	if (!OwnerComponent.GetOwner()) return EBTNodeResult::Failed;

	if (!OwnerComponent.GetOwner()->IsA<ABTBotController>()) return EBTNodeResult::Failed;

	ABTBotController* BotController = Cast<ABTBotController>(OwnerComponent.GetOwner());

	if (!BotController->HasBrainOfType<UBTBrainComponent>()) return EBTNodeResult::Failed;

	UBTBrainComponent* BotBrain = BotController->GetBrainAs<UBTBrainComponent>();

	if (!BotBrain) return EBTNodeResult::Failed;

	if (!BotController->GetPawn()->IsA<ABasicZombieCharacter>()) return EBTNodeResult::Failed;

	ABasicZombieCharacter* Bot = Cast<ABasicZombieCharacter>(BotController->GetPawn());

	bool bShouldRoar = FMath::RandRange(0, 100) <= ChanceToRoar;

	if (bShouldRoar)
	{
		Bot->TriggerRoar();
		BotBrain->GetBotBB()->SetValueAsBool(BB_IS_ROARING, bShouldRoar);
	}

	return EBTNodeResult::Succeeded;
}