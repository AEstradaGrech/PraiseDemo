// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_HandleZombieSpeed.h"
#include "../../../Structs/CommonUtility/FLogger.h"

void UBTService_HandleZombieSpeed::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
}

void UBTService_HandleZombieSpeed::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!CanExecute()) return;

	if (BotBrain->HasValidBBTarget(BB_PRIORITY_TARGET))
		HandleChaseTargetSpeed();

	else HandleWanderingSpeed();
}

void UBTService_HandleZombieSpeed::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
}

void UBTService_HandleZombieSpeed::HandleWanderingSpeed()
{
	if (BotBrain->GetBotBB()->GetValueAsBool(BB_HAS_VALID_LOCATION))
	{
		if (LastSprintTimeStamp == 0 || (LastSprintTimeStamp + CurrentAwaitSeconds <= BotBrain->GetTimeSinceGameStart()))
		{
			FLogger::LogTrace(__FUNCTION__ + FString(" :: UPDATE SPEED :: CHAR NAME --> " + BTPawn->GetName()), FColor::Yellow);
			//Sprint w Stam
			if (FMath::RandRange(0, 100) > 80)
			{
				FLogger::LogTrace(__FUNCTION__ + FString(" :: RESTORING DEFAULT SPEED "), FColor::Green);
				BTPawn->RestoreWalkSpeed();
			}
			else
			{
				if (BTPawn->GetScaledSpeed() == 1)
				{
					FLogger::LogTrace(__FUNCTION__ + FString(" :: DECREASING SPEED "), FColor::Red);
					BTPawn->HandleWalkSpeed(FMath::RandRange(0.3f, 0.5f), true, false);
				}
			}
			
			CurrentAwaitSeconds = SprintAwaitSeconds + (SprintAwaitSeconds * SprintSecondsDeviation * FMath::Sign(FMath::RandRange(-1, 1)));

			LastSprintTimeStamp = BotBrain->GetTimeSinceGameStart();
		}
	}
}

void UBTService_HandleZombieSpeed::HandleChaseTargetSpeed()
{
	BTPawn->RestoreWalkSpeed();
}