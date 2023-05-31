// Fill out your copyright notice in the Description page of Project Settings.


#include "CreaturesFaction.h"
#include "../../../Praise.h"
#include "../../../Characters/AI/BotCharacters/BaseBotCharacter.h"

ACreaturesFaction::ACreaturesFaction() : Super()
{
	ConstructorHelpers::FClassFinder<ABaseBotCharacter> SeekerDemonCharClass(TEXT("/Game/Core/Blueprints/Characters/Bots/BP_SeekerDemonBotCharacter"));

	if (SeekerDemonCharClass.Succeeded())
		BotBPs.Add(BP_SEEKER_DEMON_BOT, SeekerDemonCharClass.Class);
}

void ACreaturesFaction::CacheBotBPs()
{
	Super::CacheBotBPs();

	
}
