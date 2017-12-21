#include "StdAfx.h"
#include "RandRefreshMonsMgr.h"
#include "IRandRefreshMonsMgr.h"
#include "SchemeWarDef.h"
#include "IWar.h"
#include "ISafetyZoneMgr.h"
#include "ExternalHelper.h"
#include "HeightFieldMgr.h"
#include "IWarMonsterMgr.h"
#include "MathHelper.h"

CRandRefreshMonsMgr::CRandRefreshMonsMgr(IWar * pWar)
	:m_pWar(pWar)
	,m_nWarType(0)
{
}

CRandRefreshMonsMgr::~CRandRefreshMonsMgr()
{
}

bool CRandRefreshMonsMgr::Init(int nMatchType)
{
	if (m_pWar == NULL)
		return false;

	m_nWarType = m_pWar->getWarTypeId();

	// 初始化刷怪信息
	ISchemeWarRandRefreshMons* pSchemeRandMons = gServerGlobal->getSchemeCenter()->getSchemeWarRandRefreshMons();
	if (pSchemeRandMons == NULL)
		return false;
	vector<SWarRandRefreshMons> pRefreshMonsInfo = pSchemeRandMons->getRandRefreshMonsList(m_nWarType);

	vector<SWarRandRefreshMons>::iterator iter = pRefreshMonsInfo.begin();
	for (; iter != pRefreshMonsInfo.end(); ++iter)
	{
		m_RandRefreshInfoList[iter->nMonsterIndexID].nRefreshIndex = iter->nMonsterIndexID;
	}

	return true;
}

void CRandRefreshMonsMgr::Release()
{
	m_pWar = NULL;

	// 关闭刷怪检测定时器
	g_EHelper.KillTimer(ETimerID_RefreshMonster, this);
	g_EHelper.KillTimer(ETimerID_MonsterLifeOver, this);

	delete this;
}

void CRandRefreshMonsMgr::onWarRealStart()
{
	// 启动刷怪检测定时器
	g_EHelper.SetTimer(ETimerID_RefreshMonster, ETimerInternal_RefreshMonster, this);
	g_EHelper.SetTimer(ETimerID_MonsterLifeOver, ETimerInternal_MonsterLifeOver, this);
}

void CRandRefreshMonsMgr::onWarEnd()
{
	// 关闭刷怪检测定时器
	g_EHelper.KillTimer(ETimerID_RefreshMonster, this);
	g_EHelper.KillTimer(ETimerID_MonsterLifeOver, this);
}

