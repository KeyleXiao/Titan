#include "stdafx.h"
#include "LoadBattleInfoTask.h"
#include "RedisMng.h"
#include "Replay.h"
#include "ViewMsgDef_Client.h"
#include "STime.h"


void LoadBattleInfoTask::Run()
{
	Redis::IRedisConnector* pRedis = gRedisMng.GetRedis();
	if (pRedis == nullptr)
		return;

	pRedis->listOp()->lIndex(m_prepKey, ViewRedisData::indexEndTime, m_timeEnd);

	// 读取总长度
	lLength = 0;
	D_IF_FALSE_RETURN(pRedis->listOp()->lLen(strKey, &lLength));

	if (m_tHolder.m_lReaded == lLength)
		return;

	// 战报List是从右边塞入的，因此要从左边读取
	D_IF_FALSE_RETURN(pRedis->listOp()->lRange(strKey, m_tHolder.m_lReaded, lLength - 1, &vecInfo));
}

void LoadBattleInfoTask::Callback(Replay& tHolder)
{
	// 成功读取
	// 将读到的数据存起来
	for (size_t i = 0; i < vecInfo.size(); i++)
	{
		tHolder.m_vecBattleInfo.push_back(SMsgView_VC_BattleInfo(tHolder.m_rID, std::move(vecInfo.at(i))));
	}
	vecInfo.clear();	// 清理，以备下次调用

	tHolder.m_lReaded = lLength;	// 记录读取数目

	if (m_timeEnd.IsValid())
		tHolder.m_bFinished = true;
}