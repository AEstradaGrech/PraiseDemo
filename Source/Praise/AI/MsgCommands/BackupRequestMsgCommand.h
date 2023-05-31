// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMsgCommand.h"
#include "BackupRequestMsgCommand.generated.h"

UCLASS()
class PRAISE_API UBackupRequestMsgCommand : public UBaseMsgCommand
{
	GENERATED_BODY()
	
public:
	UBackupRequestMsgCommand();
	virtual void Copy(UBaseMsgCommand* Original) override;
	virtual void ExecuteCommand() override;

private:
	void HandleBackupRequest();
	void UpdateTargetLocation();
};
