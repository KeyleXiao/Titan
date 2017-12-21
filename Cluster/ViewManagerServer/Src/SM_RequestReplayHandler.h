#pragma once
#include "Handler.h"
#include "GameUser.h"
#include "ReplayMng.h"
#include "GateList.h"
#include "ViewMsgDef_Server.h"


class SM_RequestReplayHandler : public Handler<GameUser, SMsgView_SM_RequestReplay>
{
protected:
	virtual void Handle(GameUser* pSession, SGameMsgHead* head, SMsgView_SM_RequestReplay* pMsg)
	{
		// 1.��ѯ���ػ��棬��֪�ĸ�View���ڿ��˾֣��������ָ�����������Ҹ����ص͵ģ���û�еĻ�������һ�����ص͵�
		ReplayNode* replayNode = nullptr;
		auto result = gReplayMng.Find(pMsg->rID, replayNode);
		if (result != E_RESULT_RR_SUCCESS)
		{
			// ����ʧ��ԭ���Game��
			SMsgView_SM_RequestReplay_Fail msg;
			msg.eResult = result;
			msg.reqMsg = *pMsg;

			const SGameMsgHead& header = gMsg.BuildHead_MS(ENUM_MSG_VIEW_REQUEST_REPLAY_FAIL);
			TSendMsg(pSession, header, msg);

			return;
		}

		// 2.��һ�����ص͵�Gate
		GateID nGateID = gGateList.GetIdleGate();
		if (nGateID==0)
		{
			// ����ʧ��ԭ���Game��
			SMsgView_SM_RequestReplay_Fail msg;
			msg.eResult = E_RESULT_RR_ERR_GATE_IS_FULL;
			msg.reqMsg = *pMsg;

			const SGameMsgHead& header = gMsg.BuildHead_MS(ENUM_MSG_VIEW_REQUEST_REPLAY_FAIL);
			TSendMsg(pSession, header, msg);

			return;
		}

		// 3.��һ��������͵�Gate˵��IDΪXX����ҽ�Ҫ����ĳ�ֱ�������ĳ̨View�ϣ�����Ӵ���������Կ...����ʱʱ��ΪT
		SMsgView_MG_RequestAddPlayer msg;
		CreateKeyData(msg.szKeyData);
		msg.reqMsg = *pMsg;
		msg.wViewID = replayNode->GetViewID();
		msg.dwTimeOut = VIEW_SECRET_KEY_TIME_OUT;

		const SGameMsgHead& header = gMsg.BuildHead_MG(ENUM_MSG_VIEW_REQUEST_ADD_PLAYER);
		TSendMsg(pSession, header, msg);
	}

private:
	// ������Կ
	void CreateKeyData(BYTE* szKeyData)
	{
		for (int i = 0; i < VIEW_SECRET_KEY_SIZE; ++i)
		{
			szKeyData[i] = rand() % ('z' - 'A') + 'A';
		}
	}

};
