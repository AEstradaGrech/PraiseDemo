#pragma once
#include "../../Enums/AI/EAI_Msg.h"
#include "../../Enums/AI/EAI_MsgChannel.h"
#include "../../AI/MsgCommands/BaseMsgCommand.h"
#include <math.h>
#include "FTelegram.generated.h"

USTRUCT()
struct PRAISE_API FTelegram
{
	GENERATED_BODY()

	FTelegram() : Msg(EAI_Msg::STAY_IN_PLACE), Sender(-1), DispatchTime(0), AdmittanceThreshold(-1) {};

	FTelegram(EAI_Msg Msg, int SenderId, float Delay = 0, float AdmittanceThreshold = 0 ) :
		Msg(Msg), Sender(SenderId), DispatchTime(Delay), AdmittanceThreshold(AdmittanceThreshold) {};
	
	EAI_Msg Msg;	
	int Sender;	
	float DispatchTime;
	float AdmittanceThreshold = 0.5f;

	UBaseMsgCommand* Cmd;

	FORCEINLINE bool IsValidMsg() const { return Cmd != nullptr; }
	FORCEINLINE bool IsDirectMsg() const { return Cmd && Cmd->GetReceiver(); }

	void SetMsgCmd(UBaseMsgCommand* Command, EAI_MsgChannel Channel)
	{
		Cmd = Command;
		Cmd->SetCmdMsg(Msg);
		Cmd->SetMsgChannel(Channel);
	};
};

inline bool operator==(const FTelegram& t1, const FTelegram& t2)
{
	return (FMath::Abs(t1.DispatchTime - t2.DispatchTime) < t1.AdmittanceThreshold) &&
		t1.AdmittanceThreshold == t2.AdmittanceThreshold &&
		t1.Sender == t2.Sender &&
		t1.IsDirectMsg() == t2.IsDirectMsg() && 
		t1.Msg == t2.Msg;
}

inline bool operator<(const FTelegram& t1, const FTelegram& t2) { return t1 == t2 ? false : (t1.DispatchTime < t2.DispatchTime); }