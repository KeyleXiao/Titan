/*******************************************************************
** 文件名: EquipSchemeMgr.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/4/18
** 版  本: 1.0
** 描  述: 装备管理
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/


#include "EquipSchemeMgr.h"
#include "ISceneManager.h"
#include "ITimerService.h"
#include "IPlayerRole.h"
#include "WarMessageDef.h"
#include "SchemeWarDef.h"
#include "War_ManagedDef.h"
#include "MobaGoodsSchemeDef.h"
#include "ExternalHelper.h"
#include "IWarPart.h"
#include "IEntityEvent.h"

//#define __EQUIPSCHEME_MGR_DEBUG__ 

CEquipSchemeMgr::CEquipSchemeMgr()
	: m_nMatchTypeID(0)
	, m_pMaster(NULL)
	, m_isSaveOftenUseEquip(false)
	, m_nBaseWeight(0)
	, m_fDampRatio(0.f)
	, m_Pdbid(0)
	, m_nCurHeroID(0)
	, m_nWorldID(0)
{
}

CEquipSchemeMgr::~CEquipSchemeMgr()
{
}

bool CEquipSchemeMgr::onSerialize(rkt::ibuffer & in, int flag)
{
	return true;
}

bool CEquipSchemeMgr::deSerialize(rkt::obuf & out, int flag)
{
	return true;
}

void CEquipSchemeMgr::addEquip(int nEquipID)
{
	m_AllEquipList.push(nEquipID);
}

void CEquipSchemeMgr::revocationEquip()
{
	if (!m_AllEquipList.empty())
		m_AllEquipList.pop();
}

void CEquipSchemeMgr::onWarEnd()
{
	if (!m_isSaveOftenUseEquip)
	{
		return;
	}

	IDBEngineService* pDBEngine = gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		return;
	}

	// 计算核心装备
	calcOftenUseEquipOfMain();

	const SWarEquipScheme* pScheme = getEquipScheme(m_nCurHeroID, EWEST_OftenUse);
	if (pScheme == NULL)
	{
		ErrorLn(__FUNCTION__": No OftenUse equip scheme.");
		return;
	}

	DBREQ_PARAM_SAVE_EQUIPSCHEME dbParam;
	dbParam.bySchemeID = pScheme->bySchemeID;
	dbParam.dwPDBID = m_Pdbid;
	dbParam.wHeroID = m_nCurHeroID;
	sstrcpyn(dbParam.szAuthorName, pScheme->szAuthorName, sizeof(dbParam.szAuthorName));
	sstrcpyn(dbParam.szSchemeName, pScheme->szSchemeName, sizeof(dbParam.szSchemeName));
	sstrcpyn(dbParam.szSchemeTitle, pScheme->szSchemeTitle, sizeof(dbParam.szSchemeTitle));
	for (int i = 0; i < WARQUIPSCHEME_EQUIP_MAXSIZE; ++i)
	{
		dbParam.dwEquipID[i] = pScheme->equipWeightInfo[i].nEquipID;
		dbParam.nWeight[i] = pScheme->equipWeightInfo[i].nWeight;
	}

#ifdef __EQUIPSCHEME_MGR_DEBUG__
	printEquipSchemeInfo(m_nCurHeroID, *pScheme, " War ended, save often use equip scheme.");
#endif

	IDBEngineService* pDBEngineService = gServerGlobal->getDBEngineService();
	if (pDBEngineService == NULL)
	{
		ErrorLn(__FUNCTION__": pDBEngineService == NULL");
		return;
	}

	if (!pDBEngineService->executeSP(GAMEDB_REQUEST_SAVE_EQUIPSCHEME, dbParam.dwPDBID, (LPCSTR)&dbParam, sizeof(dbParam), this, m_nWorldID))
	{
		ErrorLn(__FUNCTION__":Execute  GAMEDB_REQUEST_SAVE_EQUIPSCHEME Failed.");
		return;
	}

}

void CEquipSchemeMgr::onMessage(int msgId, void * data, size_t len)
{
	switch (msgId)
	{
	case CS_MSG_WAR_ADD_EQUIPSCHEME:
		{
			onClientAddEquipScheme(data, len);
		}
		break;
	case CS_MSG_WAR_DEL_EQUIPSCHEME:
		{
			onClientDelEquipScheme(data, len);
		}
		break;
	case CS_MSG_WAR_MODIFY_EQUIPSCHEME:
		{
			onClientModifyEquipScheme(data, len);
		}
		break;
	case CS_MSG_WAR_READ_EQUIPSCHEME:
		{
			onClientReadEquipScheme(data, len);
		}
		break;
	}
}

bool CEquipSchemeMgr::init(__IEntity* pMaster, LONGLONG WarDBKey, int nMatchTypeID)
{
	if (pMaster == NULL)
		return false;

	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter is NULL.");
		return false;
	}

	m_pMaster = pMaster;
	m_nMatchTypeID = nMatchTypeID;
	m_Pdbid = m_pMaster->getIntProperty(PROPERTY_ID);
	m_nCurHeroID = m_pMaster->getIntProperty(PROPERTY_VOCATION);
	m_nWorldID = m_pMaster->getFromGameWorldID();

	gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__);

	// 在战场则请求常用装备
	__IWarPart* pWarPart = (__IWarPart*)m_pMaster->getEntityPart(PART_WAR);
	if (pWarPart != NULL && pWarPart->getWarID() != INVALID_WAR_ID)
	{
		ISchemeWarSubFunc* pSchemeWarSubFunc = pSchemeCenter->getSchemeWarSubFunc();
		if (pSchemeWarSubFunc == NULL)
		{
			ErrorLn(__FUNCTION__": pSchemeWarSubFunc is NULL.");
			return false;
		}
		m_isSaveOftenUseEquip = pSchemeWarSubFunc->isEnableSubFunc(m_nMatchTypeID, WarSubFuncType_SaveOftenUseEquip);

		// 开启了常用装备保存功能
		if (m_isSaveOftenUseEquip)
		{
			gServerGlobal->getTimerService()->SetTimer(ETimerEventID_OftenUseEquip, WarOftenUseEquipTime, this, 1);

			ISchemeWarOftenUseEquip* pSchemeWarOftenUseEquip = pSchemeCenter->getSchemeWarOftenUseEquip();
			if (pSchemeWarOftenUseEquip == NULL)
			{
				ErrorLn(__FUNCTION__": pSchemeWarOftenUseEquip is NULL.");
				return false;
			}
			m_nBaseWeight = pSchemeWarOftenUseEquip->getBaseWeight();
			m_fDampRatio = pSchemeWarOftenUseEquip->getDampRatio();
		}

		// 去数据库查询英雄装备方案
		readEquipSchemeFromDB(m_nCurHeroID);

	}

	__IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
	if (pEntityEvent == NULL)
	{
		ErrorLn("EquipSchemeMgr onLoad pEntityEvent==NULL");
		return false;
	}
	pEntityEvent->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CONTINUE_PLAY, __FUNCTION__);

	return true;
}

void CEquipSchemeMgr::release()
{
	if (m_isSaveOftenUseEquip)
	{
		gServerGlobal->getTimerService()->KillTimer(ETimerEventID_OftenUseEquip, this);
	}

	if (m_pMaster != NULL)
	{
		__IEntityEvent *pEntityEvent = m_pMaster->getEntityEvent();
		if (pEntityEvent != NULL)
		{
			pEntityEvent->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CONTINUE_PLAY);
		}
	}

	gServerGlobal->getDBEngineService()->unregisterHandler(this);

	m_pMaster = NULL;

	delete this;
}

void CEquipSchemeMgr::OnTimer(unsigned long dwTimerID)
{
	switch (dwTimerID)
	{
	case ETimerEventID_OftenUseEquip:
		{
			calcOftenUseEquipOfEarly();
		}
		break;
	default:
		break;
	}
}

void CEquipSchemeMgr::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_ENTITY_CONTINUE_PLAY:
		{
			// 续玩后重新下发重用装备
			sendEquipSchemeToClient(m_nCurHeroID);
		}
		break;
	default:break;
	}
}

void CEquipSchemeMgr::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	switch (nCmdID)
	{
	case GAMEDB_REQUEST_READ_EQUIPSCHEME_LIST:
		{
			onDBRetEquipSchemeList_Read(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	case GAMEDB_REQUEST_SAVE_EQUIPSCHEME:
		{
			onDBRetEquipSchemeList_Save(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	case GAMEDB_REQUEST_DEL_EQUIPSCHEME:
		{
			onDBRetEquipSchemeList_Del(pRealDBRetSink, nCmdID, nDBRetCode, pszDBRetDesc, nQueueIndex, pOutData, nOutLen);
		}
		break;
	default:break;
	}
}

// 计算常用装备的前期装备
void CEquipSchemeMgr::calcOftenUseEquipOfEarly()
{
	// 本局所有购买过的前期装备
	vector<SEquipWeightInfo> curEquipWeightList;

	stack<int> tmpEquipList = m_AllEquipList;
	while (!tmpEquipList.empty())
	{
		SEquipWeightInfo info;
		info.nEquipID = tmpEquipList.top();
		info.nWeight = 0;

		// 判断是否小件物品
		if(g_EHelper.isLowGoods(info.nEquipID))
		{
			curEquipWeightList.push_back(info);
		}

		tmpEquipList.pop();
	}

	// 去掉重复的装备
	sort(curEquipWeightList.begin(), curEquipWeightList.end(), SEquipWeightInfo::compareForUnique);
	curEquipWeightList.erase(unique(curEquipWeightList.begin(), curEquipWeightList.end()), curEquipWeightList.end());

	// 获取旧的前期装
	vector<SEquipWeightInfo> oldEquipWeidhtList;
	getOftenUseEquipOfEarly(oldEquipWeidhtList);

	// 计算新的前期装
	vector<SEquipWeightInfo> newEquipWeidhtList;
	calcNewOftenUseEquip(curEquipWeightList, oldEquipWeidhtList, newEquipWeidhtList);

	// 保存新的前期装
	saveOftenUseEquip(newEquipWeidhtList, EWarEquipType_Early);
}

// 保存常用准备
void CEquipSchemeMgr::saveOftenUseEquip(const vector<SEquipWeightInfo>& allEquipWeidhtList, EWarEquipType nEquiType)
{
	if (allEquipWeidhtList.empty())
	{
		return;
	}

	if (m_pMaster == NULL)
	{
		return;
	}


	// 还没有常用装备
	if (!isExistEquipSchemeID(m_nCurHeroID, EWEST_OftenUse))
	{
		SWarEquipScheme newScheme;
		newScheme.bySchemeID = EWEST_OftenUse;
		sstrcpyn(newScheme.szAuthorName, m_pMaster->getName(), sizeof(newScheme.szAuthorName));
		sstrcpyn(newScheme.szSchemeName, a2utf8("常用方案"), sizeof(newScheme.szSchemeName));
		sstrcpyn(newScheme.szSchemeTitle, a2utf8("常用方案"), sizeof(newScheme.szSchemeTitle));

		m_EquipSchemeMap[m_nCurHeroID][newScheme.bySchemeID] = newScheme;
	}

	// 当前常用装备的前期装备
	const SWarEquipScheme* oftenUseScheme = getEquipScheme(m_nCurHeroID, EWEST_OftenUse);

	SWarEquipScheme tmpOftenUseScheme = *oftenUseScheme;

	// 保存最新的常用装备的前期装备
	size_t nSize = allEquipWeidhtList.size();


	// 前期装和核心装都是6个
	const int WarEquipNum = WARQUIPSCHEME_EQUIP_MAXSIZE / 2;

	if (nEquiType == EWarEquipType_Early)	// 前期装
	{
		memset(tmpOftenUseScheme.equipWeightInfo, 0, sizeof(SEquipWeightInfo) * WarEquipNum);
		int minIndex = 0;
		for (int i = 0; i < nSize; ++i)
		{
			if (minIndex >= WarEquipNum)
			{
				break;
			}

			tmpOftenUseScheme.equipWeightInfo[minIndex++] = allEquipWeidhtList[i];
		}
	}
	else if (nEquiType == EWarEquipType_Main)		// 核心装
	{
		memset(tmpOftenUseScheme.equipWeightInfo + WarEquipNum, 0, sizeof(SEquipWeightInfo) * (WARQUIPSCHEME_EQUIP_MAXSIZE - WarEquipNum));
		int minIndex = WarEquipNum;

		for (int i = 0; i < nSize; ++i)
		{
			if (minIndex >= WARQUIPSCHEME_EQUIP_MAXSIZE)
			{
				break;
			}

			tmpOftenUseScheme.equipWeightInfo[minIndex++] = allEquipWeidhtList[i];
		}
	}

	m_EquipSchemeMap[m_nCurHeroID][EWEST_OftenUse] = tmpOftenUseScheme;
}

// 计算核心装备
void CEquipSchemeMgr::calcOftenUseEquipOfMain()
{
	//// 战场结束时候的装备
	//vector<SEquipWeightInfo> curEquipWeightList;
	//for (int i = 0; i < equipList.size(); ++i)
	//{
	//	SEquipWeightInfo info;
	//	info.nEquipID = equipList[i];
	//	info.nWeight = 0;

	//	curEquipWeightList.push_back(info);
	//}

	// 本局所有购买过的核心装备
	vector<SEquipWeightInfo> curEquipWeightList;

	stack<int> tmpEquipList = m_AllEquipList;
	while (!tmpEquipList.empty())
	{
		SEquipWeightInfo info;
		info.nEquipID = tmpEquipList.top();
		info.nWeight = 0;

		// 判断是否大件物品
		if (g_EHelper.isHighGoods(info.nEquipID))
		{
			curEquipWeightList.push_back(info);
		}

		tmpEquipList.pop();
	}

	// 去重复的装备
	sort(curEquipWeightList.begin(), curEquipWeightList.end(), SEquipWeightInfo::compareForUnique);
	curEquipWeightList.erase(unique(curEquipWeightList.begin(), curEquipWeightList.end()),curEquipWeightList.end());

	// 旧的核心装
	vector<SEquipWeightInfo> oldEquipWeidhtList;
	getOftenUseEquipOfMain(oldEquipWeidhtList);

	// 计算新的核心装
	vector<SEquipWeightInfo> newEquipWeidhtList;
	calcNewOftenUseEquip(curEquipWeightList, oldEquipWeidhtList, newEquipWeidhtList);

	// 保存新的核心装
	saveOftenUseEquip(newEquipWeidhtList, EWarEquipType_Main);
}

// 获取常用前期装备
void CEquipSchemeMgr::getOftenUseEquipOfEarly(vector<SEquipWeightInfo>& infoList)
{
	const SWarEquipScheme* pScheme = getEquipScheme(m_nCurHeroID, EWEST_OftenUse);
	if (pScheme == NULL)
	{
		return;
	}

	for (int i = 0; i < (WARQUIPSCHEME_EQUIP_MAXSIZE / 2); ++i)
	{
		const SEquipWeightInfo& info = pScheme->equipWeightInfo[i];
		if (info.nEquipID <= 0)
		{
			continue;
		}

		infoList.push_back(info);
	}

}

// 获取常用核心装备
void CEquipSchemeMgr::getOftenUseEquipOfMain(vector<SEquipWeightInfo>& infoList)
{
	const SWarEquipScheme* pScheme = getEquipScheme(m_nCurHeroID, EWEST_OftenUse);
	if (pScheme == NULL)
	{
		return;
	}

	for (int i = (WARQUIPSCHEME_EQUIP_MAXSIZE / 2); i < WARQUIPSCHEME_EQUIP_MAXSIZE; ++i)
	{
		const SEquipWeightInfo& info = pScheme->equipWeightInfo[i];
		if (info.nEquipID <= 0)
		{
			continue;
		}

		infoList.push_back(info);
	}
}

// 计算新的常用装备
void CEquipSchemeMgr::calcNewOftenUseEquip(vector<SEquipWeightInfo>& curInfoList, const vector<SEquipWeightInfo>& oldInfoList, vector<SEquipWeightInfo>& newInfoList)
{
	// 统计所有装备的权重(本局购买过的装备+旧的前期装备)
	for (int i = 0; i < oldInfoList.size(); ++i)
	{
		const SEquipWeightInfo& oldInfo = oldInfoList[i];

		// 该装备是否也在本局出现
		bool isBuyAgain = false;

		// 计算上局和本局都出现的装备
		vector<SEquipWeightInfo>::iterator iter = curInfoList.begin();
		for (; iter != curInfoList.end(); )
		{
			// 当前的常用装备中的装备,在本局中又购买了的话
			if (oldInfo.nEquipID == iter->nEquipID)
			{
				// 最新权重 = 当前权重 * 80% + 基础权重
				SEquipWeightInfo newInfo;
				newInfo.nEquipID = oldInfo.nEquipID;
				newInfo.nWeight = oldInfo.nWeight * m_fDampRatio + m_nBaseWeight;

				newInfoList.push_back(newInfo);
				iter = curInfoList.erase(iter);

				isBuyAgain = true;
			}
			else
			{
				++iter;
			}
		}

		// 未在本局出现的装备
		if (!isBuyAgain)
		{
			// 最新权重 = 当前权重 * 80%
			SEquipWeightInfo newInfo;
			newInfo.nEquipID = oldInfo.nEquipID;
			newInfo.nWeight = oldInfo.nWeight * m_fDampRatio;
			newInfoList.push_back(newInfo);
		}
	}

	// 本局新出现的装备
	for (int i = 0; i < curInfoList.size(); ++i)
	{
		SEquipWeightInfo newInfo;
		newInfo.nEquipID = curInfoList[i].nEquipID;
		newInfo.nWeight = m_nBaseWeight;

		newInfoList.push_back(newInfo);
	}

	// 排序
	std::sort(newInfoList.begin(), newInfoList.end(), SEquipWeightInfo::compareForWeight);
}

void CEquipSchemeMgr::initOftenUseEquipFunc()
{


}

// 读取装备方案
void CEquipSchemeMgr::onDBRetEquipSchemeList_Read(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__ << pszDBRetDesc << " nDBRetCode=" << nDBRetCode);
		return;
	}

	if (nOutLen < sizeof(DBREQ_RESULT_READ_EQUIPSCHEME_LIST))
	{
		ErrorLn(__FUNCTION__": The length of outData is too short.nOutLen=" << nOutLen);
		return;
	}

	DBREQ_RESULT_READ_EQUIPSCHEME_LIST* pResult = (DBREQ_RESULT_READ_EQUIPSCHEME_LIST*)pOutData;
	int nSchemeCount = pResult->nCount;
	int nHeroID = pResult->wHeroID;

#ifdef	__EQUIPSCHEME_MGR_DEBUG__
	EmphasisLn(__FUNCTION__": The number of equip scheme num is " << nSchemeCount << ", pdbid=" << m_Pdbid << ", nHeroID=" << m_nCurHeroID);
#endif

	int nNeedLen = sizeof(DBREQ_RESULT_READ_EQUIPSCHEME_LIST) + nSchemeCount * sizeof(DBREQ_TABLE_EQUIP_SCHEME);
	if (nOutLen < nNeedLen)
	{
		ErrorLn(__FUNCTION__": The length of outData is too short than need length.nOutLen=" << nOutLen << " nNeedLen=" << nNeedLen);
		return;
	}

	DBREQ_TABLE_EQUIP_SCHEME* pScheme = (DBREQ_TABLE_EQUIP_SCHEME*)(pOutData + sizeof(DBREQ_RESULT_READ_EQUIPSCHEME_LIST));

	for (int i = 0; i < nSchemeCount; ++i)
	{
		SWarEquipScheme s;
		s.bySchemeID = pScheme->bySchemeID;
		sstrcpyn(s.szAuthorName, pScheme->szAuthorName, sizeof(s.szAuthorName));
		sstrcpyn(s.szSchemeName, pScheme->szSchemeName, sizeof(s.szSchemeName));
		sstrcpyn(s.szSchemeTitle, pScheme->szSchemeTitle, sizeof(s.szSchemeTitle));
		for (int j = 0; j < WARQUIPSCHEME_EQUIP_MAXSIZE; ++j)
		{
			s.equipWeightInfo[j].nEquipID = pScheme->dwEquipID[j];
			s.equipWeightInfo[j].nWeight = pScheme->nWeight[j];
		}

		if (m_EquipSchemeMap[pScheme->wHeroID].find(s.bySchemeID) != m_EquipSchemeMap[pScheme->wHeroID].end())
		{
			WarningLn(__FUNCTION__": Has same scheme id on equip scheme. schemeID=" << s.bySchemeID);
		}

#ifdef	__EQUIPSCHEME_MGR_DEBUG__
		//printEquipSchemeInfo(pScheme->wHeroID, s, "Read equip scheme");
#endif

		m_EquipSchemeMap[pScheme->wHeroID][pScheme->bySchemeID] = s;

		pScheme++;
	}

	// 获取常用装备方案为空的话，初始一个系统默认的
	if (m_isSaveOftenUseEquip)
	{
		const SWarEquipScheme* pOftenUseScheme = getEquipScheme(m_nCurHeroID, EWEST_OftenUse);
		if (pOftenUseScheme == NULL)
		{
			initDefaultOftenUseScheme();
		}
	}

	// 除了常用装备方案，其他方案都删掉(临时补丁)
	WAR_EQUIPSCHEME_MAP tmpEquipSchemeMap = m_EquipSchemeMap;
	for (WAR_EQUIPSCHEME_MAP::iterator iter = tmpEquipSchemeMap.begin(); iter != tmpEquipSchemeMap.end(); ++iter)
	{
		if (iter->first != nHeroID)
			continue;

		for (map<int, SWarEquipScheme>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
		{
			if (iter2->first != EWEST_OftenUse)
			{
				msg_war_del_equipscheme msg;
				msg.nHeroID = nHeroID;
				msg.bySchemeID = iter2->first;
				onClientDelEquipScheme(&msg, sizeof(msg));
			}
		}
	}


	// 把该英雄的装备方案发送到客户端
	sendEquipSchemeToClient(nHeroID);
}

void CEquipSchemeMgr::onDBRetEquipSchemeList_Save(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__ << pszDBRetDesc << " nDBRetCode=" << nDBRetCode);
		return;
	}

#ifdef	__EQUIPSCHEME_MGR_DEBUG__
	EmphasisLn(__FUNCTION__ " Save equip scheme succeed.");
#endif
}

void CEquipSchemeMgr::onDBRetEquipSchemeList_Del(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__ << pszDBRetDesc << " nDBRetCode=" << nDBRetCode);
		return;
	}

#ifdef	__EQUIPSCHEME_MGR_DEBUG__
	EmphasisLn(__FUNCTION__ " Del equip scheme succeed.");
#endif
}

void CEquipSchemeMgr::printEquipSchemeInfo(int nHeroID, const SWarEquipScheme& scheme, const char* debugInfo)
{
	TraceLn(_GT("----------------装备方案 Start--------------------"));
	TraceLn(debugInfo);
	TraceLn("pdbid:" << m_Pdbid);
	TraceLn("heroID:" << nHeroID);
	TraceLn("schemeID:" << scheme.bySchemeID);
	TraceLn("schemeName:" << scheme.szSchemeName);
	TraceLn("authorName:" << scheme.szAuthorName);
	TraceLn("schemeTitle:" << scheme.szSchemeTitle);
	for (int i = 0; i < WARQUIPSCHEME_EQUIP_MAXSIZE; ++i)
	{
		int nEquipID = scheme.equipWeightInfo[i].nEquipID;
		string str = g_EHelper.isHighGoods(nEquipID) ? "大件" : "小件";
		TraceLn(nEquipID << "," << g_EHelper.getGoodsName(nEquipID).c_str() << "," << scheme.equipWeightInfo[i].nWeight << "," << a2utf8(str));
	}
	TraceLn(_GT("--------------装备方案 End----------------------"));
}

const SWarEquipScheme* CEquipSchemeMgr::getEquipScheme(int nHeroID, byte bySchemeID)
{
	WAR_EQUIPSCHEME_MAP_ITER iter = m_EquipSchemeMap.find(nHeroID);
	if (iter == m_EquipSchemeMap.end())
	{
		return NULL;
	}

	map<int, SWarEquipScheme>::iterator iter2 = iter->second.find(bySchemeID);
	if (iter2 == iter->second.end())
	{
		return NULL;
	}

	return &iter2->second;
}

byte CEquipSchemeMgr::getFreeEquipSchemeID(int nHeroID)
{
	for (byte i = EWEST_Diy1; i < EWEST_Max; ++i)
	{
		const SWarEquipScheme* pScheme = getEquipScheme(nHeroID, i);
		if (pScheme == NULL)
		{
			return i;
		}
	}

	return EWEST_None;
}

bool CEquipSchemeMgr::isExistEquipSchemeID(int nHeroID, byte bySchemeID)
{
	return getEquipScheme(nHeroID, bySchemeID) != NULL;
}

bool CEquipSchemeMgr::readEquipSchemeFromDB(int nHeroID)
{

#ifdef __EQUIPSCHEME_MGR_DEBUG__
	TraceLn(__FUNCTION__": Read equip scheme. pdbid=" << m_Pdbid << ", nHeroID=" << nHeroID);
#endif

	// 添加到数据库
	DBREQ_PARAM_READ_EQUIPSCHEME_LIST dbParam;
	dbParam.dwPDBID = m_Pdbid;
	dbParam.wHeroID = nHeroID;

	IDBEngineService* pDBEngineService = gServerGlobal->getDBEngineService();
	if (pDBEngineService == NULL)
	{
		ErrorLn(__FUNCTION__": pDBEngineService == NULL");
		return false;
	}

	if (!pDBEngineService->executeSP(GAMEDB_REQUEST_READ_EQUIPSCHEME_LIST, m_Pdbid, (LPCSTR)&dbParam, sizeof(dbParam), this, m_nWorldID))
	{
		ErrorLn(__FUNCTION__ ":Execute  GAMEDB_REQUEST_READ_EQUIPSCHEME_LIST Failed.");
		return false;
	}

	return true;
}

void CEquipSchemeMgr::initDefaultOftenUseScheme()
{

#ifdef __EQUIPSCHEME_MGR_DEBUG__
	TraceLn(__FUNCTION__": init default oftenuse equip scheme. pdbid=" << m_Pdbid << ", nHeroID=" << m_nCurHeroID);
#endif

	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}
	ISchemeMobaGoods* pISchemeMobaGoods = gServerGlobal->getSchemeCenter()->getSchemeMobaGoods();
	if (pISchemeMobaGoods == NULL)
	{
		return;
	}
	SSchemeRecommendGood* pRecommandScheme = pISchemeMobaGoods->getRecommandGoods(m_nCurHeroID, m_nMatchTypeID);
	if (pRecommandScheme == NULL)
	{
		return;
	}

	// 还没有常用装备方案，则增加一个默认的
	vector<SEquipWeightInfo> defaultScheme;
	for (int i = 0; i < RECOMMEND_GOODS_MAX_COUNT; ++i)
	{
		SEquipWeightInfo info;
		info.nWeight = 0;
		info.nEquipID = pRecommandScheme->MiddleStage[i];
		defaultScheme.push_back(info);
	}
	saveOftenUseEquip(defaultScheme, EWarEquipType_Early);   // 前期装备

	defaultScheme.clear();
	for (int i = 0; i < RECOMMEND_GOODS_MAX_COUNT; ++i)
	{
		SEquipWeightInfo info;
		info.nWeight = 0;
		info.nEquipID = pRecommandScheme->DownWindStage[i];
		defaultScheme.push_back(info);
	}
	saveOftenUseEquip(defaultScheme, EWarEquipType_Main);    // 核心装备
}

void CEquipSchemeMgr::sendClientMessage(BYTE byKeyAction, LPCSTR pszData, int nDataLen)
{
	obuf obufData;
	SNetMsgHead head;
	head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
	head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
	head.byKeyModule = MSG_MODULEID_ENTITY;
	head.byKeyAction = byKeyAction;

	// 实体类统一用子头来用于转向部件
	SMsgEntityHead entityHead;
	entityHead.uidMaster = m_pMaster->getUID();
	entityHead.byPartID = PART_MOBAGOODS;

	obufData << head << entityHead;
	obufData.push_back(pszData, nDataLen);

	((__IPlayerRole*)m_pMaster)->send2Client(obufData.data(), obufData.size());
}

void CEquipSchemeMgr::sendEquipSchemeToClient(int nHeroID)
{
	obuf ob;
	obuf listBuf;

	msg_war_equipscheme_list msg;
	msg.nHeroID = nHeroID;
	msg.pdbid = m_Pdbid;
	msg.nSchemeCount = 0;

	// 方案列表数据
	WAR_EQUIPSCHEME_MAP_ITER iter = m_EquipSchemeMap.find(nHeroID);
	if (iter != m_EquipSchemeMap.end())
	{
		map<int, SWarEquipScheme>& schemeList = iter->second;
		msg.nSchemeCount = schemeList.size();

		map<int, SWarEquipScheme>::iterator iter2 = schemeList.begin();
		for (; iter2 != schemeList.end(); ++iter2)
		{
			listBuf << iter2->second;

#ifdef	__EQUIPSCHEME_MGR_DEBUG__
		printEquipSchemeInfo(nHeroID, iter2->second, "sendEquipSchemeToClient");
#endif

		}
	}

	ob << msg;
	ob.push_back(listBuf.data(), listBuf.size());

	sendClientMessage(SC_MSG_WAR_EQUIPSCHEME_LIST, ob.data(), ob.size());
}

void CEquipSchemeMgr::sendEquipSchemeToClient(int nHeroID, const SWarEquipScheme& scheme)
{
	msg_war_equipscheme_list msg;
	msg.nHeroID = nHeroID;
	msg.pdbid = m_Pdbid;
	msg.nSchemeCount = 1;

	obuf ob;
	ob << msg;
	ob << scheme;

	sendClientMessage(SC_MSG_WAR_EQUIPSCHEME_LIST, ob.data(), ob.size());
}

void CEquipSchemeMgr::onClientAddEquipScheme(void * data, size_t len)
{
	if (len < sizeof(msg_war_add_equipscheme))
	{
		return;
	}

	msg_war_add_equipscheme* msg = (msg_war_add_equipscheme*)data;

	int nHeroID = msg->nHeroID;

	byte bySchemeID = getFreeEquipSchemeID(nHeroID);
	if (bySchemeID == EWEST_None)
	{
		ErrorLn(__FUNCTION__": Equip scheme already full.");
		return;
	}

	// 添加到缓存
	SWarEquipScheme scheme;
	scheme.bySchemeID = bySchemeID;
	sstrcpyn(scheme.szAuthorName, msg->szAuthorName, sizeof(scheme.szAuthorName));
	sstrcpyn(scheme.szSchemeName, msg->szSchemeName, sizeof(scheme.szSchemeName));
	sstrcpyn(scheme.szSchemeTitle, msg->szSchemeTitle, sizeof(scheme.szSchemeTitle));
	for (int i = 0; i < WARQUIPSCHEME_EQUIP_MAXSIZE; ++i)
	{
		scheme.equipWeightInfo[i].nEquipID = msg->nEquipIDList[i];
		scheme.equipWeightInfo[i].nWeight = 0;
	}
	m_EquipSchemeMap[nHeroID][bySchemeID] = scheme;

	// 同步到客户端
	sendEquipSchemeToClient(nHeroID, scheme);

#ifdef __EQUIPSCHEME_MGR_DEBUG__
	printEquipSchemeInfo(nHeroID, scheme, "Add equip scheme");
#endif

	// 添加到数据库
	DBREQ_PARAM_SAVE_EQUIPSCHEME dbParam;
	dbParam.dwPDBID = m_Pdbid;
	dbParam.wHeroID = nHeroID;
	dbParam.bySchemeID = bySchemeID;
	sstrcpyn(dbParam.szAuthorName, msg->szAuthorName, sizeof(dbParam.szAuthorName));
	sstrcpyn(dbParam.szSchemeName, msg->szSchemeName, sizeof(dbParam.szSchemeName));
	for (int i = 0; i < WARQUIPSCHEME_EQUIP_MAXSIZE; ++i)
	{
		dbParam.dwEquipID[i] = msg->nEquipIDList[i];
		dbParam.nWeight[i] = 0;
	}

	IDBEngineService* pDBEngineService = gServerGlobal->getDBEngineService();
	if (pDBEngineService == NULL)
	{
		ErrorLn(__FUNCTION__": pDBEngineService == NULL");
		return;
	}

	if (!pDBEngineService->executeSP(GAMEDB_REQUEST_SAVE_EQUIPSCHEME, m_Pdbid, (LPCSTR)&dbParam, sizeof(dbParam), this, m_nWorldID))
	{
		ErrorLn(__FUNCTION__ "Execute  GAMEDB_REQUEST_SAVE_EQUIPSCHEME Failed.");
		return;
	}
}

void CEquipSchemeMgr::onClientDelEquipScheme(void * data, size_t len)
{
	if (len < sizeof(msg_war_del_equipscheme))
	{
		return;
	}

	msg_war_del_equipscheme* msg = (msg_war_del_equipscheme*)data;

	int nHeroID = msg->nHeroID;
	byte bySchemeID = msg->bySchemeID;

	if (!isExistEquipSchemeID(nHeroID, bySchemeID))
	{
		WarningLn(__FUNCTION__": Equip scheme not exist.bySchemeID=" << bySchemeID);
		return;
	}

	// 删除缓存中的装备方案
	m_EquipSchemeMap[nHeroID].erase(bySchemeID);

#ifdef __EQUIPSCHEME_MGR_DEBUG__
	TraceLn(__FUNCTION__": Del Equip scheme.pdbid=" << m_Pdbid << ", nHeroID=" << nHeroID << ", schemeID=" << bySchemeID);
#endif

	DBREQ_PARAM_DEL_EQUIPSCHEME dbParam;
	dbParam.dwPDBID = m_Pdbid;
	dbParam.wHeroID = nHeroID;
	dbParam.bySchemeID = bySchemeID;

	IDBEngineService* pDBEngineService = gServerGlobal->getDBEngineService();
	if (pDBEngineService == NULL)
	{
		ErrorLn(__FUNCTION__": pDBEngineService == NULL");
		return;
	}

	if (!pDBEngineService->executeSP(GAMEDB_REQUEST_DEL_EQUIPSCHEME, m_Pdbid, (LPCSTR)&dbParam, sizeof(dbParam), this, m_nWorldID))
	{
		ErrorLn(__FUNCTION__ "Execute  GAMEDB_REQUEST_DEL_EQUIPSCHEME Failed.");
		return;
	}

}

void CEquipSchemeMgr::onClientModifyEquipScheme(void * data, size_t len)
{
	if (len < sizeof(msg_war_modify_equipscheme))
	{
		return;
	}

	msg_war_modify_equipscheme* msg = (msg_war_modify_equipscheme*)data;

	int nHeroID = msg->nHeroID;
	byte bySchemeID = msg->bySchemeID;

	if (!isExistEquipSchemeID(nHeroID, bySchemeID))
	{
		WarningLn(__FUNCTION__": Equip scheme not exist.bySchemeID=" << bySchemeID);
		return;
	}

	// 修改缓存
	SWarEquipScheme& scheme = m_EquipSchemeMap[nHeroID][bySchemeID];
	scheme.bySchemeID = bySchemeID;
	sstrcpyn(scheme.szAuthorName, msg->szAuthorName, sizeof(scheme.szAuthorName));
	sstrcpyn(scheme.szSchemeName, msg->szSchemeName, sizeof(scheme.szSchemeName));
	sstrcpyn(scheme.szSchemeTitle, msg->szSchemeTitle, sizeof(scheme.szSchemeTitle));
	for (int i = 0; i < WARQUIPSCHEME_EQUIP_MAXSIZE; ++i)
	{
		scheme.equipWeightInfo[i].nEquipID = msg->nEquipIDList[i];
		scheme.equipWeightInfo[i].nWeight = 0;
	}

#ifdef __EQUIPSCHEME_MGR_DEBUG__
	printEquipSchemeInfo(nHeroID, scheme, "Modify equip scheme");
#endif

	// 添加到数据库
	DBREQ_PARAM_SAVE_EQUIPSCHEME dbParam;
	dbParam.dwPDBID = m_Pdbid;
	dbParam.wHeroID = nHeroID;
	dbParam.bySchemeID = bySchemeID;
	sstrcpyn(dbParam.szAuthorName, msg->szAuthorName, sizeof(dbParam.szAuthorName));
	sstrcpyn(dbParam.szSchemeName, msg->szSchemeName, sizeof(dbParam.szSchemeName));
	sstrcpyn(dbParam.szSchemeTitle, msg->szSchemeTitle, sizeof(dbParam.szSchemeTitle));
	for (int i = 0; i < WARQUIPSCHEME_EQUIP_MAXSIZE; ++i)
	{
		dbParam.dwEquipID[i] = msg->nEquipIDList[i];
		dbParam.nWeight[i] = 0;
	}

	IDBEngineService* pDBEngineService = gServerGlobal->getDBEngineService();
	if (pDBEngineService == NULL)
	{
		ErrorLn(__FUNCTION__": pDBEngineService == NULL");
		return;
	}

	if (!pDBEngineService->executeSP(GAMEDB_REQUEST_SAVE_EQUIPSCHEME, m_Pdbid, (LPCSTR)&dbParam, sizeof(dbParam), this, m_nWorldID))
	{
		ErrorLn(__FUNCTION__ "Execute  GAMEDB_REQUEST_SAVE_EQUIPSCHEME Failed.");
		return;
	}

	sendEquipSchemeToClient(nHeroID, scheme);
}

void CEquipSchemeMgr::onClientReadEquipScheme(void * data, size_t len)
{
	if (len < sizeof(msg_war_read_equipscheme))
	{
		return;
	}

	msg_war_read_equipscheme* msg = (msg_war_read_equipscheme*)data;

	int nHeroID = msg->nHeroID;

	readEquipSchemeFromDB(nHeroID);
}
