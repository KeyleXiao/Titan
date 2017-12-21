#pragma once
#include "Handler.h"
#include "MngConnector.h"
#include "ViewMsgDef_Server.h"
#include "Setting.h"

class HandshakeResponseHandler : public Handler<MngConnector, SMsgView_MV_HandshakeResponse>
{
protected:
	virtual void Handle(MngConnector* pMng, SGameMsgHead* head, SMsgView_MV_HandshakeResponse* pMsg)
	{
		pMng->SetID(pMsg->dwID);
		gSetting.SetID(pMsg->dwID);

		TraceLn(_GT("�յ��������Ӧ������Ϣ! ") << a2utf8(pMng->ToString().data()));
	}
};
