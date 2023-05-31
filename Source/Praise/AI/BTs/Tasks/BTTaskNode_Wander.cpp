// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_Wander.h"
#include "NavigationSystem.h"
#include "../../../Praise.h"
#include "../../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../../Characters/AI/BTBotController.h"
#include "../../../Components/Actor/AI/BTBrainComponent.h"
#include "../../../Enums/AI/EBrainType.h"
#include "../../../Structs/CommonUtility/FUtilities.h"

EBTNodeResult::Type UBTTaskNode_Wander::ExecuteTask(UBehaviorTreeComponent& OwnerComponent, uint8* NodeMemory)
{
	if (!OwnerComponent.GetOwner()) return EBTNodeResult::Failed;

	if (!OwnerComponent.GetOwner()->IsA<ABTBotController>()) return EBTNodeResult::Failed;

	ABTBotController* BotController = Cast<ABTBotController>(OwnerComponent.GetOwner());

	NavMesh = UNavigationSystemV1::GetCurrent(BotController->GetPawn()->GetWorld());

	if (!BotController->HasBrainOfType<UBTBrainComponent>()) return EBTNodeResult::Failed;
	
	UBTBrainComponent* BotBrain = BotController->GetBrainAs<UBTBrainComponent>();

	if (!BotBrain) return EBTNodeResult::Failed;
	
	BotBrain->GetBotBB()->SetValueAsBool(BB_HAS_VALID_LOCATION, false);
	BotBrain->GetBotBB()->SetValueAsVector(BB_PRIORITY_LOCATION, FVector::ZeroVector);

	BotBrain->GetBotBB()->SetValueAsFloat(BB_MAX_WANDERING, FMath::RandRange(MinWaitingTime, MaxWaitingTime));
	
	float Distance = FMath::RandRange(MinDistance, MaxDistance);
	FVector Location = BotBrain->GetBotBB()->GetValueAsVector(BB_SPAWN_LOC);
	
	if (!FUtilities::IsValidVector(Location))
		Location = BotController->GetPawn()->GetActorLocation();

	FNavLocation Loc;
	if (NavMesh->GetRandomPointInNavigableRadius(Location, Distance, Loc))
	{
		BotBrain->GetBotBB()->SetValueAsBool(BB_HAS_VALID_LOCATION, true);
		BotBrain->GetBotBB()->SetValueAsVector(BB_PRIORITY_LOCATION, Loc.Location);
	
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Succeeded;
}
