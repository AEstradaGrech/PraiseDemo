// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_ExecuteAttack.h"
#include "../../../Characters/AI/BotCharacters/BasicZombieCharacter.h"
#include "../../../Characters/AI/BTBotController.h"
#include "../../../Components/Actor/AI/BTBrainComponent.h"
#include "../../../Enums/AI/EBrainType.h"
#include "../../../Structs/CommonUtility/FUtilities.h"

EBTNodeResult::Type UBTTaskNode_ExecuteAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	if (!OwnerComponent.GetOwner()) return EBTNodeResult::Failed;

	if (!OwnerComponent.GetOwner()->IsA<ABTBotController>()) return EBTNodeResult::Failed;

	ABTBotController* BotController = Cast<ABTBotController>(OwnerComponent.GetOwner());

	if (!BotController->HasBrainOfType<UBTBrainComponent>()) return EBTNodeResult::Failed;

	UBTBrainComponent* BotBrain = BotController->GetBrainAs<UBTBrainComponent>();

	if (!BotBrain) return EBTNodeResult::Failed;

	if (!BotController->GetPawn()->IsA<ABasicZombieCharacter>()) return EBTNodeResult::Failed;

	ABasicZombieCharacter* Bot = Cast<ABasicZombieCharacter>(BotController->GetPawn());

	EAttackType AttackType = FMath::RandRange(0, 100) <= ChanceToSecondary ? EAttackType::SECONDARY_ATTACK : EAttackType::PRIMARY_ATTACK;

	//if (AttackType == EAttackType::PRIMARY_ATTACK)
		Bot->StartPrimaryAttack();

	//else Bot->StartSecondaryAttack();

	return EBTNodeResult::Succeeded;
}