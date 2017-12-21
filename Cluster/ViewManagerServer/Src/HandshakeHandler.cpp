#include "stdafx.h"
#include "HandshakeHandler.h"
#include "ViewUser.h"
#include "ViewMngSetting.h"
#include "GateList.h"
#include "ViewList.h"


void HandshakeHandler::Handle(ViewUser* pSession, SGameMsgHead* head, SMsgView_VM_Handshake* pMsg)
{
	// 目前Mng不需要知道对方的本地ID
	pSession->SetMaxCount(pMsg->dwMaxCount);
	pSession->SetCurrCount(pMsg->dwCurrCount);

	// 回复握手
	{
		const SGameMsgHead& header = gMsg.BuildHead_MV(ENUM_MSG_VIEW_HANDSHAKE_RESPONSE);
		SMsgView_MV_HandshakeResponse sendData;
		sendData.dwID = pSession->GetID();
		TSendMsg(pSession, header, sendData);
	}

	// 检测是否可以对外服务了
	if (gSetting.CheckReady())
	{
		// 将所有Gate的信息推送给所有View
		gViewList.BroadcastGatesInfoToView();
	}
}