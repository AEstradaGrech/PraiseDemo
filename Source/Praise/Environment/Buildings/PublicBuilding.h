// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrivateBuilding.h"
#include "PublicBuilding.generated.h"


UCLASS()
class PRAISE_API APublicBuilding : public APrivateBuilding
{
	GENERATED_BODY()
	
public:
	APublicBuilding();
protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString BuildingLabel; 
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<ABaseBotCharacter>> DefaultOwners;
	
	TArray<class ABasBotCharacter*> BuildingOwners;
};
