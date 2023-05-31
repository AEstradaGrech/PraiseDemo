// Fill out your copyright notice in the Description page of Project Settings.


#include "PublicBuilding.h"

APublicBuilding::APublicBuilding() : Super()
{

}

void APublicBuilding::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Tags.Add(FName("PublicBuilding"));

	if(!BuildingLabel.IsEmpty())
		Tags.Add(*BuildingLabel);
}

void APublicBuilding::BeginPlay()
{
	Super::BeginPlay();
}
