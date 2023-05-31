// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMsgCommand.h"
#include "TargetSeenMsgCommand.generated.h"

UCLASS()
class PRAISE_API UTargetSeenMsgCommand : public UBaseMsgCommand
{
	GENERATED_BODY()
	
public:
	UTargetSeenMsgCommand();
	virtual void ExecuteCommand() override;

	virtual void Copy(UBaseMsgCommand* Original) override;
	FORCEINLINE FVector GetTargetLocation() const { return TargetLocation; }
	FORCEINLINE void SetTargetLocation(FVector Loc) { TargetLocation = Loc; }
private:
	FVector TargetLocation;
};