void CRandRefreshMonsMgr::createMonster(SWarRandRefreshMons* randRefreshMonsInfo)
{
	IWarMonsterMgr* pWarMonsterMgr = (IWarMonsterMgr*)m_pWar->getWarMgr(Mgr_Monster);
	if (pWarMonsterMgr == NULL)
	{
		ErrorLn(__FUNCTION__":pWarMonsterMgr == NULL");
		return;
	}

	// 获得配置信息
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}

	ISchemeWarPoint * pSchemeWarPoint = pSchemeCenter->getSchemeWarPoint();
	if (pSchemeWarPoint == NULL)
	{
		return;
	}

	int nSceneID = m_pWar->getWarSceneID();
	SceneLogicHelper helper(nSceneID);
	ISceneLogicService* pSceneLogicService = helper.m_ptr;
	if (pSceneLogicService == NULL)
	{
		return;
	}

	INPCService* pNpcService = pSceneLogicService->getNPCService();
	if (pNpcService == NULL)
	{
		return;
	}

	ISchemeAIIDTable* pAIIDTable = gServerGlobal->getSchemeCenter()->getAISchemeIDTable();
	ISafetyZoneMgr* pSafetyZoneMgr = (ISafetyZoneMgr*)m_pWar->getWarMgr(Mgr_SafetyZone);

	WarPointVect warPointVector = pSchemeWarPoint->GetWarPointByIndex(randRefreshMonsInfo->nMonsterIndexID);
	int nMonsterCount = warPointVector.size();

	// 随机出怪物坐标
	vector<Vector3> locList;
	Cicle cicle;
	if (randRefreshMonsInfo->isInSafetyZone)
	{
		// 在安全圈内随机刷
		if (pSafetyZoneMgr != NULL)
		{
			// 先计算这波野怪圈的圆心
			cicle = pSafetyZoneMgr->getSafetyZone();
		}
	}
	else
	{
		// 在非安全圈定点圆内随机刷
		cicle.center = randRefreshMonsInfo->centerPos;
		cicle.radius = randRefreshMonsInfo->radiusRange;
	}
	locList = CMathHelper::getRandomPosOfCicleFixNumEx(cicle, randRefreshMonsInfo->fMonsDistance, nMonsterCount);

	SMonsterRandRefreshInfo& info = m_RandRefreshInfoList[randRefreshMonsInfo->nMonsterIndexID];
	info.dwRefreshTick = getTickCountEx();
	//for (vector<Vector3>::iterator iter = locList.begin(); iter != locList.end(); ++iter)
	int nLocNum = locList.size();
	for(int i = 0; i < nMonsterCount; ++i)
	{
		SWarPointScheme& warPoint = warPointVector[i];

		CreateNpcContext npcContext;
		npcContext.nWarID = m_pWar->getWarID();
		npcContext.nCamp = randRefreshMonsInfo->nMonsterCamp;
		npcContext.nAIID = pAIIDTable == NULL ? 0 : pAIIDTable->getNpcAISchemeID(m_pWar->getWarTypeId(), warPoint.nMonsterID);
		npcContext.nPathID = -1;
		npcContext.Angle = Vector3(warPoint.fAX, warPoint.fAY, warPoint.fAZ);

		if (i < nLocNum)
		{
			Vector3 tmpLoc = locList[i];
			//WarningLn(__FUNCTION__":loc before," << tmpLoc.x << "," << tmpLoc.y << "," << tmpLoc.z);
			gServerGlobal->getHeightFieldMgr()->correctPosFromPhysic(m_pWar->getMapID(), tmpLoc, 200, 200);
			//WarningLn(__FUNCTION__":loc after," << tmpLoc.x << "," << tmpLoc.y << "," << tmpLoc.z);

			UID uID = pWarMonsterMgr->createNpc(warPoint.nMonsterID, tmpLoc, EWME_Monster, &npcContext, warPoint.nIndex, warPoint.nLocalIndex, pNpcService, true);

			// 保存新生成怪物的信息
			if (uID != INVALID_UID)
			{
				info.createdMonUIDList[uID] = warPoint.nLocalIndex;
			}
		}
		else
		{
			ErrorLn(__FUNCTION__": The number of loc less than monster number.");
		}
	}

	//WarningLn("Create NPC...," << randRefreshMonsInfo->nMonsterIndexID);
}

void CRandRefreshMonsMgr::OnTimer(unsigned long dwTimerID)
{
	switch (dwTimerID)
	{
	case ETimerID_RefreshMonster:
		{
			checkRefreshMonster();
		}
		break;
	case ETimerID_MonsterLifeOver:
		{
			checkMonsterLife();
		}
		break;
	default:break;
	}

}

void CRandRefreshMonsMgr::checkRefreshMonster()
{
	ISchemeWarRandRefreshMons* pSchemeRandMons = gServerGlobal->getSchemeCenter()->getSchemeWarRandRefreshMons();
	if (pSchemeRandMons == NULL)
		return;
	vector<SWarRandRefreshMons> pRefreshMonsInfo = pSchemeRandMons->getRandRefreshMonsList(m_nWarType);

	// 战场正式进行的时间（毫秒）
	DWORD playTime = m_pWar->getWarPlayTime();

	// 检查时间是否到了刷新时间，如果到了则刷新
	vector<SWarRandRefreshMons>::iterator iter = pRefreshMonsInfo.begin();
	for (; iter != pRefreshMonsInfo.end(); ++iter)
	{
		// 已经刷新
		if (m_RandRefreshInfoList[iter->nMonsterIndexID].isRefresh)
			continue;

		if (playTime >= iter->nRefreshTime)
		{
			// 到了时间刷新怪物
			createMonster(&(*iter));

			m_RandRefreshInfoList[iter->nMonsterIndexID].isRefresh = true;
		}
	}
}

