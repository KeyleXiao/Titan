#include "stdafx.h"
#include "Replay.h"
#include "RedisMng.h"
#include "ViewRedisKey.h"
#include "LoadPrepInfoTask.h"
#include "LoadBattleInfoTask.h"
#include "RedisThread.h"
#include "ClientUser.h"
#include "ITimeSyncService.h"
#include "ClientList.h"


bool Replay::Start()
{
	m_strKey = ViewRedisKey::BuildReplayKey(m_rID);

	Redis::IRedisConnector* pRedis = gRedisMng.GetRedis();
	if (pRedis == nullptr)
		return false;

	m_prepKey = ViewRedisKey::BuildReplayPrepKey(m_rID);

	// ��������Redis�߳�
	m_battlePrepMsg.rID = m_rID;
	LoadPrepInfoTask* pTask = new LoadPrepInfoTask(*this, m_prepKey, m_battlePrepMsg, m_timeBegin, m_timeEnd);
	gRedisThread.spawn(pTask);

	// ��������
	m_pTask = new LoadBattleInfoTask(*this, m_strKey, m_prepKey, m_timeEnd);

	return true;
}

void Replay::HeartBeat()
{
	if (!m_bStart)
		return;

	Redis::IRedisConnector* pRedis = gRedisMng.GetRedis();
	if (pRedis == nullptr)
		return;

	// 1.����ս����Ϣ����ս����
	BroadCastBattleInfo();

	// 2.�������񣺶�ȡս��
	if (!m_bFinished && m_pTask && m_pTask->IsDone())
	{
		gRedisThread.spawn(m_pTask);
	}

	// ��ͨ��ttl��ѯkey�Ƿ����
	//redis > EXPIRE mykey 10 # ����mykey 10������
	//(integer) 1
	//	redis > TTL mykey # �鿴mykeyʣ��Ĺ���ʱ��
	//	(integer) 10
}

void Replay::Remove(PlayerID dwPlayerID)
{
	m_UserMap.erase(dwPlayerID);
}

void Replay::BroadCastBattleInfo()
{
	// �����㲥��map
	std::map<DWORD, VecPlayerID>	_map;

	// ��װmap
	for each (auto entry in m_UserMap)
	{
		const PlayerID& dwPlayerID = entry.first;
		ClientUser* pClientUser = gClientList.Get(dwPlayerID);
		if (pClientUser == nullptr)
			continue;	// ���������ʱɾ�������ﲻ��	TODO

		ViewNode& node = entry.second;
		if (!node.m_bRecv)
		{
			// ���͡�Ԥ����Ϣ��
			pClientUser->SendMsg(m_battlePrepMsg);
			node.m_bRecv = true;
		}

		STime alowTime = gTimeSyncService.GetTime();	// ��ǰʱ��
		alowTime.tSec -= node.m_sViewParam.dwDelaySec;	// С�����ʱ��Ķ����Բ���

		DWORD dwIndex = node.m_dwIndex;

		for (DWORD i = dwIndex; i < m_vecBattleInfo.size(); i++)
		{
			SMsgView_VC_BattleInfo& msg = m_vecBattleInfo.at(i);
			STime battleTime = this->m_timeBegin + msg.sBattleInfo.tTime;
			node.m_dwIndex = i;
			if (alowTime < battleTime)
				break;

			// �����IDѹ������Ͷ���
			_map[i].push_back(dwPlayerID);
		}
	}

	for each (auto entry in _map)
	{
		SMsgView_VC_BattleInfo& msg = m_vecBattleInfo.at(entry.first);
		const VecPlayerID& vec = entry.second;

		gClientList.BroadCastData(msg, vec);
	}
}

void Replay::Insert(ClientUser* pClientUser, const ViewParam& sViewParam)
{
	const PlayerID& dwPlayerID = pClientUser->m_dwPlayerID;
	m_UserMap[dwPlayerID].m_dwPlayerID = dwPlayerID;
	m_UserMap[dwPlayerID].m_sViewParam = sViewParam;
}
