/*******************************************************************
** �ļ���: WarMonsterMgr.cpp
** ��  Ȩ: (C) ���ڱ������缼�����޹�˾(www.weimingtech.com)
** ������: ���к�
** ��  ��: 2017/11/16
** ��  ��: 1.0
** ��  ��: ս�����ˢ�ֽӿ�
** Ӧ  ��:
**************************** �޸ļ�¼ ******************************
** �޸���:
** ��  ��:
** ��  ��:
********************************************************************/

#include "StdAfx.h"
#include "WarMonsterMgr.h"
#include "IWar.h"
#include "MonsBaseExec.h"
#include "MonsSoliderExec.h"
#include "MonsterExec.h"
#include "MonsTowerExec.h"
#include "IMonsterDiedExecMgr.h"
#include "IRandRefreshMonsMgr.h"
#include "ExternalHelper.h"

CWarMonsterMgr::CWarMonsterMgr(IWar * pWar)
	: m_pWar(pWar)
	, m_nWarType(0)
	, m_bReleaseAllMonster(false)        // �Ƿ��ͷŹ���
	, m_nWarMaxMonsterCount(MAX_INT_VALUE)
{
}

CWarMonsterMgr::~CWarMonsterMgr()
{
}

bool CWarMonsterMgr::Init(int nMatchType)
{
	if (m_pWar == NULL)
		return false;

	// ���������Ϣ
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return false;
	}

	ISchemeWarManager * pSchemeWarManager = pSchemeCenter->getSchemeWarManager();
	if (pSchemeWarManager == NULL)
	{
		return false;
	}
	
	m_nWarType = m_pWar->getWarTypeId();

	// ս����Ϣ
	SWarScheme tempScheme = pSchemeWarManager->GetWarScheme(m_nWarType);
	if (tempScheme.nStaticID <= 0)
	{
		return false;
	}

	if (LoadMonsterScheme() == false)
	{
		ErrorLn(__FUNCTION__":LoadMonsterScheme failed.");
		return false;
	}

	m_nWarMaxMonsterCount = tempScheme.MaxMonsterCount;

	return true;
}

void CWarMonsterMgr::Release()
{
	// �ر�ˢ�ּ�ⶨʱ��
	g_EHelper.KillTimer(ETimerID_RefreshMonster, this);

	m_pWar = NULL;

	delete this;
}

void CWarMonsterMgr::onWarRealStart()
{
	// ����ˢ�ּ�ⶨʱ��
	g_EHelper.SetTimer(ETimerID_RefreshMonster, ETimerInternal_RefreshMonster, this);
}

void CWarMonsterMgr::onWarEnd()
{
	// �ر�ˢ�ּ�ⶨʱ��
	g_EHelper.KillTimer(ETimerID_RefreshMonster, this);
}

bool CWarMonsterMgr::LoadMonsterScheme()
{
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return false;
	}

	ISchemeWarManager * pSchemeWarManager = pSchemeCenter->getSchemeWarManager();
	if (pSchemeWarManager == NULL)
	{
		return false;
	}
	// ս����Ϣ
	ISchemeMatchRoom *pSchemeMatchRoom = pSchemeCenter->getSchemeMatchRoom();
	if (pSchemeMatchRoom == NULL)
	{
		ErrorLn(__FUNCTION__": failed, pWarCampInfo == NULL");
		return false;
	}
	EWarMonsTypeVect * pRefreshVect = pSchemeWarManager->GetWarEMonstTypeVect(m_nWarType);
	if (pRefreshVect == NULL)
	{
		SWarCreateContext* pContext = m_pWar->getWarCreateContext();
		ErrorLn(__FUNCTION__": failed, pRefreshVecpRefreshVect == NULL, mode=" << pContext->byMode << ", dwMapID=" << pContext->dwMapID << ", static=" 
			<< pContext->nConfigWarID << ",scene=" << m_pWar->getWarSceneID());

		return false;
	}

	// build monster type info
	EWarMonsTypeVect::iterator itType = pRefreshVect->begin();
	for (; itType != pRefreshVect->end(); ++itType)
	{
		// ���ݾ�̬ID�����Ҫ��ʼ��������
		IMonsterExec *pMonsterExec = NULL;
		switch (*itType)
		{
		case EWME_Base:     // ���صĴ���ʽ
			{
				pMonsterExec = CMonsBaseExec::execCreateMonster(m_pWar, *itType);
			}
			break;

		case EWME_Tower:
			{
				pMonsterExec = CMonsTowerExec::execCreateMonster(m_pWar, *itType);
			}
			break;

		case EWME_Crystal:
			{
				pMonsterExec = CMonsTowerExec::execCreateMonster(m_pWar, *itType);
			}
			break;

		case EWME_Soldier:  // ʿ���Ĵ���ʽ
			{
				pMonsterExec = CMonsSoliderExec::execCreateMonster(m_pWar, *itType);
			}
			break;

		case EWME_Monster:  // Ұ�ֵĴ���ʽ
			{
				pMonsterExec = CMonsterExec::execCreateMonster(m_pWar, *itType);
			}
			break;

		case EWME_BOSS:
			{
				pMonsterExec = CMonsterExec::execCreateMonster(m_pWar, *itType);
			}
			break;

		default:
			{
				Error(__FUNCTION__": EWarMonsterExec *itType=" << *itType << endl);
			}
			break;
		}
		if (pMonsterExec != NULL)
		{
			m_mapAllMonstExec[*itType] = pMonsterExec;
		}
	}
	return true;
}

