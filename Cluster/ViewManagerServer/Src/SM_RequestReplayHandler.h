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
		// 0.0 数据准备
		const ReplayID&	rID = pMsg->rID;				// 战报ID
		const PlayerID&	dwPlayerID = pMsg->dwPlayerID;	// 标识观战客户端的唯一ID

		// 0.1 检查该玩家是否之前请求登录过
		AuthNode* pAuthNode = gAuthMng.Get(dwPlayerID);
		if (pAuthNode!=nullptr)
		{
			GateUser* pGate = gGateList.Get(pAuthNode->m_wGateID);
			if (pGate)
			{
				// 之前请求过“观看战报”，但一直没有登录，且没有过超时时间（过了的话，Gate会告诉我删除的）
				if (pAuthNode->m_info.reqMsg.rID == rID)
				{
					// 游戏服又请求同样ID的战报
					if (pServer->GetID() == pAuthNode->m_reqServerID)
					{
						// 同一个服申请的.告诉他，不要重复申请
						NotifyFail(E_RESULT_RR_ERR_REQUEST_REPEATED, pMsg, pServer);
						return;
					}
					else
					{
						// 不同服申请的（只能推测为玩家之前申请完，掉线了，然后又登陆的别的服）
						// 那么把验证消息重新给此游戏服发一遍
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
					// 游戏服请求新的战报
					// 那么通知该Gate，撤销其登录信息
					SMsgView_MG_AuthDel msg;
					msg.dwPlayerID = dwPlayerID;
					msg.eReason = E_AuthDelR_USER_CANCEL;
					pGate->SendMsg(msg);
					return;
				}
			}
			else
			{
				// 糟糕，那个Gate此时连不上了
				// 删除此节点
				gAuthMng.Remove(dwPlayerID);
				// 下面会为其分配新的Gate
			}
		}


		// 0.2 检查该玩家是否已经在观看某场战报了	TODO


		// 1.查询本地缓存，得知哪个View正在看此局（如果超过指定人数，就找个负载低的）；没有的话，就找一个负载低的
		ReplayNode* replayNode = nullptr;
		auto result = gReplayMng.Find(rID, replayNode);// 如果节点是新生成的，会通知View
		if (result != E_RESULT_RR_SUCCESS)
		{
			NotifyFail(result, pMsg, pServer);// 返回失败原因给Game服
			return;
		}

		// 2.找一个负载低的Gate
		GateID nGateID = gGateList.GetIdleGate();
		if (nGateID==0)
		{
			NotifyFail(E_RESULT_RR_ERR_GATE_IS_FULL, pMsg, pServer);// 返回失败原因给Game服
			return;
		}

		// 3.对一个负载最低的Gate说：ID为XX的玩家将要来看某局比赛，在某台View上，请你接待，这是秘钥...。超时时间为T
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
	// Qualifier: 通知游戏服，请求失败
	// Parameter: EnumResultRequestReplay result	失败原因
	// Parameter: SMsgView_SM_RequestReplay * pMsg	你发来的请求消息
	// Parameter: GameUser * pServer				发来请求的游戏服
	//************************************
	void NotifyFail(EnumResultRequestReplay result, SMsgView_SM_RequestReplay* pMsg, GameUser* pServer)
	{
		SMsgView_MS_RequestReplay_Fail msg;
		msg.eResult = result;
		msg.reqMsg = *pMsg;

		pServer->SendMsg(msg);
	}

	// 生成密钥
	void CreateKeyData(BYTE* szKeyData)
	{
		for (int i = 0; i < VIEW_SECRET_KEY_SIZE; ++i)
		{
			szKeyData[i] = rand() % ('z' - 'A') + 'A';
		}
	}

};
