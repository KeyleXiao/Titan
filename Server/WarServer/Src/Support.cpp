/*******************************************************************
** 文件名: Support.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/3/15
** 版  本: 1.0
** 描  述: 点赞
** 应  用: 
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "StdAfx.h"
#include "Support.h"
#include "IWar.h"
#include "ITimerService.h"
#include "IOSSLogService.h"
#include "WarDef_Internal.h"
#include "OSSLogDef.h"

// 电脑玩家点赞定时器
#define WAR_SUPPORT_TIMER_ADD_LABEL 1
// 电脑玩家点赞间隔时间
#define WAR_SUPPORT_TIMER_ADD_LABEL_TIME 1000
// 游戏结束后延迟3s才第一个电脑玩家点赞
#define WAR_SUPPORT_FIRST_SUPPORT_TIME 8

CSupport::CSupport(IWar* pWar)
	:m_pWar(pWar)
	, m_nCurSupportIndex(0)
	, m_nFirstSupportTime(0)
{

}

CSupport::~CSupport()
{
	m_pWar = NULL;
}

void CSupport::OnTimer(unsigned long dwTimerID)
{
	if (dwTimerID == WAR_SUPPORT_TIMER_ADD_LABEL)
	{
		// 若干秒后才开始第一电脑玩家点赞
		m_nFirstSupportTime++;
		if (m_nFirstSupportTime <= WAR_SUPPORT_FIRST_SUPPORT_TIME)
			return;

		// 获取当前可以点赞的电脑
		PDBID operatorPdbid= getNextSupportPlayer();
		if (operatorPdbid == INVALID_PDBID)
		{
			gServerGlobal->getTimerService()->KillTimer(WAR_SUPPORT_TIMER_ADD_LABEL, this);
			return;
		}

		// 随机被点赞的玩家
		PDBID pdbid = randomSupportedPlayer();
		if (pdbid == INVALID_PDBID)
		{
			gServerGlobal->getTimerService()->KillTimer(WAR_SUPPORT_TIMER_ADD_LABEL, this);
			return;
		}

		// 随机标签
		int nLabelID = getRandomLabel(pdbid);
		if (nLabelID == 0)
		{
			return;
		}

		addLabelRequest(operatorPdbid, pdbid, nLabelID);
	}
}

bool CSupport::Init(int nMatchType)
{
	if (m_pWar == NULL)
		return false;

	if (!gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__))
	{
		ErrorLn(__FUNCTION__ " Register DBEngine service failed.");
		return false;
	}

	return true;
}

void CSupport::Release()
{
	if (!gServerGlobal->getDBEngineService()->unregisterHandler(this))
	{
		WarningLn(__FUNCTION__ " UnRegister DBEngine Service failed.");
	}

	gServerGlobal->getTimerService()->KillTimer(WAR_SUPPORT_TIMER_ADD_LABEL, this);

	delete this;
}

void CSupport::onWarRealStart()
{
}

void CSupport::onWarEnd()
{
}

void CSupport::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__ << " nCmdID=" << nCmdID << ",nDBRetCode=" << nDBRetCode << ",Desc=" << pszDBRetDesc);
		return;
	}

	switch (nCmdID)
	{
		case GAMEDB_REQUEST_ADD_SUPOORT:
		{

		}
		break;
		case GAMEDB_REQUEST_UPDATE_LABEL:
		{

		}
		break;
	}
}

void CSupport::filterPlayersForSupportCondition(bool bAwark)
{
	if (m_pWar == NULL)
	{
		return;
	}

	if(bAwark == false)
		return;

	m_mapSupportCacheEnd.clear();

	SESVTPersonData personESVTList[ESVT_MAX];

	// 计算点赞台资格的玩家列表
	getEveryESVTMaxList(personESVTList);

	// 过滤下不合格的点赞台资格玩家
	filterEveryESVTMaxList(personESVTList);

	// 把最终上点赞台的选手存起来
	updateESVTMaxListToCache(personESVTList);

	// 发到客户端
	updateAllFilteredPlayersForSupportToClient();

	// 计算所有电脑玩家
	calcAllComputerPlayer();

	// 缓存电脑玩家的名字,因为点赞界面时电脑实体已经释放了
	cacheAllPlayerName();

	// 计算点赞台上的真实玩家
	calcAllRealPlayer();

	//启动电脑玩家点赞定时器
	gServerGlobal->getTimerService()->SetTimer(WAR_SUPPORT_TIMER_ADD_LABEL, WAR_SUPPORT_TIMER_ADD_LABEL_TIME, this);
}


void CSupport::getEveryESVTMaxList(SESVTPersonData* personESVTList)
{
	if (m_pWar == NULL)
	{
		return;
	}

	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return;
	}

	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}

	ISchemeWarEndSupportPlayer *pSchemeWarEndSupport = pSchemeCenter->getSchemeWarEndSupportPlayer();
	if ((pSchemeWarEndSupport == NULL))
	{
		return;
	}

	SWarEndSupportVoteConfig_S supportVoteList[ESVT_MAX];
	if (!pSchemeWarEndSupport->getEndSupportPlayerVoteInfoList(supportVoteList))
	{
		return;
	}

	// 遍历所有的评选选项,选出5个
	map<PDBID, int> mapESVT;
	const int nESVTPlayerMax = 5;
	for (int nVoteID = 0; nVoteID < ESVT_MAX; ++nVoteID)
	{
		if (mapESVT.size() >= nESVTPlayerMax)
			break;

		vector<int> campList = m_pWar->getCampList();
		for(int i = 0; i < campList.size(); ++i)
		{
			int nCamp = campList[i];
			SWarCamp* pWarCamp = m_pWar->getWarCamp(nCamp);
			if (pWarCamp == NULL)
				continue;

			SCampTeamRecordInfo* pCampRecord = warMiscMgr->getCampRecordInfo(nCamp);
			if (pCampRecord == NULL)
			{
				continue;
			}

			WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
			for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
			{
				if (mapESVT.find(itPerson->uPDBID) != mapESVT.end())
					continue;
				SWPerson& pPerson = *itPerson;
				int nBaseRecord = getEveryESVTMaxValue(&pPerson, pCampRecord, nVoteID);
				if (nBaseRecord < supportVoteList[nVoteID].nParam1)	// 数据未达到该评选类型的最低要求
					continue;

				if (nBaseRecord > personESVTList[nVoteID].nBaseRecord) // 比记录中的玩家数据更高
				{
					personESVTList[nVoteID].nBaseRecord = nBaseRecord;
					personESVTList[nVoteID].dwActorID = itPerson->uPDBID;
				}
				else if (nBaseRecord == personESVTList[nVoteID].nBaseRecord)	// 数据相等的两个人,则比较下综合得分
				{
					SWPerson* pRecordPerson = m_pWar->getPlayerInfoByPDBIU(personESVTList[nVoteID].dwActorID);
					if (pRecordPerson != NULL)
					{
						if (itPerson->nBaseRecord[EDT_Score] > pRecordPerson->nBaseRecord[EDT_Score])
						{
							// 保存综合得分更高的玩家的数据
							personESVTList[nVoteID].nBaseRecord = nBaseRecord;
							personESVTList[nVoteID].dwActorID = itPerson->uPDBID;
						}
					}
				}
			}
		}

		// 计算出某个评选类型属于某个玩家后,存起来
		if (personESVTList[nVoteID].dwActorID > 0)
			mapESVT[personESVTList[nVoteID].dwActorID] = nVoteID;
	}
}

int CSupport::getEveryESVTMaxValue(const SWPerson* pPerson, const SCampTeamRecordInfo* pCampRecord, int nVoteID)
{
	if (m_pWar == NULL || pPerson == NULL || pCampRecord == NULL)
	{
		return 0;
	}

	int nBaseRecord = 0;
	switch (ESupportVoteType(nVoteID))
	{
	case ESVT_MVP:
	{
		if (pPerson->nCamp != m_pWar->getWinCamp())
			break;

		nBaseRecord = pPerson->nBaseRecord[EDT_Score];
	}
	break;
	case ESVT_PENTA:
	{
		nBaseRecord = pPerson->nTimeKill >= 5 ? 1 : 0;
	}
	break;
	case ESVT_ENTERBATTLERATE:
	{
		int nCampKillNum = max(pCampRecord->nCampKillNum, 1);
		nBaseRecord = 100 * ((float)pPerson->nBaseRecord[EDT_KillCount] + pPerson->nBaseRecord[EDT_AssistCount]) / nCampKillNum;
	}
	break;
	case ESVT_OUTPUTRATE:
	{
		int nCampOutput = max(pCampRecord->nCampOutput, 1);
		nBaseRecord = 100 * ((float)pPerson->nBaseRecord[EDT_OutPut] / nCampOutput);
	}
	break;
	case ESVT_CURETEARATE:
	{
		nBaseRecord = 0;// 策划未定
	}
	break;
	case ESVT_BEARDEMAGERATE:
	{
		int nCampBearDamage = max(pCampRecord->nCampBearDamage, 1);
		nBaseRecord = 100 * (float)pPerson->nBaseRecord[EDT_Damage] / nCampBearDamage;
	}
	break;
	case ESVT_KILLLDRAGEN:
	{

	}
	break;
	case ESVT_KILLSDRAGEN:
	{
		nBaseRecord = pPerson->nBigBossMonster + pPerson->nSmallBossMonster;
	}
	break;
	case ESVT_SLEVELTITLE:
	{

	}
	break;
	case ESVT_KILLPERSON:
	{
		nBaseRecord = pPerson->nBaseRecord[EDT_KillCount];
	}
	break;
	case ESVT_ALIVEKILLMAX:
	{
		nBaseRecord = pPerson->nDeadKill;
	}
	break;
	case ESVT_ASSISPERSON:
	{
		nBaseRecord = pPerson->nBaseRecord[EDT_AssistCount];
	}
	break;
	case ESVT_ALIVEASSISTMAX:
	{
		nBaseRecord = pPerson->nAliveContAssistMax;
	}
	break;
	case ESVT_DESTROYTOWER:
	{
		nBaseRecord = pPerson->nBaseRecord[EDT_DestoryTower];
	}
	break;
	case ESVT_OUTPUTDEMAGE:
	{
		nBaseRecord = pPerson->nBaseRecord[EDT_OutPut];
	}
	break;
	case ESVT_BEARDEMAGE:
	{
		nBaseRecord = pPerson->nBaseRecord[EDT_Damage];
	}
	break;
	case ESVT_CURETEAM:
	{
		nBaseRecord = pPerson->nBaseRecord[EDT_Cure];
	}
	break;
	case ESVT_DIEMAX:
	{
		nBaseRecord = pPerson->nDeadNum;
	}
	break;
	case ESVT_DIEMOREKILL:
	{
		nBaseRecord = pPerson->nBaseRecord[EDT_KillCount] - pPerson->nDeadNum;
		nBaseRecord = max(nBaseRecord, 0);
	}
	break;
	//case ESVT_KILLMONSTER:
	//{
	//	nBaseRecord = pPerson->nBaseRecord[EDT_KillMonster];
	//}
	//break;
	case ESVT_MONERYMOST:
	{
		nBaseRecord = pPerson->nBaseRecord[EDT_Money];
	}
	break;
	case ESVT_FLYSUPPOT:
	{

	}
	break;
	default:
		break;
	}

	return nBaseRecord;
}

void CSupport::filterEveryESVTMaxList(SESVTPersonData* personESVTList)
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		return;
	}

	ISchemeWarEndSupportPlayer *pSchemeWarEndSupport = pSchemeCenter->getSchemeWarEndSupportPlayer();
	if ((pSchemeWarEndSupport == NULL))
	{
		return;
	}

	SWarEndSupportVoteConfig_S supportVoteList[ESVT_MAX];
	if (!pSchemeWarEndSupport->getEndSupportPlayerVoteInfoList(supportVoteList))
	{
		return;
	}

	for (int i = 0; i < ESVT_MAX; ++i)
	{
		if (personESVTList[i].nBaseRecord < supportVoteList[i].nParam1)
			memset(&personESVTList[i], 0, sizeof(personESVTList[i]));
	}
}

void CSupport::updateESVTMaxListToCache(SESVTPersonData* personESVTList)
{
	if (m_pWar == NULL)
	{
		return;
	}

	const int nLimitCount = 5;

	// 把玩家获得的点赞评选类型添加到缓存表中
	for (int i = 0; i < ESVT_MAX; ++i)
	{
		if (m_mapSupportCacheEnd.size() >= nLimitCount)
			break;

		PDBID uPDBIU = personESVTList[i].dwActorID;
		if (uPDBIU <= 0 || personESVTList[i].nBaseRecord <= 0)
			continue;

		if (m_mapSupportCacheEnd.find(uPDBIU) != m_mapSupportCacheEnd.end())
			continue;

		SSupportVoteInfo info;
		info.nVoteID = i;
		info.uPDBIU = uPDBIU;

		m_mapSupportCacheEnd.insert(make_pair(info.uPDBIU, info));
	}

	if (m_mapSupportCacheEnd.size() <= 0)
	{
		// 没有一个玩家有上点赞台的资格,则计算各个阵营的最高综合分
		const int nCampListMax = 2;
		PDBID campBestDBList[nCampListMax] = { 0 };
		int nCampListIndex = 0;
		
		vector<int> campList = m_pWar->getCampList();
		for (int i = 0; i < campList.size(); ++i)
		{
			int nCamp = campList[i];
			SWarCamp* pWarCamp = m_pWar->getWarCamp(nCamp);
			if (pWarCamp == NULL)
				continue;

			int nComScore = 0;
			WarPersonVector::iterator itPer = pWarCamp->warPersVect.begin();
			for (; itPer != pWarCamp->warPersVect.end(); ++itPer)
			{
				if (itPer->nBaseRecord[EDT_Score] > nComScore)
				{
					if (nCampListIndex >= nCampListMax)
						continue;

					campBestDBList[nCampListIndex] = itPer->uPDBID;
					nComScore = itPer->nBaseRecord[EDT_Score];
				}
			}

			nCampListIndex++;
		}

		// 添加到点赞台资格缓存中
		for (int i = 0; i < nCampListMax; ++i)
		{
			SSupportVoteInfo info;
			info.nVoteID = ESVT_CAMPCOMSCORE;
			info.uPDBIU = campBestDBList[i];

			m_mapSupportCacheEnd.insert(make_pair(info.uPDBIU, info));
		}
	}
	else
	{
		// 判断点赞台资格列表中是否有两个不同的阵营的玩家
		bool bHasOtherCamp = false;
		int nCompareCamp = -1;
		map<PDBID, SSupportVoteInfo> ::iterator itSupport = m_mapSupportCacheEnd.begin();
		for (; itSupport != m_mapSupportCacheEnd.end(); ++itSupport)
		{
			SWPerson* pPerson = m_pWar->getPlayerInfoByPDBIU(itSupport->first);
			if (pPerson == NULL)
				continue;

			if (nCompareCamp != -1 && nCompareCamp != pPerson->nCamp)
				bHasOtherCamp = true;

			nCompareCamp = pPerson->nCamp;
		}

		if (bHasOtherCamp)
			return;

		// 如果只有一个阵营的玩家,则计算另外一个阵营的综合分最高的玩家
		PDBID campBestPDBIU = 0;
		vector<int> campList = m_pWar->getCampList();
		for (int i = 0; i < campList.size(); ++i)
		{
			int nCamp = campList[i];

			if (nCompareCamp == nCamp)
				continue;

			SWarCamp* pWarCamp = m_pWar->getWarCamp(nCamp);
			if (pWarCamp == NULL)
				continue;

			int nComScore = 0;
			WarPersonVector::iterator itPer = pWarCamp->warPersVect.begin();
			for (; itPer != pWarCamp->warPersVect.end(); ++itPer)
			{
				if (itPer->nBaseRecord[EDT_Score] > nComScore)
				{
					nComScore = itPer->nBaseRecord[EDT_Score];
					campBestPDBIU = itPer->uPDBID;
				}
			}
		}

		if (campBestPDBIU <= 0)
			return;

		SSupportVoteInfo info;
		info.nVoteID = ESVT_CAMPCOMSCORE;
		info.uPDBIU = campBestPDBIU;

		if (m_mapSupportCacheEnd.size() >= nLimitCount)
			m_mapSupportCacheEnd.erase(--m_mapSupportCacheEnd.end());

		m_mapSupportCacheEnd.insert(make_pair(info.uPDBIU, info));
	}
}

void CSupport::updateAllFilteredPlayersForSupportToClient()
{
	if (m_pWar == NULL)
	{
		return;
	}

	obuf obData;
	int nPlayerCount = m_mapSupportCacheEnd.size();
	obData.push_back(&nPlayerCount, sizeof(int));
	map<PDBID, SSupportVoteInfo> ::iterator itPer = m_mapSupportCacheEnd.begin();
	for (; itPer != m_mapSupportCacheEnd.end(); ++itPer)
	{
		msg_war_filter_support_person msg;
		msg.uPDBIU = itPer->first;
		msg.nVoteID = itPer->second.nVoteID;
		obData.push_back(&msg, sizeof(msg));
	}

	m_pWar->broadCastDataToAllPlayer(SC_MSG_WAR_FILTER_SUPPORT_PERSON, obData.data(), obData.size());
}




void CSupport::broadcastAddLabelToAllPlayer(PDBID operatorPdbid, PDBID targetPdbid, int nLabelID)
{
	if (m_pWar == NULL)
	{
		return;
	}

	string operatorName;
	map<PDBID, string>::iterator iter = m_AllPlayerName.find(operatorPdbid);
	if (iter != m_AllPlayerName.end())
	{
		operatorName = iter->second;
	}

	string targetName;
	iter = m_AllPlayerName.find(targetPdbid);
	if (iter != m_AllPlayerName.end())
	{
		targetName = iter->second;
	}

	msg_war_add_label msg;
	msg.nLabelID = nLabelID;
	msg.targetPdbid = targetPdbid;
	sstrcpyn(msg.szOperatorName, operatorName.c_str(), sizeof(msg.szOperatorName));
	sstrcpyn(msg.szTargetName, targetName.c_str(), sizeof(msg.szTargetName));

	m_pWar->broadCastDataToAllPlayer(SC_MSG_WAR_ADD_LABEL, (LPCSTR)&msg, sizeof(msg));
}


// 注：给玩家点赞的时候同时会设置一个标签
void CSupport::addLabelRequest(PDBID operatorPdbid, PDBID targetPdbid, int nLabelID)
{
	if (m_pWar == NULL)
	{
		return;
	}

	// 验证操作玩家有效性,是否战场内玩家
	if (m_AllPlayerName.find(operatorPdbid) == m_AllPlayerName.end())
		return;

	// 每个玩家只能点赞一次
	if (find(m_AlreadySupportList.begin(), m_AlreadySupportList.end(), operatorPdbid) != m_AlreadySupportList.end())
	{
		return;
	}
	m_AlreadySupportList.push_back(operatorPdbid);

	// 目标玩家是否在点赞台
	if (m_mapSupportCacheEnd.find(targetPdbid) == m_mapSupportCacheEnd.end())
		return;

	// 验证标签ID合法性
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return;
	ISchemeWarEndSupportPlayer* pSchemeSupport = pSchemeCenter->getSchemeWarEndSupportPlayer();
	if (pSchemeSupport == NULL)
		return;
	int nPlayerVoteID = m_mapSupportCacheEnd[targetPdbid].nVoteID;
	if (!pSchemeSupport->isHasLabelID(nPlayerVoteID, nLabelID))
	{
		ErrorLn(__FUNCTION__ " There is no exist label, nLabelID=" << nLabelID);
		return;
	}

	// 点赞值上限判断
	int nVotePointLimit = 10;	//  后面取配置
	int nAddPoints = 1;
	m_mapSupportCacheEnd[targetPdbid].nVotePoint = min(nVotePointLimit, m_mapSupportCacheEnd[targetPdbid].nVotePoint + nAddPoints);

	// 广播给所有玩家
	broadcastAddLabelToAllPlayer(operatorPdbid, targetPdbid, nLabelID);

	// 电脑玩家直接返回
	SWPerson* pPerson = m_pWar->getPlayerInfoByPDBIU(targetPdbid);
	if (pPerson == NULL || pPerson->bIsComputer)
	{
		return;
	}

	ActorHelper helper(targetPdbid, ACTOR_DBID);
	IActorService* pActor = helper.m_ptr;
	if (pActor != NULL)
	{
		// 更新角色信息
		pActor->setActorBaseInfo(nAddPoints, ENPLAYERINFOTYPE_SUPPORTPOINT, ENCHANGEDATAMODE_ADD, OSS_NUMERIC_SUPPORTPOINT, OSS_RESOURCE_OPT_ADD, OSSRES_ADD_WAR_PRIZE, targetPdbid, "", 0);
	}

	IDBEngineService* pDBEngineService = gServerGlobal->getDBEngineService();
	if (NULL == pDBEngineService)
	{
		ErrorLn(__FUNCTION__ " NULL == pDBEngineService");
		return;
	}

	// 标签数据增加到数据库
	int nAddLabelNum = 1;
	DBREQ_PARAM_UPDATE_LABEL dbParam2;
	dbParam2.dwPDBID = targetPdbid;
	dbParam2.nLabelID = nLabelID;
	dbParam2.nLabelNum = nAddLabelNum;
	if (!pDBEngineService->executeSP(GAMEDB_REQUEST_UPDATE_LABEL, 0, (LPCSTR)&dbParam2, sizeof(dbParam2), CAST_TYPE(IDBRetSink*, this)))
	{
		ErrorLn("Execute GAMEDB_REQUEST_UPDATE_SUPOORT_INFO failed! dwPlayerID =" << dbParam2.dwPDBID);
	}

	// 点赞数更新到数据库
	int nAddSupportNum = 1;
	DBREQ_PARAM_ADD_SUPPORT dbParam;
	dbParam.dwPDBID = targetPdbid;
	dbParam.nAddSupportNum = nAddSupportNum;
	if (!pDBEngineService->executeSP(GAMEDB_REQUEST_ADD_SUPOORT, 0, (LPCSTR)&dbParam, sizeof(dbParam), CAST_TYPE(IDBRetSink*, this)))
	{
		ErrorLn("Execute GAMEDB_REQUEST_UPDATE_SUPOORT_INFO failed! dwPlayerID =" << dbParam.dwPDBID);
		return;
	}

	// 点赞数写入日志
	IOSSLogService * pOSSLogServer = gServerGlobal->getOSSLogService();
	if (pOSSLogServer != NULL)
	{
		int nTime = (int)time(NULL);
		DWORD dwWorldID = 0;
		if (pActor != NULL)
		{
			dwWorldID = pActor->getFromGameWorldID();
		}
		pOSSLogServer->addNumericLog(dwWorldID, OSS_OPT_TYPE_PERSON, targetPdbid, m_pWar->getWarDBKey(), nAddSupportNum, OSS_NUMERIC_SUPPORTPOINT, OSS_RESOURCE_OPT_ADD, OSSRES_ADD_HONOR_INC_VAL, nTime, a2utf8("点赞日志"));
	}
}

PDBID CSupport::getNextSupportPlayer()
{
	if (m_nCurSupportIndex >= m_AllComputer.size())
	{
		return INVALID_PDBID;
	}

	return m_AllComputer[m_nCurSupportIndex++];
}

void CSupport::calcAllComputerPlayer()
{
	if (m_pWar == NULL)
	{
		return;
	}

	UID personArray[10];
	int nSize = m_pWar->getAllPerson(personArray, 10);
	for (int i = 0; i < nSize; ++i)
	{
		UID uidTemp = personArray[i];
		SWPerson* pPserson = m_pWar->getPlayerInfo(uidTemp);
		if (pPserson == NULL)
			continue;

		PDBID pdbid = pPserson->uPDBID;
		if (pPserson->bIsComputer)
			m_AllComputer.push_back(pdbid);
	}
}

void CSupport::calcAllRealPlayer()
{
	if (m_pWar == NULL)
	{
		return;
	}

	std::map<PDBID, SSupportVoteInfo>::iterator iter = m_mapSupportCacheEnd.begin();
	for (; iter != m_mapSupportCacheEnd.end(); ++iter)
	{
		PDBID pdbid = iter->first;
		SWPerson* pPerson = m_pWar->getPlayerInfoByPDBIU(pdbid);
		if(pPerson == NULL)
			continue;

		if (pPerson->bIsComputer)
		{
			m_CPSupported.push_back(pdbid);
		}
		else
		{
			m_RPSupported.push_back(pdbid);
		}
	}
}

int CSupport::getRandomLabel(PDBID pdbid)
{
	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return 0;
	ISchemeWarEndSupportPlayer* pSchemeSupport = pSchemeCenter->getSchemeWarEndSupportPlayer();
	if (pSchemeSupport == NULL)
		return 0;
	if (m_mapSupportCacheEnd.find(pdbid) == m_mapSupportCacheEnd.end())
		return 0;

	int nVoteID = m_mapSupportCacheEnd[pdbid].nVoteID;

	return pSchemeSupport->getRandomLabel(nVoteID);
}

void CSupport::cacheAllPlayerName()
{
	UID personArray[10];
	int nSize = m_pWar->getAllPerson(personArray, 10);
	for (int i = 0; i < nSize; ++i)
	{
		UID uid = personArray[i];

		ActorHelper helper(uid);
		IActorService* pActorService = helper.m_ptr;
		if(pActorService == NULL)
			continue;
		
		string name = pActorService->getName();
		PDBID pdbid = pActorService->getProperty_Integer(PROPERTY_ID);
		m_AllPlayerName[pdbid] = name;
	}
}

PDBID CSupport::randomSupportedPlayer()
{
	PDBID result = INVALID_PDBID;
	if (m_RPSupported.empty())
	{
		// 随机点赞台的电脑玩家
		if (!m_CPSupported.empty())
		{
			int nRandNum = mt_range_rand(0, m_CPSupported.size() - 1);
			result = m_CPSupported[nRandNum];
		}
	}
	else if(m_RPSupported.size() == 1)	// 只有一个真实玩家,全部的玩家都有可能随机到（包括电脑玩家）
	{
		// 有70%的几率一定给该真实玩家
		int nRandNum = mt_range_rand(0, 100);
		if (nRandNum < 70)
		{
			return m_RPSupported[0];
		}
		else
		{
			// 随机点赞台的电脑玩家
			int nRandNum = mt_range_rand(0, m_CPSupported.size() - 1);
			result = m_CPSupported[nRandNum];
		}
	}
	else	// 多个真实玩家,则只挑真人随机
	{
		int nRandNum = mt_range_rand(0, m_RPSupported.size() - 1);
		result = m_RPSupported[nRandNum];
	}

	return result;
}

void CSupport::onClientMsgPersonAddLabelRequest(UID uidActor, void* pData, size_t stLen)
{
	if (pData == NULL || stLen < sizeof(msg_war_add_label))
	{
		ErrorLn(__FUNCTION__": The length of msg is too short! stLen=" << stLen);
		return;
	}

	msg_war_add_label * pSuppotVoteInfo = (msg_war_add_label*)pData;
	PDBID targetPdbid = pSuppotVoteInfo->targetPdbid;
	int nLabelID = pSuppotVoteInfo->nLabelID;

	SWPerson* pPerson = m_pWar->getPlayerInfo(uidActor);
	if (pPerson == NULL)
		return;

	PDBID pdbid = pPerson->uPDBID;
	addLabelRequest(pdbid, targetPdbid, nLabelID);
}