DWORD CWarMonsterMgr::getUIDListByMonsterType(BYTE bySelfCamp, BYTE byGetCampType, BYTE bySerchType, PDBID * pReturnArray, DWORD dwArrayMaxSize)
{
	DWORD dwArraySize = 0;

	// ���ҷ�����������
	CreateMonUIDList retMap;
	for (MonsterExecMap::iterator it = m_mapAllMonstExec.begin(); it != m_mapAllMonstExec.end(); ++it)
	{
		if (it->first & bySerchType)
		{
			it->second->serchCampUIDList(bySelfCamp, byGetCampType, retMap);
		}
	}

	for (CreateMonUIDList::iterator iter = retMap.begin(); iter != retMap.end(); ++iter)
	{
		if (dwArraySize >= dwArrayMaxSize)
		{
			return dwArrayMaxSize;
		}
		pReturnArray[dwArraySize++] = iter->first;
	}

	return dwArraySize;
}

IMonsterExec * CWarMonsterMgr::getWarMonsterExecByType(EWarMonsterExec WarMonsterExec)
{
	MonsterExecMap::iterator monsterExecIt = m_mapAllMonstExec.find(WarMonsterExec);
	if (monsterExecIt != m_mapAllMonstExec.end())
	{
		return monsterExecIt->second;
	}
	return NULL;
}


EWarMonsterExec CWarMonsterMgr::getWarMonsterType(int nIndex)
{
	for (MonsterExecMap::iterator it = m_mapAllMonstExec.begin(); it != m_mapAllMonstExec.end(); ++it)
	{
		if (it->second->findIndex(nIndex))
		{
			return it->first;
		}
	}
	return EWME_None;
}

EWarMonsterExec CWarMonsterMgr::getWarMonsterTypeByUID(UID uid)
{
	CreateMonsterMap::iterator itMonster = m_mapCreateMonster.find(uid);
	if (itMonster == m_mapCreateMonster.end())
	{
		return EWME_None;
	}
	return itMonster->second.eType;
}

CreateMonsterInfo * CWarMonsterMgr::getWarMonsterInfoByUID(UID uid)
{
	CreateMonsterMap::iterator itMonster = m_mapCreateMonster.find(uid);
	if (itMonster == m_mapCreateMonster.end())
	{
		return NULL;
	}

	return &itMonster->second;
}

vector<UID> CWarMonsterMgr::getWarMonsterList()
{
	vector<UID> result;

	CreateMonsterMap::iterator itMonster = m_mapCreateMonster.begin();
	for (; itMonster != m_mapCreateMonster.end(); ++itMonster)
	{
		result.push_back(itMonster->first);
	}

	return result;
}


void CWarMonsterMgr::timeExec()
{
	// ����������
	SERVICE_PTR pContainer = gServerGlobal->getSceneManager()->getScene(m_pWar->getWarSceneID());
	if (pContainer == NULL)
	{
		return;
	}
	IGameSceneService * pScene = (IGameSceneService*)pContainer->query_interface();
	if (pScene != NULL)
	{
		if (pScene->getMonsterCount() > getMaxMonsterCountLimit())
			return;
	}

	// ����ˢ�¼��
	MonsterExecMap::iterator itMonster = m_mapAllMonstExec.begin();
	for (; itMonster != m_mapAllMonstExec.end(); ++itMonster)
	{
		DWORD dwTick = getTickCountEx();
		itMonster->second->timeExec();
		if (getTickCountEx() - dwTick > 30)
		{
			ErrorLn(__FUNCTION__": timeExec: " << itMonster->first << " cost:" << getTickCountEx() - dwTick);
		}
	}
}

void CWarMonsterMgr::OnTimer(unsigned long dwTimerID)
{
	switch (dwTimerID)
	{
	case ETimerID_RefreshMonster:
		{
			timeExec();
		}
		break;
	default:break;
	}
}


