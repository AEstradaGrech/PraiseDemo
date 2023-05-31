// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../Enums/AI/EAI_Msg.h"
#include "../../Structs/AI/FTelegram.h"
#include  "../../Networking/PraiseGameMode.h"
#include <set>
#include "MsgDispatcherComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMsgBroadcastSignature, FTelegram&, Msg);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRAISE_API UMsgDispatcherComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMsgDispatcherComponent();
	FORCEINLINE bool IsEnabled() const { return bIsEnabled; }
	FORCEINLINE void SetEnablded(bool bValue) { bIsEnabled = bValue; SetComponentTickEnabled(bIsEnabled); }
	FORCEINLINE void SetDefaultAdmitThrsh(float Admt) { DefaultAdmitThrsh = Admt; }
	typedef std::set<FTelegram> TelegramQ;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SendMsg(int SenderId, int ReceiverId, EAI_Msg Msg, float Delay);
	void SendMsg(struct FTelegram& Telegram);
	void BroadcastMsg(struct FTelegram& Telegram);
	FOnMsgBroadcastSignature OnMsgBroadcast;

private:
	void Discharge(class ABasePraiseCharacter* pReceiver, struct FTelegram& Telegram);
	void DischargeDelayedMessages(float DeltaTime);
	void AddToPriorityQ(FTelegram& Msg);
	float DefaultAdmitThrsh = .5f;

	UPROPERTY(VisibleAnywhere)
		bool bIsEnabled;

	TelegramQ PriorityQ;
	class APraiseGameMode* GameMode;
	class UAIMsgCmdFactory* MsgCmdFactory;
};
