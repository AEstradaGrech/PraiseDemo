// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMsgCommand.h"
#include "../../Characters/BasePraiseCharacter.h"

UBaseMsgCommand::UBaseMsgCommand()
{
	bHasVisionCheck = false;
	Range = -1;
}
void UBaseMsgCommand::Copy(UBaseMsgCommand* Original)
{
	Sender = Original->GetSender();
	Receiver = Original->GetReceiver();
	Range = Original->GetMsgRange();
	bHasVisionCheck = Original->HasVisionCheck();
	CmdAdmittance = Original->GetCmdAdmittance();
	CmdMsg = Original->GetCmdMsg();
	MsgChannel = Original->GetMsgChannel();
	MsgOriginLocation = Original->GetMsgOriginLocation();
}
void UBaseMsgCommand::ExecuteCommand()
{

}

bool UBaseMsgCommand::CheckRange()
{
	if (HasRange())
	{
		FVector ToTarg = Sender->GetActorLocation() - Receiver->GetActorLocation();

		return ToTarg.Size() < Range;
	}

	return true;
}
