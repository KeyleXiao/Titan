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

	// ��ʼ��ˢ����Ϣ
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

	// �ر�ˢ�ּ�ⶨʱ��
	g_EHelper.KillTimer(ETimerID_RefreshMonster, this);
	g_EHelper.KillTimer(ETimerID_MonsterLifeOver, this);

	delete this;
}

void CRandRefreshMonsMgr::onWarRealStart()
{
	// ����ˢ�ּ�ⶨʱ��
	g_EHelper.SetTimer(ETimerID_RefreshMonster, ETimerInternal_RefreshMonster, this);
	g_EHelper.SetTimer(ETimerID_MonsterLifeOver, ETimerInternal_MonsterLifeOver, this);
}

void CRandRefreshMonsMgr::onWarEnd()
{
	// �ر�ˢ�ּ�ⶨʱ��
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

	// ���������Ϣ
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

	// �������������
	vector<Vector3> locList;
	Cicle cicle;
	if (randRefreshMonsInfo->isInSafetyZone)
	{
		// �ڰ�ȫȦ�����ˢ
		if (pSafetyZoneMgr != NULL)
		{
			// �ȼ����ⲨҰ��Ȧ��Բ��
			cicle = pSafetyZoneMgr->getSafetyZone();
		}
	}
	else
	{
		// �ڷǰ�ȫȦ����Բ�����ˢ
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

			// ���������ɹ������Ϣ
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

	// ս����ʽ���е�ʱ�䣨���룩
	DWORD playTime = m_pWar->getWarPlayTime();

	// ���ʱ���Ƿ���ˢ��ʱ�䣬���������ˢ��
	vector<SWarRandRefreshMons>::iterator iter = pRefreshMonsInfo.begin();
	for (; iter != pRefreshMonsInfo.end(); ++iter)
	{
		// �Ѿ�ˢ��
		if (m_RandRefreshInfoList[iter->nMonsterIndexID].isRefresh)
			continue;

		if (playTime >= iter->nRefreshTime)
		{
			// ����ʱ��ˢ�¹���
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
			// ���ٹ���
			map<UID, int>::iterator iterMons = iter->second.createdMonUIDList.begin();
			for (; iterMons != iter->second.createdMonUIDList.end(); ++iterMons)
			{
				UID uid = iterMons->first;

				// ����NPC
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
