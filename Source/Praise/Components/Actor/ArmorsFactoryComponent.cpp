// Fill out your copyright notice in the Description page of Project Settings.


#include "ArmorsFactoryComponent.h"
#include "../../Components/Actor/DbComponents/ArmorsDBComponent.h"

// Sets default values for this component's properties
UArmorsFactoryComponent::UArmorsFactoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	ArmorsDB = CreateDefaultSubobject<UArmorsDBComponent>(TEXT("Armors DB"));


	// ...
}


// Called when the game starts
void UArmorsFactoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

