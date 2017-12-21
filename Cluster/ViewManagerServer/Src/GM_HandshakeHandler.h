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
		// 目前Mng不需要知道对方的本地ID
		pSession->SetMaxCount(pMsg->dwMaxCount);
		pSession->SetCurrCount(pMsg->dwCurrCount);
		pSession->SetPortForView(pMsg->wPort);

		// 回复握手
		{
			const SGameMsgHead& header = gMsg.BuildHead_MG(ENUM_MSG_VIEW_HANDSHAKE_RESPONSE);
			SMsgView_MG_HandshakeResponse sendData;
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
};