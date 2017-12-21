#pragma once
#include "Handler.h"
#include "GateUser.h"
#include "ViewMngSetting.h"
#include "ViewList.h"
#include "ViewMsgDef_Server.h"


class GM_HandshakeHandler : public Handler<GateUser, SMsgView_GM_Handshake>
{
protected:
	virtual void Handle(GateUser* pSession, SGameMsgHead* head, SMsgView_GM_Handshake* pMsg)
	{
		// ĿǰMng����Ҫ֪���Է��ı���ID
		pSession->SetMaxCount(pMsg->dwMaxCount);
		pSession->SetCurrCount(pMsg->dwCurrCount);
		pSession->SetPortForView(pMsg->wPort);

		// �ظ�����
		{
			const SGameMsgHead& header = gMsg.BuildHead_MG(ENUM_MSG_VIEW_HANDSHAKE_RESPONSE);
			SMsgView_MG_HandshakeResponse sendData;
			sendData.dwID = pSession->GetID();
			TSendMsg(pSession, header, sendData);
		}

		// ����Ƿ���Զ��������
		if (gSetting.CheckReady())
		{
			// ������Gate����Ϣ���͸�����View
			gViewList.BroadcastGatesInfoToView();
		}
	}
};