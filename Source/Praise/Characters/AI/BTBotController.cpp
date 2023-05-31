// Fill out your copyright notice in the Description page of Project Settings.


#include "BTBotController.h"
#include "BotCharacters/BaseBotCharacter.h"

ABTBotController::ABTBotController() : Super()
{
	BotBTComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Bot BT Component"));
	BotBlackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Bot Blackboard"));
}

void ABTBotController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

bool ABTBotController::TryInitBT()
{
	if (GetPawn() && !GetPawn()->IsA<ABaseBotCharacter>()) return false;

	ABaseBotCharacter* Bot = Cast<ABaseBotCharacter>(GetPawn());

	if (Bot->GetBrainType() != EBrainType::BT_BRAIN) return false;

	if (!Bot->GetBotDefaultBT()) return false;

	if (!Bot->GetBotDefaultBT()->BlackboardAsset) return false;

	if (BotBTComp && BotBlackboard->InitializeBlackboard(*Bot->GetBotDefaultBT()->BlackboardAsset))
	{
		BotBTComp->StartTree(*Bot->GetBotDefaultBT());
		return true;
	}
	
	return false;
}
