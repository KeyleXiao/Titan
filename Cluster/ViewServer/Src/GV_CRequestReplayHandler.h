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
		const ReplayID& rID = pMsg->reqMsg.rID;		// ս��ID
		const PlayerID& dwPlayerID = pMsg->reqMsg.dwPlayerID;	// ��ʶ��ս�ͻ��˵�ΨһID
		const ViewParam& sViewParam = pMsg->reqMsg.sViewParam;	// Ȩ��ID
		Replay* pReplay = gReplays.Get(rID);
		if (pReplay==nullptr)
		{
			ErrorLn(_GT("���ID��") << dwPlayerID << _GT("������ۿ�ս����") << rID << _GT("�������� ս���ڵ�ȴδ����!"));
			return;
		}

		ClientUser* pClientUser = gClientList.InsertIfAbsent(dwPlayerID, pGateConnector);
		if (pClientUser->IsViewing())
		{
			// ֹͣ���䲥�ŵľ�ս��
			pClientUser->StopView();
		}

		pReplay->Insert(pClientUser, sViewParam);	// ��ս���������м�������
		pClientUser->m_rID = pReplay->m_rID;		// ��������ϼ�¼��ս��ID
	}
};