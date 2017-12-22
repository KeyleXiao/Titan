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
		// 目前Mng不需要知道对方的本地ID
		pGate->SetMaxCount(pMsg->dwMaxCount);
		pGate->SetCurrCount(pMsg->dwCurrCount);
		pGate->SetPortForView(pMsg->wViewPort);
		pGate->SetPortForClient(pMsg->wClientPort);

		// 回复握手
		{
			SMsgView_MG_HandshakeResponse sendData;
			sendData.dwID = pGate->GetID();
			pGate->SendMsg(sendData);
		}

		// 检测是否可以对外服务了
		if (gSetting.CheckReady())
		{
			// 将所有Gate的信息推送给所有View
			gViewList.BroadcastGatesInfoToViews();
		}
	}
};