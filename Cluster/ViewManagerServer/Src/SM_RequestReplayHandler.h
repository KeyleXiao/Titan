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
		// 1.查询本地缓存，得知哪个View正在看此局（如果超过指定人数，就找个负载低的）；没有的话，就找一个负载低的
		ReplayNode* replayNode = nullptr;
		auto result = gReplayMng.Find(pMsg->rID, replayNode);
		if (result != E_RESULT_RR_SUCCESS)
		{
			// 返回失败原因给Game服
			SMsgView_SM_RequestReplay_Fail msg;
			msg.eResult = result;
			msg.reqMsg = *pMsg;

			const SGameMsgHead& header = gMsg.BuildHead_MS(ENUM_MSG_VIEW_REQUEST_REPLAY_FAIL);
			TSendMsg(pSession, header, msg);

			return;
		}

		// 2.找一个负载低的Gate
		GateID nGateID = gGateList.GetIdleGate();
		if (nGateID==0)
		{
			// 返回失败原因给Game服
			SMsgView_SM_RequestReplay_Fail msg;
			msg.eResult = E_RESULT_RR_ERR_GATE_IS_FULL;
			msg.reqMsg = *pMsg;

			const SGameMsgHead& header = gMsg.BuildHead_MS(ENUM_MSG_VIEW_REQUEST_REPLAY_FAIL);
			TSendMsg(pSession, header, msg);

			return;
		}

		// 3.对一个负载最低的Gate说：ID为XX的玩家将要来看某局比赛，在某台View上，请你接待，这是秘钥...。超时时间为T
		SMsgView_MG_RequestAddPlayer msg;
		CreateKeyData(msg.szKeyData);
		msg.reqMsg = *pMsg;
		msg.wViewID = replayNode->GetViewID();
		msg.dwTimeOut = VIEW_SECRET_KEY_TIME_OUT;

		const SGameMsgHead& header = gMsg.BuildHead_MG(ENUM_MSG_VIEW_REQUEST_ADD_PLAYER);
		TSendMsg(pSession, header, msg);
	}

private:
	// 生成密钥
	void CreateKeyData(BYTE* szKeyData)
	{
		for (int i = 0; i < VIEW_SECRET_KEY_SIZE; ++i)
		{
			szKeyData[i] = rand() % ('z' - 'A') + 'A';
		}
	}

};
