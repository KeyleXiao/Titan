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
		const SGameMsgHead& header = gMsg.BuildHead_MV(ENUM_MSG_VIEW_HANDSHAKE_RESPONSE);
		SMsgView_MV_HandshakeResponse sendData;
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