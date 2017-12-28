#include "stdafx.h"
#include "LoadPrepInfoTask.h"
#include "RedisMng.h"
#include "Replay.h"
#include "RedisThread.h"
#include "ViewMsgDef_Client.h"
#include "STime.h"


void LoadPrepInfoTask::Run()
{
	Redis::IRedisConnector* pRedis = gRedisMng.GetRedis();
	if (pRedis == nullptr)
		return;

	// 预备信息只读一次，以免内存泄漏
	if (!m_bReadedPrep)
		m_bReadedPrep = pRedis->listOp()->lIndex(m_prepKey, indexBattlePrepInfo, m_battlePrepInfo.sPrepInfo);
	
	pRedis->listOp()->lIndex(m_prepKey, indexBeginTime, m_timeBegin);
	pRedis->listOp()->lIndex(m_prepKey, indexEndTime, m_timeEnd);
}

void LoadPrepInfoTask::Callback(Replay& tHolder)
{
	if (m_timeBegin.IsValid())
		tHolder.m_bStart = true;
}

bool LoadPrepInfoTask::NeedRecovery()
{
	return m_tHolder.m_bStart;
}

bool LoadPrepInfoTask::NeedSpawnAgain()
{
	return !m_tHolder.m_bStart;
}