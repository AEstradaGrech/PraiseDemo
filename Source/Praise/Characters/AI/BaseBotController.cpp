// Fill out your copyright notice in the Description page of Project Settings.


#include "../AI/BaseBotController.h"
#include "../../Components/Actor/AI/BotBrainComponent.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../../Components/Actor/AI/T200BrainComponent.h"
#include "../../Components/Actor/AI/BTBrainComponent.h"
#include "kismet/GameplayStatics.h"
#include "../../Networking/PraiseGameMode.h"
#include "../../Characters/AI/BotCharacters/BaseBotCharacter.h"
#include "../../Characters/AI/BotCharacters/CitizenBotCharacter.h"
#include "../../Components/GameMode/BotSpawnerComponent.h"
#include "../../Enums/AI/EBotClass.h"
#include "../../AI/CommonUtility/Factions/GameFaction.h"
#include "../../AI/CommonUtility/Factions/FactionZone.h"
#include "../../Components/Actor/AI/StateMachineBrainComponent.h"
#include "../../Interfaces/AI/Factioneable.h"

ABaseBotController::ABaseBotController() : Super()
{

}

void ABaseBotController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!InPawn->IsA<ABaseBotCharacter>()) return;

	if (!HasAuthority()) return;

	if (!ensure(GetWorld() != nullptr)) return;

	ABaseBotCharacter* Bot = Cast<ABaseBotCharacter>(InPawn);

	SetBotBrain(Bot->GetBrainType());

	if (Bot->GetTargetingComponent()) 
		BotBrain->OnTargetUpdate.BindUFunction(Bot->GetTargetingComponent(), FName("ForceLockTarget"));
}

bool ABaseBotController::TryReachLocation(TArray<FVector>& Locations, FVector& SelectedLocation, float StoppingDistance, bool bWorstLocFirst)
{
	if (Locations.Num() <= 0) return false;

	TArray<FVector> LocsCopy = Locations;

	switch (bWorstLocFirst)
	{
		case(false):
			for (int i = 0; i < LocsCopy.Num(); i++)
			{
				MoveToLocation(LocsCopy[i], StoppingDistance, true, true, true);

				if (GetPathFollowingComponent()->HasPartialPath())
				{
					MoveToLocation(GetPawn()->GetActorLocation(), StoppingDistance, true, true, true);
					Locations.Remove(LocsCopy[i]);
					return TryReachLocation(Locations, SelectedLocation, StoppingDistance, bWorstLocFirst);
				}
				else
				{
					SelectedLocation = Locations[i];
					return true;
				}
			}
			break;
		case(true):
			for (int i = LocsCopy.Num() - 1; i >= 0; i--)
			{
				MoveToLocation(LocsCopy[i], StoppingDistance, true, true, true);

				if (GetPathFollowingComponent()->HasPartialPath())
				{
					MoveToLocation(GetPawn()->GetActorLocation(), StoppingDistance, true, true, true);
					Locations.Remove(LocsCopy[i]);
					return TryReachLocation(Locations, SelectedLocation, StoppingDistance, bWorstLocFirst);
				}
				else
				{
					SelectedLocation = Locations[i];
					return true;
				}
			}
			break;
		}

	return false;
}

void ABaseBotController::SetBotBrain(EBrainType BrainType)
{
	switch (BrainType)
	{
		case(EBrainType::DEFAULT):
			OverrideBrainComponent<UStateMachineBrainComponent>();
			break;
		case(EBrainType::T200):
			OverrideBrainComponent<UT200BrainComponent>();
			break;
		case(EBrainType::BT_BRAIN):
			OverrideBrainComponent<UBTBrainComponent>();
			break;
		default:
			break;
	}
}

