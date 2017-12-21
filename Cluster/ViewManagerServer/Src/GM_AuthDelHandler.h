#pragma once
#include "Handler.h"
#include "ViewMsgDef_Server.h"

class GateUser;

class GM_AuthDelHandler : public Handler<GateUser, SMsgView_GM_AuthDel>
{
protected:
	virtual void Handle(GateUser* pSession, SGameMsgHead* head, SMsgView_GM_AuthDel* pMsg)
	{
		// TODO
		const PlayerID& dwPlayerID = pMsg->dwPlayerID;
		switch (pMsg->eReason)
		{
		// 握手登陆成功
		case E_AuthDelR_HANDSHAKE:
			break;
		// 超时了
		case E_AuthDelR_TIME_OUT:
			break;
		// 该ViewServer我连接不上了（虽然之前连得上）
		case E_AuthDelR_NOT_REACH_VIEW:
			break;

		default:
			break;
		}
		
	}
};