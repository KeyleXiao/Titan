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
	// 1.0 ����
	PlayerID dwPlayerID = pMsg->reqMsg.dwPlayerID;

	// 1.1 ����ViewServer����״��
	ViewSession* pView = gViewContainer.Get(pMsg->wViewID);
	if (pView==nullptr)
		return E_RESULT_RAP_ERR_NOT_REACH_VIEW;

	// 1.2 ���������Ƿ��Ѿ���������
	ClientSession* pClientSession = gClientContainer.Get(pMsg->reqMsg.dwPlayerID);
	if (pClientSession)
		return E_RESULT_RAP_ERR_PLAYER_EXIST;

	// 2.0 �����µĵ�½��֤���(���֮ǰ�����ݣ��͸���֮)
	m_MapAuth[dwPlayerID].m_info = *pMsg;
	m_MapAuth[dwPlayerID].m_tExpiry = time(NULL) + m_MapAuth[dwPlayerID].m_info.dwTimeOut;

	// 3.0 Ϊ����ָ�븳ֵ
	pAuthNode = &m_MapAuth[dwPlayerID];

	return E_RESULT_RAP_SUCCESS;
}

void AuthMng::Init(TimerAxis* pTimerAxis)
{
	m_pTimerAxis = pTimerAxis;

	// ������ʱ��
	m_pTimerAxis->SetTimer(ETimerEventID_CheckTimeout, CHECK_TIME_OUT_INTERVAL, this);
}

void AuthMng::Shutdown()
{
	// ɾ����ʱ��
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
		// ֪ͨMng
		SMsgView_GM_AuthDel msg;
		msg.dwPlayerID = dwPlayerID;
		msg.eReason = eReason;
		m_pMng->SendMsg(msg);

		// ɾ���ýڵ�
		m_MapAuth.erase(dwPlayerID);
	}
}
