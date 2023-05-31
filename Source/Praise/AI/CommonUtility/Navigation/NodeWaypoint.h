// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIWaypoint.h"

#include "../../../Enums/Characters/ECharFaction.h"
#include "NodeWaypoint.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API ANodeWaypoint : public AAIWaypoint
{
	GENERATED_BODY()

public:

	ANodeWaypoint();

	FORCEINLINE int GetNodeID() const { return NodeID; }
	FORCEINLINE FString GetNodeName() const { return NodeName; }
	FORCEINLINE ECharFaction FactionID() const { return Faction; }
	FORCEINLINE FString GetZoneName() const { return ZoneName; }
	FORCEINLINE TArray<FString> GetAdjacentNodes() const { return AdjacentNodes; }
	FORCEINLINE TMap<FString, class UAIPath*> GetNodePaths() const { return NodePaths; }
	

	bool TryAddPath(class UAIPath* NewPath);
	void SetNodeID(int Value);
	void AddAdjacentNodeTag(FString Tag);
	bool IsValid() const;
	bool GetRoute_BFS(ANodeWaypoint* CurrentNode, ANodeWaypoint* Destination, TArray<ANodeWaypoint*>& AvailableNodes, TArray<ANodeWaypoint*>& CheckedNodes, TQueue<ANodeWaypoint*>& SearchQ);
protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		ECharFaction Faction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FString ZoneName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		int NodeID;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		FString NodeName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TArray<FString> AdjacentNodes;

	UPROPERTY(EditAnywhere)
		bool bDebugNode;
	TMap<FString, class UAIPath*> NodePaths; 

};


