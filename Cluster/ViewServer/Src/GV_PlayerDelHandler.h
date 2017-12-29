#pragma once
#include "Handler.h"
#include "ViewMsgDef_Server.h"
#include "GateConnector.h"


class GV_PlayerDelHandler : public Handler<GateConnector, SMsgView_GV_PlayerDel>
{
protected:
	virtual void Handle(GateConnector* pGateConnector, SGameMsgHead* head, SMsgView_GV_PlayerDel* pMsg)
	{
		const PlayerID& dwPlayerID = pMsg->dwPlayerID;
		ClientUser* pClientUser = gClientList.Get(dwPlayerID);
		if (pClientUser == nullptr)
			return;

		// ֹͣ�ۿ�
		if (pClientUser->IsViewing())
			pClientUser->StopView();
		
		// ����
		gClientList.Del(dwPlayerID);
	}
};