void CRandRefreshMonsMgr::checkMonsterLife()
{
	ISchemeWarRandRefreshMons* pSchemeRandMons = gServerGlobal->getSchemeCenter()->getSchemeWarRandRefreshMons();
	if (pSchemeRandMons == NULL)
	{
		return;
	}

	IWarMonsterMgr* pWarMonsterMgr = (IWarMonsterMgr*)m_pWar->getWarMgr(Mgr_Monster);
	if (pWarMonsterMgr == NULL)
	{
		ErrorLn(__FUNCTION__":pWarMonsterMgr == NULL");
		return;
	}

	int nSceneID = m_pWar->getWarSceneID();
	SceneLogicHelper helper(nSceneID);
	ISceneLogicService* pSceneLogicService = helper.m_ptr;
	if (pSceneLogicService == NULL)
	{
		return;
	}

	INPCService* pNpcService = pSceneLogicService->getNPCService();
	if (pNpcService == NULL)
	{
		return;
	}

	DWORD dwCurTick = getTickCountEx();

	MonsterRandRefreshMap::iterator iter = m_RandRefreshInfoList.begin();
	for (; iter != m_RandRefreshInfoList.end(); ++iter)
	{
		if (iter->second.isRefresh == false)
			continue;

		SWarRandRefreshMons* pRefreshMonsInfo = pSchemeRandMons->getRandRefreshMons(iter->first);
		if (pRefreshMonsInfo == NULL)
			continue;

		if (iter->second.dwRefreshTick <= 0)
			continue;

		if (iter->second.dwRefreshTick + pRefreshMonsInfo->nAliveTime <= dwCurTick)
		{
			// 销毁怪物
			map<UID, int>::iterator iterMons = iter->second.createdMonUIDList.begin();
			for (; iterMons != iter->second.createdMonUIDList.end(); ++iterMons)
			{
				UID uid = iterMons->first;

				// 销毁NPC
				pNpcService->destroyNPC(uid);

				pWarMonsterMgr->onNpcDestroy(uid);
			}

			iter->second.createdMonUIDList.clear();
			iter->second.dwRefreshTick = 0;

			WarningLn("Destroy NPC...," << iter->first);
		}
	}
	
}


void CRandRefreshMonsMgr::stopAI()
{
	int nSceneID = m_pWar->getWarSceneID();
	AIServiceHelper aiHelper(nSceneID);
	if (aiHelper.m_ptr == NULL)
	{
		WarningLn(__FUNCTION__": aiHelper.m_ptr == NULL");
		return;
	}

	MonsterRandRefreshMap::iterator iter = m_RandRefreshInfoList.begin();
	for (; iter != m_RandRefreshInfoList.end(); ++iter)
	{
		map<UID, int>::iterator iterMons = iter->second.createdMonUIDList.begin();
		for (; iterMons != iter->second.createdMonUIDList.end(); ++iterMons)
		{
			aiHelper.m_ptr->destroyAI(iterMons->first);
		}
	}
}

void CRandRefreshMonsMgr::releaseMonsterRes()
{
	int nSceneID = m_pWar->getWarSceneID();
	SceneLogicHelper helper(nSceneID);
	ISceneLogicService* pSceneLogicService = helper.m_ptr;
	if (pSceneLogicService == NULL)
	{
		return;
	}

	INPCService* pNpcService = pSceneLogicService->getNPCService();
	if (pNpcService == NULL)
	{
		return;
	}

	MonsterRandRefreshMap::iterator iter = m_RandRefreshInfoList.begin();
	for (; iter != m_RandRefreshInfoList.end(); ++iter)
	{
		map<UID, int>::iterator iterMons = iter->second.createdMonUIDList.begin();
		for (; iterMons != iter->second.createdMonUIDList.end(); ++iterMons)
		{
			pNpcService->destroyNPC(iterMons->first);
		}
	}
}
