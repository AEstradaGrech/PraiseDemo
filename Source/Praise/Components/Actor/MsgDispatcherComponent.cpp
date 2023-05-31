// Fill out your copyright notice in the Description page of Project Settings.


#include "MsgDispatcherComponent.h"
#include "../../Characters/BasePraiseCharacter.h"

// Sets default values for this component's properties
UMsgDispatcherComponent::UMsgDispatcherComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMsgDispatcherComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UMsgDispatcherComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DischargeDelayedMessages(DeltaTime);
	// ...
}

void UMsgDispatcherComponent::SendMsg(int SenderId, int ReceiverId, EAI_Msg MsgId, float Delay)
{

}


void UMsgDispatcherComponent::SendMsg(FTelegram& Telegram)
{
	if (!Telegram.IsValidMsg()) return;

	if (Telegram.DispatchTime <= 0.f)
	{
		if (Telegram.IsDirectMsg())
			Discharge(Telegram.Cmd->GetReceiver(), Telegram);

		else BroadcastMsg(Telegram);
	}
		

	else AddToPriorityQ(Telegram);
}

void UMsgDispatcherComponent::BroadcastMsg(FTelegram& Telegram)
{
	if (Telegram.DispatchTime <= 0)
		OnMsgBroadcast.Broadcast(Telegram);

	else AddToPriorityQ(Telegram);
}

void UMsgDispatcherComponent::Discharge(ABasePraiseCharacter* pReceiver, FTelegram& Telegram)
{
	pReceiver->HandleMessage(Telegram);
}

void UMsgDispatcherComponent::DischargeDelayedMessages(float DeltaTime)
{
	if (PriorityQ.size() <= 0) return;

	if (PriorityQ.begin()->DispatchTime < GetWorld()->GetTimeSeconds() &&
		PriorityQ.begin()->DispatchTime > 0)
	{
		if (PriorityQ.begin()->IsDirectMsg())
		{
			ABasePraiseCharacter* Receiver = PriorityQ.begin()->Cmd->GetReceiver();

			if (Receiver == nullptr) { FLogger::LogTrace(__FUNCTION__ + FString(" - NO RECEIVER FOUND FOR ID :: ") + FString::FromInt(Receiver->GetUniqueID())); return; }

			FLogger::LogTrace(FString("DELAYED TELEGRAM TO :: ") + Receiver->GetName() + FString(" :: AT :: ") + FString::SanitizeFloat(GetWorld()->GetTimeSeconds()));

			FTelegram t = *(PriorityQ.begin());

			Discharge(Receiver, t);

			return;
		}

		FTelegram t = *(PriorityQ.begin());

		OnMsgBroadcast.Broadcast(t);

		PriorityQ.erase(PriorityQ.begin());
	}
}

void UMsgDispatcherComponent::AddToPriorityQ(FTelegram& Msg)
{
	if (!GameMode) return;

	float DispatchTime = GameMode->GetWorld()->GetTimeSeconds() + Msg.DispatchTime;

	Msg.DispatchTime = DispatchTime;

	PriorityQ.insert(Msg);
}

