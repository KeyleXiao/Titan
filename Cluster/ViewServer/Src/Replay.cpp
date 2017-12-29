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

	// 发送任务到Redis线程
	m_battlePrepMsg.rID = m_rID;
	LoadPrepInfoTask* pTask = new LoadPrepInfoTask(*this, m_prepKey, m_battlePrepMsg, m_timeBegin, m_timeEnd);
	gRedisThread.spawn(pTask);

	// 心跳任务
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

	// 1.发送战报信息给观战者们
	BroadCastBattleInfo();

	// 2.发布任务：读取战报
	if (!m_bFinished && m_pTask && m_pTask->IsDone())
	{
		gRedisThread.spawn(m_pTask);
	}

	// 先通过ttl查询key是否过期
	//redis > EXPIRE mykey 10 # 设置mykey 10秒后过期
	//(integer) 1
	//	redis > TTL mykey # 查看mykey剩余的过期时间
	//	(integer) 10
}

void Replay::Remove(PlayerID dwPlayerID)
{
	m_UserMap.erase(dwPlayerID);
}

void Replay::BroadCastBattleInfo()
{
	// 用来广播的map
	std::map<DWORD, VecPlayerID>	_map;

	// 组装map
	for each (auto entry in m_UserMap)
	{
		const PlayerID& dwPlayerID = entry.first;
		ClientUser* pClientUser = gClientList.Get(dwPlayerID);
		if (pClientUser == nullptr)
			continue;	// 由玩家下线时删除，这里不管	TODO

		ViewNode& node = entry.second;
		if (!node.m_bRecv)
		{
			// 发送“预备信息”
			pClientUser->SendMsg(m_battlePrepMsg);
			node.m_bRecv = true;
		}

		STime alowTime = gTimeSyncService.GetTime();	// 当前时间
		alowTime.tSec -= node.m_sViewParam.dwDelaySec;	// 小于这个时间的都可以播放

		DWORD dwIndex = node.m_dwIndex;

		for (DWORD i = dwIndex; i < m_vecBattleInfo.size(); i++)
		{
			SMsgView_VC_BattleInfo& msg = m_vecBattleInfo.at(i);
			STime battleTime = this->m_timeBegin + msg.sBattleInfo.tTime;
			node.m_dwIndex = i;
			if (alowTime < battleTime)
				break;

			// 把玩家ID压入待发送队列
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
