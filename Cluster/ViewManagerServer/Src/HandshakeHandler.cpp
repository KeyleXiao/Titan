#include "stdafx.h"
#include "HandshakeHandler.h"
#include "ViewUser.h"
#include "ViewMngSetting.h"
#include "GateList.h"
#include "ViewList.h"


void HandshakeHandler::Handle(ViewUser* pSession, SGameMsgHead* head, SMsgView_VM_Handshake* pMsg)
{
	// ĿǰMng����Ҫ֪���Է��ı���ID
	pSession->SetMaxCount(pMsg->dwMaxCount);
	pSession->SetCurrCount(pMsg->dwCurrCount);

	// �ظ�����
	{
		SMsgView_MV_HandshakeResponse msg;
		msg.dwID = pSession->GetID();
		pSession->SendMsg(msg);
	}

	// ����Ƿ���Զ��������
	if (gSetting.CheckReady())
	{
		// ������Gate����Ϣ���͸�����View
		gViewList.BroadcastGatesInfoToViews();
	}
}