void CWarMonsterMgr::stopMonsterAI()
{
	for (MonsterExecMap::iterator it = m_mapAllMonstExec.begin(); it != m_mapAllMonstExec.end(); ++it)
	{
		it->second->stopAI();
	}

	IRandRefreshMonsMgr* pRandRefreshMonsMgr = (IRandRefreshMonsMgr*)m_pWar->getWarMgr(Mgr_RandRefreshMons);
	if (pRandRefreshMonsMgr != NULL)
	{
		pRandRefreshMonsMgr->stopAI();
	}
}

void CWarMonsterMgr::releaseMonsterRes()
{
	if (m_bReleaseAllMonster)
	{
		return;
	}

	for (MonsterExecMap::iterator it = m_mapAllMonstExec.begin(); it != m_mapAllMonstExec.end(); ++it)
	{
		it->second->release();
	}

	m_mapAllMonstExec.clear();
	m_mapCreateMonster.clear();
	
	IRandRefreshMonsMgr* pRandRefreshMonsMgr = (IRandRefreshMonsMgr*)m_pWar->getWarMgr(Mgr_RandRefreshMons);
	if (pRandRefreshMonsMgr != NULL)
	{
		pRandRefreshMonsMgr->releaseMonsterRes();
	}

	m_bReleaseAllMonster = true;
}

void CWarMonsterMgr::onActorContinuePlay(int clientID)
{
	// ����С��ͼ��Ϣ
	for (MonsterExecMap::iterator it = m_mapAllMonstExec.begin(); it != m_mapAllMonstExec.end(); ++it)
	{
		it->second->sendMiniMapInfo(clientID);
	}
}

UID CWarMonsterMgr::createNpc(int nMonsterID, Vector3 vLoc, EWarMonsterExec eType, CreateNpcContext* pNpcContext, int nRefreshIndex, int nLocalIndex, INPCService*pNpcService /*= NULL*/, bool bRandomRefresh)
{
	//Error("CCMobaWar::createNpc() nMonsterID=" << nMonsterID << ",eType=" << eType << ",Camp=" << npcContext.nCamp << ",Path=" << npcContext.nPathID << endl);
	if (pNpcService == NULL)
	{
		SceneLogicHelper helper(m_pWar->getWarSceneID());
		ISceneLogicService* pSceneLogicService = helper.m_ptr;
		if (pSceneLogicService == NULL)
		{
			return INVALID_UID;
		}

		pNpcService = pSceneLogicService->getNPCService();
		if (pNpcService == NULL)
		{
			return INVALID_UID;
		}
	}

	UID uID = pNpcService->createNPC(nMonsterID, vLoc, *pNpcContext);
	CreateMonsterInfo tempInfo;
	tempInfo.eType = eType;
	tempInfo.nMonsterID = nMonsterID;
	tempInfo.nLocalIndex = nLocalIndex;
	tempInfo.nRefreshIndex = nRefreshIndex;
	tempInfo.bRandomRefresh = bRandomRefresh;
	tempInfo.dwCreateTick = getTickCountEx();
	m_mapCreateMonster[uID] = tempInfo;

	return uID;
}

void CWarMonsterMgr::onMonsterDie(sEntityDieInfo * pEntityDieInfo)
{
	// ���ҹ��������
	CreateMonsterMap::iterator itMonster = m_mapCreateMonster.find(pEntityDieInfo->uDiedUID);
	if (itMonster != m_mapCreateMonster.end())
	{
		if (itMonster->second.bRandomRefresh)
		{
			// �����������
			IMonsterDiedExecMgr* pMonsDiedExecMgr = (IMonsterDiedExecMgr*)m_pWar->getWarMgr(Mgr_MonsterDiedExec);
			if (pMonsDiedExecMgr != NULL)
			{
				pMonsDiedExecMgr->onMonsterDied(pEntityDieInfo);
			}
		}
		else
		{
			MonsterExecMap::iterator itExec = m_mapAllMonstExec.find(itMonster->second.eType);
			if (itExec != m_mapAllMonstExec.end())
			{
				itExec->second->execMonsterDead(*pEntityDieInfo);
			}
		}

		// ����ǰ����
		m_mapCreateMonster.erase(itMonster);
	}
}

void CWarMonsterMgr::onNpcDestroy(UID uid)
{
	// �Ƴ�NPC
	CreateMonsterMap::iterator itMonster = m_mapCreateMonster.find(uid);
	if (itMonster != m_mapCreateMonster.end())
	{
		m_mapCreateMonster.erase(itMonster);
	}
}
