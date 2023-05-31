// Fill out your copyright notice in the Description page of Project Settings.


#include "SeekDecision.h"
#include "../../Interfaces/CombatSystem/Targeteable.h"
#include "../../Components/Actor/AI/T200BrainComponent.h"
#include "../../Characters/AI/BotCharacters/FightingBotCharacter.h"
#include "../../Enums/AI/EChaseMode.h"

USeekDecision::USeekDecision() : Super()
{
	bIsLimitedDistance = false;
	bIsSeekingAnyTarget = true;
	bIsSeekingLivingTargets = false;
	bShouldSeekClosestTarget = true;
	bShouldDoFactionCheck = true;
	ETargetType::PLAYER;
	SearchRadius = KINDA_SMALL_NUMBER;

	LastUpdateTimestamp = 0;
	UpdateTargetSeconds = 10.f;
}

bool USeekDecision::HasDecided(FDecisionOutput& DecisionOutput, UBotDecision* PreviousDecision)
{
	Super::HasDecided(DecisionOutput, PreviousDecision);

	ITargeteable* Targ = Cast<ITargeteable>(BotBrain->GetPriorityTarget());

	if (BotBrain->GetPriorityTarget())
	{		
		if(Cast<ITargeteable>(BotBrain->GetPriorityTarget())->IsDead() && !BotBrain->GetBotBB()->GetValueAsBool(BB_DEAD_TARGETS_ALLOWED))
			BotBrain->GetBotBB()->ClearValue(BB_PRIORITY_TARGET);

		return false; 
	}

	if(ShouldUpdateTargets()) 
		UpdateTargets();
	
	if (CurrentTargets.Num() <= 0)
		UpdateTargets();

	if (CurrentTargets.Num() > 0 && !BotBrain->GetBot<ABaseBotCharacter>()->IsDead()) 
	{
		AActor* Target = CurrentTargets[0];
		if (bShouldSeekClosestTarget)
		{
			FVector Distance = FVector::ZeroVector;
			for (int i = 0; i < CurrentTargets.Num(); i++)
			{
				if (!CurrentTargets[i]) continue;
				
				if (bIsSeekingLivingTargets && CurrentTargets[i]->Implements<UTargeteable>() && Cast<ITargeteable>(CurrentTargets[i])->IsDead()) continue;
					
				FVector ToTarget = CurrentTargets[i]->GetActorLocation() - BotBrain->GetBot<AActor>()->GetActorLocation();

				if (Distance == FVector::ZeroVector || ToTarget.Size() < Distance.Size())
				{
					Distance = ToTarget;
					Target = CurrentTargets[i];
				}
			}
			
		}

		else Target = CurrentTargets[FMath::Clamp(FMath::RandRange(0, CurrentTargets.Num() - 1), 0 , CurrentTargets.Num())];

		if (!Target) return false;

		if (Cast<ITargeteable>(Target)->IsDead() && (bIsSeekingLivingTargets || !BotBrain->GetBotBB()->GetValueAsBool(BB_DEAD_TARGETS_ALLOWED))) return false;

		BotBrain->SetPriorityTarget(Target);

		if (bSetTargetAsPersonalEnemy)
			BotBrain->GetBot<ABaseBotCharacter>()->AddPersonalFoE(Target, false); 

		return IsChained() ? ChainResult(DecisionOutput) : true;
	}

	return false;
}

void USeekDecision::UpdateTargets()
{
	CurrentTargets = GetTargets();
	
	LastUpdateTimestamp = BotBrain->GetTimeSinceGameStart();
}

bool USeekDecision::ShouldUpdateTargets()
{
	return LastUpdateTimestamp == 0 ? true : BotBrain->GetTimeSinceGameStart() > LastUpdateTimestamp +  (UpdateTargetSeconds <= 0.f ? 10.f : UpdateTargetSeconds);
}

TArray<AActor*> USeekDecision::GetTargets()
{
	TArray<AActor*> OutTargets;
	UGameplayStatics::GetAllActorsWithInterface(BotBrain->GetWorld(), UTargeteable::StaticClass(), OutTargets);
	if (bIsSeekingAnyTarget)
		return OutTargets;

	TArray<AActor*> FilteredTargets;
	for (AActor* Target : OutTargets)
	{
		if (Target == BotBrain->GetBot<AActor>()) continue;

		if (Cast<ITargeteable>(Target)->IsDead()) continue;

		if (bShouldDoFactionCheck)
		{
			if (Target->Implements<UFactioneable>())
			{
				IFactioneable* FactioneableBot = Cast<IFactioneable>(BotBrain->GetBot<ABaseBotCharacter>());

				if (!FactioneableBot) continue;

				if(FactioneableBot->IsFriendTarget(Cast<IFactioneable>(Target)) || FactioneableBot->GetIgnoredFactions().Contains(Cast<IFactioneable>(Target)->FactionID()))
					continue;
			}
		}

		if (bIsSeekingLivingTargets) 
		{
			if (Cast<ITargeteable>(Target)->IsDead()) continue;
			
			if (Cast<ITargeteable>(Target)->IsTargetType() == ETargetType::PLAYER ||
				Cast<ITargeteable>(Target)->IsTargetType() == ETargetType::NPC) {
				if (bIsLimitedDistance && IsTargetInRange(Target)) 
					FilteredTargets.Add(Target);
				
				else FilteredTargets.Add(Target);	
			}
		}
		else
		{
			if (Cast<ITargeteable>(Target))
			{
				switch (TargetType)
				{
				case(ETargetType::PLAYER):
					ProcessTarget(Cast<ITargeteable>(Target), ETargetType::PLAYER, FilteredTargets);
					break;
				case(ETargetType::NPC):
					ProcessTarget(Cast<ITargeteable>(Target), ETargetType::NPC, FilteredTargets);
					break;
				case(ETargetType::ITEM):
					ProcessTarget(Cast<ITargeteable>(Target), ETargetType::ITEM, FilteredTargets);
					break;
				case(ETargetType::VIP):
					ProcessTarget(Cast<ITargeteable>(Target), ETargetType::VIP, FilteredTargets);
					break;
				default:
					break;
				}
			}
		}
	}

	return FilteredTargets;
}

bool USeekDecision::IsTargetInRange(AActor* Target)
{
	if (!bIsLimitedDistance) return true;

	FVector ToTarget = Target->GetActorLocation() - BotBrain->GetBot<ABaseBotCharacter>()->GetActorLocation();

	return ToTarget.Size() <= SearchRadius;
}

void USeekDecision::ProcessTarget(ITargeteable* Target, ETargetType MatchType, TArray<AActor*>& FilteredTargets)
{
	if (Target->IsTargetType() == MatchType && !Target->IsDead())
	{
		if (bIsLimitedDistance)
		{
			if (IsTargetInRange(Cast<AActor>(Target)))
				FilteredTargets.Add(Cast<AActor>(Target));
		}

		else FilteredTargets.Add(Cast<AActor>(Target));
	}
}
