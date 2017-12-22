#pragma once
#include "Handler.h"
#include "GateUser.h"
#include "ViewMngSetting.h"
#include "ViewList.h"
#include "ViewMsgDef_Server.h"


class GM_HandshakeHandler : public Handler<GateUser, SMsgView_GM_Handshake>
{
protected:
	virtual void Handle(GateUser* pGate, SGameMsgHead* head, SMsgView_GM_Handshake* pMsg)
	{
		// ĿǰMng����Ҫ֪���Է��ı���ID
		pGate->SetMaxCount(pMsg->dwMaxCount);
		pGate->SetCurrCount(pMsg->dwCurrCount);
		pGate->SetPortForView(pMsg->wViewPort);
		pGate->SetPortForClient(pMsg->wClientPort);

		// �ظ�����
		{
			SMsgView_MG_HandshakeResponse sendData;
			sendData.dwID = pGate->GetID();
			pGate->SendMsg(sendData);
		}

		// ����Ƿ���Զ��������
		if (gSetting.CheckReady())
		{
			// ������Gate����Ϣ���͸�����View
			gViewList.BroadcastGatesInfoToViews();
		}
	}
};