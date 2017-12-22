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
			// 那么通知该Gate，撤销其登录信息
			SMsgView_MG_AuthDel msg;
			msg.dwPlayerID = dwPlayerID;
			msg.eReason = E_AuthDelR_USER_CANCEL;
			pGate->SendMsg(msg);
			return;
		}

		// 之前发来请求的ServerID
		const GameID& reqServerID = pAuthNode->m_reqServerID;
		GameUser* pServer = gGameList.Get(reqServerID);
		if (pServer==nullptr)
		{
			// 清空其各种申请(应该在删除GameUser的地方做这个事) TODO
			return;
		}

		switch (pMsg->eResult)
		{
		// 成功
		case E_RESULT_RAP_SUCCESS:
		{
			// 通知游戏服，让Client拿着Key和唯一ID，去连指定的Gate
			SMsgView_MS_RequestReplay_Success msg;
			msg.reqMsg = pAuthNode->m_info.reqMsg;
			msg.strIp = pGate->GetRemoteIP();
			msg.wPort = pGate->GetPortForClient();
			pAuthNode->GetKeyData(msg.szKeyData);
			
			pServer->SendMsg(msg);
		}
		break;
		
		// 该ID玩家已存在于本Gate，且已登陆
		case E_RESULT_RAP_ERR_PLAYER_EXIST:
		{
			// 这个情况理论上不可能出现
			ErrorLn(_GT("玩家已存在于该Gate，且已登陆.GateID=") << pGate->GetID() << _GT(",玩家ID=") << dwPlayerID);
			return;
		}
		break;
		// 该ViewServer我连接不上(即使是内网，也有可能出现某几台机器之间连接不上的情况)
		case E_RESULT_RAP_ERR_NOT_REACH_VIEW:
		{
			// 2.找一个负载低的Gate
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
				// 3.对一个负载最低的Gate说：ID为XX的玩家将要来看某局比赛，在某台View上，请你接待，这是秘钥...。超时时间为T
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