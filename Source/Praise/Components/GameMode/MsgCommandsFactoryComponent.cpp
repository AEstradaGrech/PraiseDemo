// Fill out your copyright notice in the Description page of Project Settings.


#include "MsgCommandsFactoryComponent.h"
#include "../../Characters/BasePraiseCharacter.h"
#include "../../AI/MsgCommands/BackupRequestMsgCommand.h"
#include "../../AI/MsgCommands/TargetSeenMsgCommand.h"

// Sets default values for this component's properties
UMsgCommandsFactoryComponent::UMsgCommandsFactoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	RegisterCmds();
	// ...
}


// Called when the game starts
void UMsgCommandsFactoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (CmdsMap.empty())
		RegisterCmds();
}


// Called every frame
void UMsgCommandsFactoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UBaseMsgCommand* UMsgCommandsFactoryComponent::GetCmd(EAI_Msg Msg)
{
	CommandsMap::iterator iterator = CmdsMap.find(Msg);

	if (iterator == CmdsMap.end()) return nullptr;

	return ((*this).*iterator->second)();
}

UBaseMsgCommand* UMsgCommandsFactoryComponent::GetCmd(EAI_Msg Msg, ABasePraiseCharacter* Sender, ABasePraiseCharacter* Receiver)
{
	UBaseMsgCommand* Cmd = GetCmd(Msg);

	Cmd->SetSender(Sender);

	if (Receiver != nullptr)
		Cmd->SetReceiver(Receiver);

	return Cmd;
}

UBaseMsgCommand* UMsgCommandsFactoryComponent::CloneCmd(UBaseMsgCommand* Original)
{
	UBaseMsgCommand* Clone = GetCmd(Original->GetCmdMsg());

	if (!Clone) return nullptr;

	Clone->Copy(Original);

	return Clone;
}


void UMsgCommandsFactoryComponent::RegisterCmds()
{
	RegisterCmd<UBackupRequestMsgCommand>(EAI_Msg::BACKUP_REQ);
	RegisterCmd<UTargetSeenMsgCommand>(EAI_Msg::TARGET_SEEN_AT);
}

