#pragma once
#include "Handler.h"
#include "GameUser.h"
#include "ReplayMng.h"
#include "GateList.h"
#include "ViewMsgDef_Server.h"
#include "AuthMng.h"
#include "GateUser.h"


class SM_RequestReplayHandler : public Handler<GameUser, SMsgView_SM_RequestReplay>
{
protected:
	virtual void Handle(GameUser* pServer, SGameMsgHead* head, SMsgView_SM_RequestReplay* pMsg)
	{
		// 0.0 ����׼��
		const ReplayID&	rID = pMsg->rID;				// ս��ID
		const PlayerID&	dwPlayerID = pMsg->dwPlayerID;	// ��ʶ��ս�ͻ��˵�ΨһID

		// 0.1 ��������Ƿ�֮ǰ�����¼��
		AuthNode* pAuthNode = gAuthMng.Get(dwPlayerID);
		if (pAuthNode!=nullptr)
		{
			GateUser* pGate = gGateList.Get(pAuthNode->m_wGateID);
			if (pGate)
			{
				// ֮ǰ��������ۿ�ս��������һֱû�е�¼����û�й���ʱʱ�䣨���˵Ļ���Gate�������ɾ���ģ�
				if (pAuthNode->m_info.reqMsg.rID == rID)
				{
					// ��Ϸ��������ͬ��ID��ս��
					if (pServer->GetID() == pAuthNode->m_reqServerID)
					{
						// ͬһ���������.����������Ҫ�ظ�����
						NotifyFail(E_RESULT_RR_ERR_REQUEST_REPEATED, pMsg, pServer);
						return;
					}
					else
					{
						// ��ͬ������ģ�ֻ���Ʋ�Ϊ���֮ǰ�����꣬�����ˣ�Ȼ���ֵ�½�ı�ķ���
						// ��ô����֤��Ϣ���¸�����Ϸ����һ��
						SMsgView_MS_RequestReplay_Success msg;
						msg.reqMsg = *pMsg;
						pAuthNode->GetKeyData(msg.szKeyData);

						msg.strIp = pGate->GetRemoteIP();
						msg.wPort = pGate->GetPortForClient();
						pServer->SendMsg(msg);
						return;
					}
				}
				else
				{
					// ��Ϸ�������µ�ս��
					// ��ô֪ͨ��Gate���������¼��Ϣ
					SMsgView_MG_AuthDel msg;
					msg.dwPlayerID = dwPlayerID;
					msg.eReason = E_AuthDelR_USER_CANCEL;
					pGate->SendMsg(msg);
					return;
				}
			}
			else
			{
				// ��⣬�Ǹ�Gate��ʱ��������
				// ɾ���˽ڵ�
				gAuthMng.Remove(dwPlayerID);
				// �����Ϊ������µ�Gate
			}
		}


		// 0.2 ��������Ƿ��Ѿ��ڹۿ�ĳ��ս����	TODO


		// 1.��ѯ���ػ��棬��֪�ĸ�View���ڿ��˾֣��������ָ�����������Ҹ����ص͵ģ���û�еĻ�������һ�����ص͵�
		ReplayNode* replayNode = nullptr;
		auto result = gReplayMng.Find(rID, replayNode);// ����ڵ��������ɵģ���֪ͨView
		if (result != E_RESULT_RR_SUCCESS)
		{
			NotifyFail(result, pMsg, pServer);// ����ʧ��ԭ���Game��
			return;
		}

		// 2.��һ�����ص͵�Gate
		GateID nGateID = gGateList.GetIdleGate();
		if (nGateID==0)
		{
			NotifyFail(E_RESULT_RR_ERR_GATE_IS_FULL, pMsg, pServer);// ����ʧ��ԭ���Game��
			return;
		}

		// 3.��һ��������͵�Gate˵��IDΪXX����ҽ�Ҫ����ĳ�ֱ�������ĳ̨View�ϣ�����Ӵ���������Կ...����ʱʱ��ΪT
		SMsgView_MG_RequestAddPlayer msg;
		CreateKeyData(msg.szKeyData);
		msg.reqMsg = *pMsg;
		msg.wViewID = replayNode->GetViewID();
		msg.dwTimeOut = VIEW_SECRET_KEY_TIME_OUT;

		pServer->SendMsg(msg);
	}


private:
	//************************************
	// Returns:   void
	// Qualifier: ֪ͨ��Ϸ��������ʧ��
	// Parameter: EnumResultRequestReplay result	ʧ��ԭ��
	// Parameter: SMsgView_SM_RequestReplay * pMsg	�㷢����������Ϣ
	// Parameter: GameUser * pServer				�����������Ϸ��
	//************************************
	void NotifyFail(EnumResultRequestReplay result, SMsgView_SM_RequestReplay* pMsg, GameUser* pServer)
	{
		SMsgView_MS_RequestReplay_Fail msg;
		msg.eResult = result;
		msg.reqMsg = *pMsg;

		pServer->SendMsg(msg);
	}

	// ������Կ
	void CreateKeyData(BYTE* szKeyData)
	{
		for (int i = 0; i < VIEW_SECRET_KEY_SIZE; ++i)
		{
			szKeyData[i] = rand() % ('z' - 'A') + 'A';
		}
	}

};
