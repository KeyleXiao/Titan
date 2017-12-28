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

	// ��ȡ�ܳ���
	lLength = 0;
	D_IF_FALSE_RETURN(pRedis->listOp()->lLen(strKey, &lLength));

	if (m_tHolder.m_lReaded == lLength)
		return;

	// ս��List�Ǵ��ұ�����ģ����Ҫ����߶�ȡ
	D_IF_FALSE_RETURN(pRedis->listOp()->lRange(strKey, m_tHolder.m_lReaded, lLength - 1, &vecInfo));
}

void LoadBattleInfoTask::Callback(Replay& tHolder)
{
	// �ɹ���ȡ
	// �����������ݴ�����
	for (size_t i = 0; i < vecInfo.size(); i++)
	{
		tHolder.m_vecBattleInfo.push_back(SMsgView_VC_BattleInfo(tHolder.m_rID, std::move(vecInfo.at(i))));
	}
	vecInfo.clear();	// �����Ա��´ε���

	tHolder.m_lReaded = lLength;	// ��¼��ȡ��Ŀ

	if (m_timeEnd.IsValid())
		tHolder.m_bFinished = true;
}