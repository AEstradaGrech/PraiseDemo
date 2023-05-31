// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../AI/MsgCommands/BaseMsgCommand.h"
#include "../../Enums/AI/EAI_Msg.h"
#include <map>
#include "MsgCommandsFactoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRAISE_API UMsgCommandsFactoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMsgCommandsFactoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	typedef UBaseMsgCommand* (UMsgCommandsFactoryComponent::* pCreateCmdFn)();

	template<typename T>
	FORCEINLINE void RegisterCmd(EAI_Msg Msg) { CmdsMap[Msg] = &UMsgCommandsFactoryComponent::CreateCmd<T>; }

	void RegisterCmds();

	UBaseMsgCommand* GetCmd(EAI_Msg Msg);
	UBaseMsgCommand* GetCmd(EAI_Msg Msg, class ABasePraiseCharacter* Sender, class ABasePraiseCharacter* Receiver = nullptr);
	UBaseMsgCommand* CloneCmd(UBaseMsgCommand* Original);
private:
	typedef std::map<EAI_Msg, pCreateCmdFn> CommandsMap;
	CommandsMap CmdsMap;

	template<typename T>
	FORCEINLINE UBaseMsgCommand* CreateCmd() { return NewObject<T>(); }
};
