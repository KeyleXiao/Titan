#pragma once
#include "Handler.h"
#include "MngConnector.h"
#include "ViewMsgDef_Server.h"
#include "Setting.h"

class HandshakeResponseHandler : public Handler<MngConnector, SMsgView_MV_HandshakeResponse>
{
protected:
	virtual void Handle(MngConnector* pSession, SGameMsgHead* head, SMsgView_MV_HandshakeResponse* pMsg)
	{
		pSession->SetID(pMsg->dwID);
		gSetting.SetID(pMsg->dwID);

		TraceLn(_GT("收到管理服回应握手信息! ") << a2utf8(pSession->ToString().data()));
	}
};
