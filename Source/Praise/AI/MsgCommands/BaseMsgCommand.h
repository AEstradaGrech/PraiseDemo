// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../../Structs/CommonUtility/FLogger.h"
#include "../../Enums/AI/EAI_Msg.h"
#include "../../Enums/AI/EAI_MsgChannel.h"
#include "BaseMsgCommand.generated.h"

/**
 * 
 */
UCLASS()
class PRAISE_API UBaseMsgCommand : public UObject
{
	GENERATED_BODY()
	
public:
	UBaseMsgCommand();

	virtual void Copy(UBaseMsgCommand* Original);

	FORCEINLINE float GetCmdAdmittance() const { return CmdAdmittance; }
	FORCEINLINE bool HasVisionCheck() const { return bHasVisionCheck; }
	FORCEINLINE bool HasRange() const { return Range > 0; }
	FORCEINLINE float GetMsgRange() const { return Range; }
	FORCEINLINE EAI_Msg GetCmdMsg() const { return CmdMsg; }
	FORCEINLINE EAI_MsgChannel GetMsgChannel() const { return MsgChannel; }
	FORCEINLINE class ABasePraiseCharacter* GetSender() const { return Sender; }
	FORCEINLINE class ABasePraiseCharacter* GetReceiver() const { return Receiver; }
	FORCEINLINE FVector GetMsgOriginLocation() const { return MsgOriginLocation; }


	FORCEINLINE void SetMsgRange(float Value) { Range = Value; }
	FORCEINLINE void SetHasVisionCheck(bool bValue) { bHasVisionCheck = bValue; }
	FORCEINLINE void SetSender(class ABasePraiseCharacter* Actor) { Sender = Actor; }
	FORCEINLINE void SetReceiver(class ABasePraiseCharacter* Actor) { Receiver = Actor; }
	FORCEINLINE void SetCmdMsg(EAI_Msg Value) { CmdMsg = Value; }
	FORCEINLINE void SetMsgChannel(EAI_MsgChannel Value) { MsgChannel = Value; }
	FORCEINLINE void SetOriginLocation(FVector Location) { MsgOriginLocation = Location; }
	

	virtual void ExecuteCommand();

protected:
	EAI_Msg CmdMsg;
	EAI_MsgChannel MsgChannel;

	float CmdAdmittance = 0.f;
	class ABasePraiseCharacter* Sender;
	class ABasePraiseCharacter* Receiver;

	float Range;
	bool bHasVisionCheck;

	UPROPERTY()
		FVector MsgOriginLocation;

	bool CheckRange();
};
