#pragma once
#include "Handler.h"
#include "ViewMsgDef_Server.h"
#include "AuthMng.h"
#include "GameList.h"
#include "GameUser.h"
#include "GateList.h"

class GateUser;

class GM_RequestAddPlayerReplyHandler : public Handler<GateUser, SMsgView_GM_RequestAddPlayerReply>
{
protected:
	virtual void Handle(GateUser* pGate, SGameMsgHead* head, SMsgView_GM_RequestAddPlayerReply* pMsg)
	{
		const PlayerID& dwPlayerID = pMsg->dwPlayerID;
		AuthNode* pAuthNode = gAuthMng.Get(dwPlayerID);
		if (pAuthNode==nullptr)
		{
			// ��ô֪ͨ��Gate���������¼��Ϣ
			SMsgView_MG_AuthDel msg;
			msg.dwPlayerID = dwPlayerID;
			msg.eReason = E_AuthDelR_USER_CANCEL;
			pGate->SendMsg(msg);
			return;
		}

		// ֮ǰ���������ServerID
		const GameID& reqServerID = pAuthNode->m_reqServerID;
		GameUser* pServer = gGameList.Get(reqServerID);
		if (pServer==nullptr)
		{
			// ������������(Ӧ����ɾ��GameUser�ĵط��������) TODO
			return;
		}

		switch (pMsg->eResult)
		{
		// �ɹ�
		case E_RESULT_RAP_SUCCESS:
		{
			// ֪ͨ��Ϸ������Client����Key��ΨһID��ȥ��ָ����Gate
			SMsgView_MS_RequestReplay_Success msg;
			msg.reqMsg = pAuthNode->m_info.reqMsg;
			msg.strIp = pGate->GetRemoteIP();
			msg.wPort = pGate->GetPortForClient();
			pAuthNode->GetKeyData(msg.szKeyData);
			
			pServer->SendMsg(msg);
		}
		break;
		
		// ��ID����Ѵ����ڱ�Gate�����ѵ�½
		case E_RESULT_RAP_ERR_PLAYER_EXIST:
		{
			// �����������ϲ����ܳ���
			ErrorLn(_GT("����Ѵ����ڸ�Gate�����ѵ�½.GateID=") << pGate->GetID() << _GT(",���ID=") << dwPlayerID);
			return;
		}
		break;
		// ��ViewServer�����Ӳ���(��ʹ��������Ҳ�п��ܳ���ĳ��̨����֮�����Ӳ��ϵ����)
		case E_RESULT_RAP_ERR_NOT_REACH_VIEW:
		{
			// 2.��һ�����ص͵�Gate
			GateID nGateID = gGateList.GetIdleGate(pAuthNode->m_wGateID);
			if (nGateID == 0)
			{
				SMsgView_MS_RequestReplay_Fail msg;
				msg.eResult = E_RESULT_RR_ERR_GATE_IS_FULL;
				msg.reqMsg = pAuthNode->m_info.reqMsg;

				pServer->SendMsg(msg);
			}
			else
			{
				// 3.��һ��������͵�Gate˵��IDΪXX����ҽ�Ҫ����ĳ�ֱ�������ĳ̨View�ϣ�����Ӵ���������Կ...����ʱʱ��ΪT
				SMsgView_MG_RequestAddPlayer msg;
				pAuthNode->GetKeyData(msg.szKeyData);
				msg.reqMsg = pAuthNode->m_info.reqMsg;
				msg.wViewID = pAuthNode->m_info.wViewID;
				msg.dwTimeOut = VIEW_SECRET_KEY_TIME_OUT;

				pServer->SendMsg(msg);
			}
		}
		break;

		default:
			break;
		}
	}
};