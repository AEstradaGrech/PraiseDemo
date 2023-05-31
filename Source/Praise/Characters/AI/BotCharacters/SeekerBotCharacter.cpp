// Fill out your copyright notice in the Description page of Project Settings.


#include "SeekerBotCharacter.h"

ASeekerBotCharacter::ASeekerBotCharacter() : Super()
{
	bSeekOnlyPlayers = false;
	bSeekClosestTarget = true;
	SearchDistance = 0; 
	bShouldFightToDeath = true;
}