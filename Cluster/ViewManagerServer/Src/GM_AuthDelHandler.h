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
		// ���ֵ�½�ɹ�
		case E_AuthDelR_HANDSHAKE:
			break;
		// ��ʱ��
		case E_AuthDelR_TIME_OUT:
			break;
		// ��ViewServer�����Ӳ����ˣ���Ȼ֮ǰ�����ϣ�
		case E_AuthDelR_NOT_REACH_VIEW:
			break;

		default:
			break;
		}
		
	}
};