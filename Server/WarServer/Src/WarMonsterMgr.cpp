/*******************************************************************
** 文件名: WarMonsterMgr.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/16
** 版  本: 1.0
** 描  述: 战场随机刷怪接口
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
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
	, m_bReleaseAllMonster(false)        // 是否释放怪物
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

	// 获得配置信息
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

	// 战场信息
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
	// 关闭刷怪检测定时器
	g_EHelper.KillTimer(ETimerID_RefreshMonster, this);

	m_pWar = NULL;

	delete this;
}

void CWarMonsterMgr::onWarRealStart()
{
	// 启动刷怪检测定时器
	g_EHelper.SetTimer(ETimerID_RefreshMonster, ETimerInternal_RefreshMonster, this);
}

void CWarMonsterMgr::onWarEnd()
{
	// 关闭刷怪检测定时器
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
	// 战场信息
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
		// 根据静态ID获得需要初始化的数据
		IMonsterExec *pMonsterExec = NULL;
		switch (*itType)
		{
		case EWME_Base:     // 基地的处理方式
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

		case EWME_Soldier:  // 士兵的处理方式
			{
				pMonsterExec = CMonsSoliderExec::execCreateMonster(m_pWar, *itType);
			}
			break;

		case EWME_Monster:  // 野怪的处理方式
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

	// 查找符合条件怪物
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
	// 检测怪物数量
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

	// 怪物刷新检测
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
	// 发送小地图信息
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
	// 查找怪物的死亡
	CreateMonsterMap::iterator itMonster = m_mapCreateMonster.find(pEntityDieInfo->uDiedUID);
	if (itMonster != m_mapCreateMonster.end())
	{
		if (itMonster->second.bRandomRefresh)
		{
			// 处理怪物死亡
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

		// 清理当前怪物
		m_mapCreateMonster.erase(itMonster);
	}
}

void CWarMonsterMgr::onNpcDestroy(UID uid)
{
	// 移除NPC
	CreateMonsterMap::iterator itMonster = m_mapCreateMonster.find(uid);
	if (itMonster != m_mapCreateMonster.end())
	{
		m_mapCreateMonster.erase(itMonster);
	}
}
