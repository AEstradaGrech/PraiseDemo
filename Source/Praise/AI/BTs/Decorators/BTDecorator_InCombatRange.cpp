// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_InCombatRange.h"
#include "../../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../../Characters/AI/BTBotController.h"
#include "../../../Components/Actor/AI/BTBrainComponent.h"
#include "../../../Structs/CommonUtility/FLogger.h"


bool UBTDecorator_InCombatRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (!OwnerComp.GetOwner()) return false;

	if (!OwnerComp.GetOwner()->IsA<ABTBotController>()) return false;

	ABTBotController* Controller = Cast<ABTBotController>(OwnerComp.GetOwner());

	if (!Controller->GetPawn()) return false;

	if (!Controller->GetPawn()->IsA<ABaseBotCharacter>()) return false;

	ABaseBotCharacter* Bot = Cast<ABaseBotCharacter>(Controller->GetPawn());

	if (!Controller->HasBrainOfType<UBTBrainComponent>()) return false;

	UBTBrainComponent* Brain = Controller->GetBrainAs<UBTBrainComponent>();

	if (!Brain->HasValidBBTarget(BB_PRIORITY_TARGET)) return false;
	
	bool bInMeleeRange = Bot->IsInMeleeRange(Brain->GetPriorityTarget(), Bot->GetMeleeDistanceOffset());

	bool bInCombatRange = Bot->IsInCombatRange(Brain->GetPriorityTarget(), Bot->GetMeleeDistanceOffset());

	FLogger::LogTrace(__FUNCTION__ + FString(" :: IN MELEE RANGE --> ") + (bInMeleeRange ? FString(" TRUE ") : FString(" FALSE ")), FColor::Green);
	FLogger::LogTrace(__FUNCTION__ + FString(" :: IN COMBAT RANGE --> ") + (bInCombatRange ? FString(" TRUE ") : FString(" FALSE ")));

	return bCheckMeleeRange ? Bot->IsInMeleeRange(Brain->GetPriorityTarget(), Bot->GetMeleeDistanceOffset()) : Bot->IsInCombatRange(Brain->GetPriorityTarget(), Bot->GetMeleeDistanceOffset());
}