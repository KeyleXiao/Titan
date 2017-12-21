#pragma once
#include "MngConnector.h"
#include "Handler.h"
#include "ViewMsgDef_Server.h"
#include "Setting.h"
#include "AuthMng.h"

class HandshakeResponseHandler : public Handler<MngConnector, SMsgView_MG_HandshakeResponse>
{
protected:
	virtual void Handle(MngConnector* pSession, SGameMsgHead* head, SMsgView_MG_HandshakeResponse* pMsg)
	{
		pSession->SetID(pMsg->dwID);
		gSetting.SetID(pMsg->dwID);

		gAuthMng.SetMng(pSession);

		TraceLn(_GT("�յ��������Ӧ������Ϣ! ") << a2utf8(pSession->ToString().data()));
	}
};
