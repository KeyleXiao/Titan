#pragma once
#include "Handler.h"
#include "ViewMsgDef_Server.h"
#include "GateConnector.h"
#include "ReplayContainer.h"
#include "Replay.h"
#include "ClientList.h"


class GV_CRequestReplayHandler : public Handler<GateConnector, SMsgView_GV_CRequestReplay>
{
protected:
	virtual void Handle(GateConnector* pGateConnector, SGameMsgHead* head, SMsgView_GV_CRequestReplay* pMsg)
	{
		const ReplayID& rID = pMsg->reqMsg.rID;		// 战报ID
		const PlayerID& dwPlayerID = pMsg->reqMsg.dwPlayerID;	// 标识观战客户端的唯一ID
		const ViewParam& sViewParam = pMsg->reqMsg.sViewParam;	// 权限ID
		Replay* pReplay = gReplays.Get(rID);
		if (pReplay==nullptr)
		{
			ErrorLn(_GT("玩家ID【") << dwPlayerID << _GT("】申请观看战报【") << rID << _GT("】，但是 战报节点却未创建!"));
			return;
		}

		ClientUser* pClientUser = gClientList.InsertIfAbsent(dwPlayerID, pGateConnector);
		if (pClientUser->IsViewing())
		{
			// 停止对其播放的旧战报
			pClientUser->StopView();
		}

		pReplay->Insert(pClientUser, sViewParam);	// 在战报播放组中加入此玩家
		pClientUser->m_rID = pReplay->m_rID;		// 在玩家身上记录此战报ID
	}
};