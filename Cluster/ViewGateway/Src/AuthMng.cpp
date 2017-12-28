#include "stdafx.h"
#include "AuthMng.h"
#include "ViewMsgDef_Server.h"
#include "ViewContainer.h"
#include "ClientContainer.h"
#include "MsgFactory.h"
#include "MngConnector.h"

#define CHECK_TIME_OUT_INTERVAL	2*1000

AuthMng& gAuthMng = AuthMng::getInstance();

EnumResultRequestAddPlayer AuthMng::Insert(SMsgView_MG_RequestAddPlayer* pMsg, AuthNode*& pAuthNode)
{
	// 1.0 检验
	PlayerID dwPlayerID = pMsg->reqMsg.dwPlayerID;

	// 1.1 检验ViewServer连接状况
	ViewSession* pView = gViewContainer.Get(pMsg->wViewID);
	if (pView==nullptr)
		return E_RESULT_RAP_ERR_NOT_REACH_VIEW;

	// 1.2 检验该玩家是否已经连上来了
	ClientSession* pClientSession = gClientContainer.Get(pMsg->reqMsg.dwPlayerID);
	if (pClientSession)
		return E_RESULT_RAP_ERR_PLAYER_EXIST;

	// 2.0 生成新的登陆验证结点(如果之前有数据，就覆盖之)
	m_MapAuth[dwPlayerID].m_info = *pMsg;
	m_MapAuth[dwPlayerID].m_tExpiry = time(NULL) + m_MapAuth[dwPlayerID].m_info.dwTimeOut;

	// 3.0 为传出指针赋值
	pAuthNode = &m_MapAuth[dwPlayerID];

	return E_RESULT_RAP_SUCCESS;
}

void AuthMng::Init(TimerAxis* pTimerAxis)
{
	m_pTimerAxis = pTimerAxis;

	// 启动定时器
	m_pTimerAxis->SetTimer(ETimerEventID_CheckTimeout, CHECK_TIME_OUT_INTERVAL, this);
}

void AuthMng::Shutdown()
{
	// 删除定时器
	m_pTimerAxis->KillTimer(ETimerEventID_CheckTimeout, this);
}

void AuthMng::OnTimer(unsigned long dwTimerID)
{
	switch (dwTimerID)
	{
	case ETimerEventID_CheckTimeout:
	{
		if (m_MapAuth.empty())
			break;

		std::vector<PlayerID> vecTmp;
		time_t now = time(NULL);
		for each (auto entry in m_MapAuth)
		{
			AuthNode& node = entry.second;
			if (node.IsExpiry(now))
			{
				vecTmp.push_back(entry.first);
			}
		}

		for (size_t i = 0; i < vecTmp.size(); i++)
		{
			const PlayerID& dwPlayerID = vecTmp.at(i);
			Remove(dwPlayerID, E_AuthDelR_TIME_OUT);
		}
		vecTmp.clear();
	}
		break;
	default:
		break;
	}
}

void AuthMng::SetMng(MngConnector* pSession)
{
	m_pMng = pSession;
}

AuthNode* AuthMng::Find(const PlayerID& dwPlayerID)
{
	auto it = m_MapAuth.find(dwPlayerID);
	if (it!= m_MapAuth.end())
		return &it->second;
	else
		return nullptr;
}

void AuthMng::Remove(const PlayerID& dwPlayerID, EnumAuthDelReason eReason)
{
	auto it = m_MapAuth.find(dwPlayerID);
	if (it != m_MapAuth.end())
	{
		// 通知Mng
		SMsgView_GM_AuthDel msg;
		msg.dwPlayerID = dwPlayerID;
		msg.eReason = eReason;
		m_pMng->SendMsg(msg);

		// 删除该节点
		m_MapAuth.erase(dwPlayerID);
	}
}
