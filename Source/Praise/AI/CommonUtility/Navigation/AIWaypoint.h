// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../../Enums/AI/EWaypointType.h"
#include "../../../Enums/CombatSystem/ETargetType.h"
#include "../../../Enums/Characters/ECharStatus.h"
#include "../../../Interfaces/CombatSystem/Targeteable.h"
#include "AIWaypoint.generated.h"

UCLASS()
class PRAISE_API AAIWaypoint : public AActor
{
	GENERATED_BODY()
	
public:	

	AAIWaypoint();
	FORCEINLINE EWaypointType GetWaypointType() const { return WaypointType; }
	FORCEINLINE FString GetWaypointLabel() const { return WaypointLabel; }
	FORCEINLINE TArray<FString> GetWaypointTags() const { return WaypointTags; }
	FORCEINLINE bool IsBuildingWaypoint() const { return bIsBuildingNode; }
	FORCEINLINE bool IsWaitingSpot() const { return bIsWaitingSpot; }
	FORCEINLINE bool CanWanderArround() const { return bCanWanderArround; }
	FORCEINLINE float GetWaitingTime() const { return WaitingTime; }
	FORCEINLINE float GetWanderingRadius() const { return WanderingRadius; }

	bool TryAddTag(FString NewTag);
protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	void AddActorTags();
	virtual void SetupBuildingWaypoint();
	bool TryAddBuildingWaypoint(class ABaseBuilding*& OutBuilding);

	UPROPERTY(EditAnywhere, BlueprintReadonly)
		EWaypointType WaypointType;
	UPROPERTY(EditAnywhere, BlueprintReadonly)
		FString WaypointLabel;
	UPROPERTY(EditAnywhere, BlueprintReadonly)
		TArray<FString> WaypointTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bIsBuildingNode;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly)
		bool bIsWaitingSpot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WaitingTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanWanderArround;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WanderingRadius;

};
