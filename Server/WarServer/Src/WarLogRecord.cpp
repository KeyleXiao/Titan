/*******************************************************************
** 文件名: WarLogRecord.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/10/18
** 版  本: 1.0
** 描  述: 战场日志记录
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#include "StdAfx.h"
#include "WarLogRecord.h"
#include "IWar.h"
#include "IOSSLogServer.h"
#include "IOSSLogService.h"
#include "SWPerson.h"
#include "WarDef_Internal.h"
#include "ExternalHelper.h"

CWarLogRecord::CWarLogRecord(IWar* pWar)
	:m_pWar(pWar)
	, m_dwLastSaveLogTick(0)
{

}

CWarLogRecord::~CWarLogRecord()
{
	m_pWar = NULL;
}


bool CWarLogRecord::Init(int nMatchType)
{
	if (m_pWar == NULL)
	{
		return false;
	}

	IDBEngineService* pDBEngineService = gServerGlobal->getDBEngineService();
	if (pDBEngineService == NULL)
	{
		return false;
	}
	pDBEngineService->registerHandler(this, __FUNCTION__);

	return true;
}

void CWarLogRecord::Release()
{
	IDBEngineService* pDBEngineService = gServerGlobal->getDBEngineService();
	if (pDBEngineService)
	{
		pDBEngineService->unregisterHandler(this);
	}

	delete this;
}

void CWarLogRecord::onWarRealStart()
{
}

void CWarLogRecord::onWarEnd()
{
}

void CWarLogRecord::OnTimer(unsigned long dwTimerID)
{

}

void CWarLogRecord::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if (nDBRetCode != DBRET_CODE_SUCCEED)
		ErrorLn(__FUNCTION__":" << pszDBRetDesc);
}


void CWarLogRecord::fillWarActorLog(bool bIsEnd)
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

	IWar* pWarBase = (IWar*)m_pWar;

	DBREQ_PARAM_WAR_ACTOR_INFO warActorInfo;
	UID uidList[64] = { 0 };
	int nCount = m_pWar->getAllPerson(uidList, sizeof(uidList));
	for (int i = 0; i < nCount; ++i)
	{
		SWPerson* itBeginWPV = m_pWar->getPlayerInfo(uidList[i]);
		if (itBeginWPV == NULL)
			continue;

		if (UID_2_TYPE(itBeginWPV->uID) != TYPE_PLAYER_ROLE)
		{
			TraceLn(__FUNCTION__":UID_2_TYPE(itBeginWPV->uID) != TYPE_PLAYER_ROLE uId=" << itBeginWPV->uID);
			continue;
		}

		memset(&warActorInfo, 0, sizeof(warActorInfo));

		warActorInfo.dwWorldID = itBeginWPV->nSrcWorldID;
		warActorInfo.dwUserID = itBeginWPV->dwUserID;
		warActorInfo.dwActorID = itBeginWPV->uPDBID;
		warActorInfo.wHeroID = itBeginWPV->nHeroID;
		warActorInfo.wHeroLevel = itBeginWPV->nLevel;
		warActorInfo.wSkinID = static_cast<WORD>(itBeginWPV->nHeroSkinID);
		warActorInfo.wKillPersonNum = itBeginWPV->nBaseRecord[EDT_KillCount];
		warActorInfo.wDieCount = itBeginWPV->nDeadNum;
		warActorInfo.dwGetMoney = itBeginWPV->nBaseRecord[EDT_Money];
		warActorInfo.wAssistCount = itBeginWPV->nBaseRecord[EDT_AssistCount];
		warActorInfo.wKillMonsterNum = itBeginWPV->nBaseRecord[EDT_KillMonster];
		warActorInfo.wContinueKillMax = itBeginWPV->nDeadKill;
		warActorInfo.byMultiKillMax = itBeginWPV->nTimeKill;
		warActorInfo.byKillTowerNum = itBeginWPV->nBaseRecord[EDT_DestoryTower];
		warActorInfo.dwOutHeroPhysics = itBeginWPV->nOutHeroPhysics;
		warActorInfo.dwOutHeroMagic = itBeginWPV->nOutHeroMagic;
		warActorInfo.dwOutTotal = itBeginWPV->nBaseRecord[EDT_OutPut];
		warActorInfo.dwDamageTotal = itBeginWPV->nBaseRecord[EDT_Damage];
		warActorInfo.dwCureTotal = itBeginWPV->nBaseRecord[EDT_Cure];
		warActorInfo.byCamp = itBeginWPV->nCamp;
		BYTE byResultType = itBeginWPV->byResultType;
		warActorInfo.byWinType = byResultType;
		warActorInfo.byPlayerType = (itBeginWPV->bIsComputer ? (m_pWar->getWarCreateContext()->byRoomType == HallRoomType_Macth ? EWPT_VIRTUALPEOPLE : EWPT_COMPUTER) : EWPT_REALPEOPLE);
		warActorInfo.byThrillSave = itBeginWPV->nBaseRecord[EDT_ThrillSave];
		warActorInfo.byDeadlyControl = itBeginWPV->nBaseRecord[EDT_DeadlyControl];
		warActorInfo.byTeamBattleControlMax = itBeginWPV->nBaseRecord[EDT_TeamBattleControl];
		warActorInfo.byAliveAssistMax = itBeginWPV->nAliveContAssistMax;
		sstrcpyn(warActorInfo.szActorName, utf82a(itBeginWPV->szName), sizeof(warActorInfo.szActorName));
		warActorInfo.wGlory = itBeginWPV->nTitle;
		int nDeadNum = max(itBeginWPV->nDeadNum, 1);
		warActorInfo.wKdaScore = (itBeginWPV->nBaseRecord[EDT_KillCount] + itBeginWPV->nBaseRecord[EDT_AssistCount]) / nDeadNum;
		warActorInfo.dwScore = itBeginWPV->nBaseRecord[EDT_Score];
		warActorInfo.dwBearMagic = itBeginWPV->nBearMagic;
		warActorInfo.dwBearPhysics = itBeginWPV->nBearPhysic;
		warActorInfo.dwBearTrue = itBeginWPV->nBearTrue;
		warActorInfo.dwOutHeroTrue = itBeginWPV->nOutHeroTrue;
		warActorInfo.byFirstBlood = (itBeginWPV->bFirstBlood ? 1 : 0);
		warActorInfo.dwRecoveryHP = itBeginWPV->nRecoveryHP;
		warActorInfo.dwCrildDamageMax = itBeginWPV->nMaxCritical;
		warActorInfo.dwXPSkillID = itBeginWPV->nXPSkillID;
		warActorInfo.dwSummerID = itBeginWPV->nSummerID;
		warActorInfo.nSex = itBeginWPV->byActorSex;
		warActorInfo.nHeroStar = itBeginWPV->byHeroStarLv;

		// 天赋激活历史记录
		int nTalentCount = min(DBDEF_WAR_ACTIVE_TALENT_SIZE, MAX_ACTIVE_TALENT_COUNT);
		for (int i = 0; i < nTalentCount; ++i)
		{
			warActorInfo.nTalentID[i] = itBeginWPV->TalentList[i];
		}

		// 装备id  下标代表槽位
		int nEquCount = min(DBDEF_EQUIP_MAX_INDEX, MAX_GOODS_SLOT_COUNT);
		for (int i = 0; i < nEquCount; ++i)
		{
			warActorInfo.dwEquipID[i] = itBeginWPV->nGoodList[i];
		}
		// 连杀数组
		{
			int nContKillIndex = 0;
			ContinuKillList::const_iterator iterDeadBegin = itBeginWPV->deadContKillList.begin();
			for (; iterDeadBegin != itBeginWPV->deadContKillList.end(); ++iterDeadBegin)
			{
				if (nContKillIndex < 0 || nContKillIndex >= DBDEF_WAR_ACTOR_AK_TK_TC_GROUP_LEN_MAX)
					continue;

				warActorInfo.wContinueKill[nContKillIndex++] = *iterDeadBegin;
			}
		}
		// 多杀数组
		{
			int nMultiKillIndex = 0;
			ContinuKillList::const_iterator iterMultiBegin = itBeginWPV->timeContKillList.begin();
			for (; iterMultiBegin != itBeginWPV->timeContKillList.end(); ++iterMultiBegin)
			{
				if (nMultiKillIndex < 0 || nMultiKillIndex >= DBDEF_WAR_ACTOR_AK_TK_TC_GROUP_LEN_MAX)
					continue;

				warActorInfo.byMultiKill[nMultiKillIndex++] = *iterMultiBegin;
			}
		}
		// 团控数组
		{
			int nTeamBattleIndex = 0;
			TeamControlList::const_iterator iterTeamBattleBegin = itBeginWPV->teamControlList.begin();
			for (; iterTeamBattleBegin != itBeginWPV->teamControlList.end(); ++iterTeamBattleBegin)
			{
				if (nTeamBattleIndex < 0 || nTeamBattleIndex >= DBDEF_WAR_ACTOR_AK_TK_TC_GROUP_LEN_MAX)
					continue;

				warActorInfo.byTeamBattleControl[nTeamBattleIndex++] = *iterTeamBattleBegin;
			}
		}

		// 每分钟金钱量
		int nContinueMin = max(pWarBase->getWarContinueTime() / 60, 1);
		warActorInfo.wGPM = itBeginWPV->nBaseRecord[EDT_Money] / nContinueMin;

		SCampTeamRecordInfo* pTeamRecordInfo = warMiscMgr->getCampRecordInfo(itBeginWPV->nCamp);
		if (pTeamRecordInfo != NULL)
		{
			// 金钱比
			int nCampMoney = max(pTeamRecordInfo->nCampMoney, 1);
			warActorInfo.byMoneyRate = 100 * ((float)itBeginWPV->nBaseRecord[EDT_Money] / nCampMoney);

			// 输出率
			int nCampOutput = max(pTeamRecordInfo->nCampOutput, 1);
			warActorInfo.byOutputRate = 100 * ((float)itBeginWPV->nBaseRecord[EDT_OutPut] / nCampOutput);

			// 参战率
			int nCampKillNum = max(pTeamRecordInfo->nCampKillNum, 1);
			warActorInfo.byJoinBattleRate = 100 * ((float)itBeginWPV->nBaseRecord[EDT_KillCount] + itBeginWPV->nBaseRecord[EDT_AssistCount]) / nCampKillNum;
		}

		// 小龙击杀数
		warActorInfo.byKillDragen = itBeginWPV->nSmallBossMonster;
		// 击杀破塔得分
		warActorInfo.wKillDestroyScore = warMiscMgr->getKillAndDestroyTowerScore(*itBeginWPV);
		// 团队贡献得分
		warActorInfo.wTeamContributionScore = warMiscMgr->getTeamContributionScore(*itBeginWPV);
		// 特殊贡献得分
		warActorInfo.wSpeicalContributionScore = warMiscMgr->getSpeicalContributionScore(*itBeginWPV);
		// 总表现分
		warActorInfo.wComprehensiveScore = warActorInfo.wKillDestroyScore + warActorInfo.wTeamContributionScore + warActorInfo.wSpeicalContributionScore;
		if (bIsEnd)
		{
			m_mapWarActorLogEnd[warActorInfo.dwActorID] = warActorInfo;
		}
		else
		{
			m_mapWarActorLog[warActorInfo.dwActorID] = warActorInfo;
		}
	}
}

bool CWarLogRecord::batchSaveWarActorLog(bool bIsEnd)
{
	if (m_pWar == NULL)
	{
		return false;
	}

	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return false;
	}

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn("BatchSavePirateLog()  pDBEngine == NULL");
		return false;
	}

	DWORD dwEndTime = (DWORD)time(NULL);

	IWar* pWarBase = (IWar*)m_pWar;
	
	int nCamp1Count = 0;
	int nCamp1Score = 0;
	SWarCamp* pWarCamp1 = pWarBase->getWarCamp(CAMP_BLUE);
	if (pWarCamp1 != NULL)
	{
		nCamp1Count = pWarCamp1->nAIPlayerCount + pWarCamp1->nRealPlayerCount;
		nCamp1Score = pWarCamp1->nTotalKillNum;
	}

	int nCamp2Count = 0;
	int nCamp2Score;
	SWarCamp* pWarCamp2 = pWarBase->getWarCamp(CAMP_BLUE);
	if (pWarCamp2 != NULL)
	{
		nCamp2Count = pWarCamp2->nAIPlayerCount + pWarCamp2->nRealPlayerCount;
		nCamp2Score = pWarCamp2->nTotalKillNum;
	}

	DBREQ_PARAM_WAR_GAMERECORD gameRecord;
	gameRecord.nWarID = pWarBase->getWarDBKey();      // 战场数据库唯一id
	gameRecord.dwWorldID = getThisGameWorldID();	// 游戏世界ID
	gameRecord.wWarType = pWarBase->getMatchTypeID();   // 比赛类型
	gameRecord.dwBeginTime = pWarBase->getWarInfo()->BeginTime;   // 战场开始时间
	gameRecord.dwEnTime = dwEndTime;				// 战场结束时间            
	gameRecord.byEndBecause = pWarBase->getWarEndReason();    // 结束原因 1、代表阵营1胜利 2、代表阵营2胜利 3、代表阵营1投降 4、代表阵营2投降
	gameRecord.byCamp1Num = nCamp1Count;   // 阵营1人数 
	gameRecord.byCamp2Num = nCamp2Count;    // 阵营2人数
	gameRecord.wCamp1Score = nCamp1Score;     // 阵营1得分数
	gameRecord.wCamp2Score = nCamp2Score;      // 阵营2得分数
	gameRecord.dwOperateTime = dwEndTime;
	// 禁用英雄列表
	warMiscMgr->getCampBanHero(CAMP_BLUE, gameRecord.wBanHeroID1, DBDEF_BAN_HERO_SIZE);
	warMiscMgr->getCampBanHero(CAMP_RED, gameRecord.wBanHeroID2, DBDEF_BAN_HERO_SIZE);
	gameRecord.bIsFull = pWarBase->getWarInfo()->isPlayerFull;
	gameRecord.dwMatchScore = pWarBase->getWarCreateContext()->dwAvgRankScore;

	obuf128 dbbuf;
	obuf dbdata;
	DB_REQUEST_WARACTOR_SAVE log;
	log.warInfo = gameRecord;
	log.warInfo.bIsEnd = false;
	log.nCount = 0;

	size_t node_len = sizeof(DBREQ_PARAM_WAR_ACTOR_INFO);
	int nWarId = pWarBase->getWarID();
	for (std::map<PDBID, DBREQ_PARAM_WAR_ACTOR_INFO>::iterator iter = m_mapWarActorLog.begin(); iter != m_mapWarActorLog.end(); ++iter)
	{
		if ((log.nCount + 1) * node_len > 7000)
		{
			dbbuf.push_back(&log, sizeof(log));
			DWORD dwBytes = log.nCount * node_len;
			dbbuf.push_back(&dwBytes, sizeof(dwBytes));
			dbbuf.push_back(dbdata.data(), dbdata.size());

			if (pDBEngine->executeSP(GAMEDB_REQUEST_WAR_SAVE_GAME, nWarId, dbbuf.data(), dbbuf.size(), static_cast<IDBRetSink *>(this)) == false)
			{
				ErrorLn(__FUNCTION__": GAMEDB_REQUEST_WAR_SAVE_GAME to DB failed");
			}
			log.nCount = 0;
			dbdata.clear();
			dbbuf.clear();
		}

		log.nCount++;
		DBREQ_PARAM_WAR_ACTOR_INFO &detail = iter->second;
		dbdata.push_back(&detail, sizeof(DBREQ_PARAM_WAR_ACTOR_INFO));
	}
	if (log.nCount > 0)
	{
		dbbuf.push_back(&log, sizeof(log));
		DWORD dwBytes = log.nCount* node_len;
		dbbuf.push_back(&dwBytes, sizeof(dwBytes));
		dbbuf.push_back(dbdata.data(), dbdata.size());

		if (pDBEngine->executeSP(GAMEDB_REQUEST_WAR_SAVE_GAME, nWarId, dbbuf.data(), dbbuf.size(), static_cast<IDBRetSink *>(this)) == false)
		{
			ErrorLn(__FUNCTION__": GAMEDB_REQUEST_WAR_SAVE_GAME to DB failed");
			return false;
		}
		WAR_TRACELN(__FUNCTION__": save warId sucess nWarId = " << nWarId);
	}

	if (bIsEnd)
	{
		obuf2048 dbEndBuf;
		obuf dbEndData;
		log.warInfo.bIsEnd = true;

		log.nCount = 0;
		for (std::map<PDBID, DBREQ_PARAM_WAR_ACTOR_INFO>::iterator iter = m_mapWarActorLogEnd.begin(); iter != m_mapWarActorLogEnd.end(); ++iter)
		{
			DBREQ_PARAM_WAR_ACTOR_INFO detail;
			detail = iter->second;
			log.nCount++;
			dbEndData.push_back(&detail, sizeof(DBREQ_PARAM_WAR_ACTOR_INFO));
		}
		dbEndBuf.push_back(&log, sizeof(log));
		DWORD dwBytes = log.nCount* node_len;
		dbEndBuf.push_back(&dwBytes, sizeof(dwBytes));
		dbEndBuf.push_back(dbEndData.data(), dbEndData.size());

		if (pDBEngine->executeSP(GAMEDB_REQUEST_WAR_SAVE_GAME, nWarId, dbEndBuf.data(), dbEndBuf.size(), static_cast<IDBRetSink *>(this)) == false)
		{
			ErrorLn(__FUNCTION__": GAMEDB_REQUEST_WAR_SAVE_GAME to DB failed");
			return false;
		}
		WAR_TRACELN(__FUNCTION__": save war record table, log.nCount = " << log.nCount << ", warID = " << log.warInfo.nWarID);
	}

	return true;
}

void CWarLogRecord::recordAddExpOrTalentLog(bool bForceSave)
{
	if (m_pWar == NULL)
	{
		return;
	}

	IOSSLogService * pOSSLogServer = gServerGlobal->getOSSLogService();
	if (pOSSLogServer == NULL)
	{
		ErrorLn(__FUNCTION__": pOSSLogServer = NULL");
		return;
	}

	SEntityXml_Server* pEntityXml = g_EHelper.getEntityXml();
	if (pEntityXml == NULL)
		return;

	if (!bForceSave)
	{
		int nSaveLogInterval = pEntityXml->nAddExpOrTalentLogTimeInterval;
		// 没有配置将默认1分钟记录一次
		if (nSaveLogInterval == 0)
			nSaveLogInterval = 60 * 1000;

		DWORD dwCurrentTick = getTickCount();
		if (dwCurrentTick < m_dwLastSaveLogTick + nSaveLogInterval)
			return;

		// 上次保存日记时间
		m_dwLastSaveLogTick = dwCurrentTick;
	}

	IWar* pWarBase = (IWar*)m_pWar;

	bool bShowPlanLog = pEntityXml->nIsShowPlanLog > 0 ? true : false;
	ostrbuf logBuf;
	logBuf.reserve(512);

	map<PDBID, SHeroAddExpOrTalentLog>::iterator it = m_mapHeroAddExpOrTalentLog.begin();
	for (; it != m_mapHeroAddExpOrTalentLog.end(); ++it)
	{
		logBuf.clear();
		PDBID uPDBID = it->first;
		SWPerson* pPerson = pWarBase->getPlayerInfoByPDBIU(uPDBID);
		// 英雄文字日记不用记录电脑
		if (!pPerson || pPerson->bIsComputer)
			continue;

		int nCamp = pPerson->nCamp;

		logBuf << (int)uPDBID << ";";
		logBuf << pWarBase->getWarDBKey() << ";";
		logBuf << nCamp << ";";
		// 组织个人EXP日记数据
		const SHeroAddExpOrTalentLog& sData = it->second;
		for (int i = 0; i < EEXPDROPTYPE_MAX; ++i)
		{
			const int & nCurExp = sData.nAddExp[i];
			logBuf << nCurExp << ";";
		}

		// 组织个人天赋日记数据
		for (int i = 0; i < ETALENTDROPTYPE_MAX; ++i)
		{
			const int & nCurTalent = sData.nAddTalent[i];
			logBuf << nCurTalent << ";";
		}

		// 写入角色文字日记
		pOSSLogServer->addGameNoteLog(pPerson->nSrcWorldID, OSS_NOTE_TRACK_WAR, uPDBID, logBuf.c_str());

		if (bShowPlanLog)
		{
			WarningLn(__FUNCTION__": Planning requirements : " << logBuf.c_str());
		}
	}
	m_mapHeroAddExpOrTalentLog.clear();
}

void CWarLogRecord::setAddExpToMap(PDBID pDbid, int nAddExp, EExpDropType type)
{
	if (m_pWar == NULL)
	{
		return;
	}

	map<PDBID, SHeroAddExpOrTalentLog>::iterator it = m_mapHeroAddExpOrTalentLog.find(pDbid);
	if (it != m_mapHeroAddExpOrTalentLog.end())
	{
		it->second.nAddExp[type] += nAddExp;
	}
	else
	{
		SHeroAddExpOrTalentLog sLog;
		sLog.nAddExp[type] = nAddExp;
		m_mapHeroAddExpOrTalentLog[pDbid] = sLog;
	}

	recordAddExpOrTalentLog(false);
}

void CWarLogRecord::setAddTalentToMap(PDBID pDbid, int nAddTalent, ETalentDropType type)
{
	if (m_pWar == NULL)
	{
		return;
	}

	if (type < 0 || type >= ETALENTDROPTYPE_MAX)
	{
		return;
	}

	map<PDBID, SHeroAddExpOrTalentLog>::iterator it = m_mapHeroAddExpOrTalentLog.find(pDbid);
	if (it != m_mapHeroAddExpOrTalentLog.end())
	{
		it->second.nAddTalent[type] += nAddTalent;
	}
	else
	{
		SHeroAddExpOrTalentLog sLog;
		sLog.nAddTalent[type] = nAddTalent;
		m_mapHeroAddExpOrTalentLog[pDbid] = sLog;
	}

	recordAddExpOrTalentLog(false);
}

void CWarLogRecord::saveHeroSkinUseCount()
{
	if (m_pWar == NULL)
		return;

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": pDBEngine == NULL");
		return;
	}

	UID uidList[64] = { 0 };
	int nCount = m_pWar->getAllPerson(uidList, sizeof(uidList));
	for (int i = 0; i < nCount; ++i)
	{
		SWPerson* itPerson = m_pWar->getPlayerInfo(uidList[i]);
		if (itPerson == NULL)
			continue;

		if (itPerson->bIsComputer)
			continue;

		DBREQ_ADD_HEROSKIN_USECOUNT dbParam;
		dbParam.dwPDBID = itPerson->uPDBID;
		dbParam.wHeroID = itPerson->nHeroID;
		dbParam.wHeroSkinID = itPerson->nHeroSkinID;
		dbParam.nAddUseCount = 1;

		if (pDBEngine->executeSP(GAMEDB_REQUEST_ADD_HEROSKIN_USECOUNT, m_pWar->getWarID(), (LPCSTR)&dbParam, sizeof(dbParam), static_cast<IDBRetSink *>(this)) == false)
		{
			ErrorLn(__FUNCTION__": GAMEDB_REQUEST_ADD_HEROSKIN_USECOUNT to DB failed");
			continue;
		}
	}
}

void CWarLogRecord::saveBanHeroListToDB()
{
	if (m_pWar == NULL)
		return;

	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return;
	}

	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": pDBEngine == NULL");
		return;
	}

	IWar* pWarBase = (IWar*)m_pWar;
	DBREQ_PARAM_WAR_SAVE_BAN_HERO dbParam;
	dbParam.nWarID = pWarBase->getWarDBKey();
	dbParam.wWarType = pWarBase->getMatchTypeID();		// 比赛类型
	dbParam.dwBeginTime = pWarBase->getWarInfo()->BeginTime;
	dbParam.dwEndTime = time(NULL);
	dbParam.bIsFull = pWarBase->getWarInfo()->isPlayerFull;
	dbParam.bIsHasComputer = warMiscMgr->isHaveComputer();
	dbParam.dwAvgRankScore = pWarBase->getWarCreateContext()->dwAvgRankScore;
	warMiscMgr->getAllBanHero(dbParam.wBanHeroList, DBDEF_BAN_HERO_TOTAL_SIZE);

	if (pDBEngine->executeSP(GAMEDB_REQUEST_WAR_SAVE_BAN_HERO, pWarBase->getWarID(), (LPCSTR)&dbParam, sizeof(dbParam), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__": GAMEDB_REQUEST_WAR_SAVE_BAN_HERO to DB failed");
		return;
	}
}

void CWarLogRecord::saveMatchTypeRankScoreAndGrade()
{
	if (m_pWar == NULL)
		return;

	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return;
	}

	ISchemeEntity *pSchemeEntity = gServerGlobal->getSchemeCenter()->getSchemeEntity();
	if (pSchemeEntity == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeEntity == NULL");
		return;
	}
	// 取得显示打印信息
	int nChangeRankInfo = pSchemeEntity->getEntityXml()->nChangeRankInfo;

	DWORD dwAvgScore_Blue = warMiscMgr->getCampMatchTypeAverageScore(CAMP_BLUE);
	DWORD dwAvgScore_Red = warMiscMgr->getCampMatchTypeAverageScore(CAMP_RED);

	UID uidList[64] = { 0 };
	int nCount = m_pWar->getAllPerson(uidList, sizeof(uidList));
	for (int i = 0; i < nCount; ++i)
	{
		SWPerson* itPerson = m_pWar->getPlayerInfo(uidList[i]);
		if (itPerson == NULL)
			continue;

		// 记录到日志数据库
		DBREQ_SET_MATCHTYPE_RANK_DETAIL DBData;
		DBData.dwWarID = m_pWar->getWarID();                           // 战场ID 
		DBData.dwWordID = itPerson->nSrcWorldID;                       // 世界ID
		DBData.dwActorID = itPerson->uPDBID;                           // 角色ID 
		DBData.byMatchType = m_pWar->getMatchTypeID();                 // 比赛类型 
		DBData.dwPreMatchTypeRank = itPerson->nPreMatchTypeScore;	   // 原天梯分
		DBData.nGetMatchTypeRank = itPerson->nGetMatchTypeScore;          // 添加或者减少天梯分
		DBData.dwSelfAvgMatchTypeRank = itPerson->nCamp == CAMP_BLUE ? dwAvgScore_Blue : dwAvgScore_Red; // 友方平均天梯分 
		DBData.dwEnemyAvgMatchTypeRank = itPerson->nCamp == CAMP_BLUE ? dwAvgScore_Red : dwAvgScore_Blue; ;	//地方平均天梯分															// 敌方平均天梯分 
		DBData.byEndWarReason = m_pWar->getWarEndReason();                // 结束原因

		if (nChangeRankInfo != 0)
		{
			TraceLn("dwWarID = " << DBData.dwWarID << ", dwWordID = " << DBData.dwWordID << ", dwActorID =" << DBData.dwActorID << ", name = " << itPerson->szName
				<< ", PreMatchTypeRank =" << DBData.dwPreMatchTypeRank << ", nGetMatchTypeRank =" << DBData.nGetMatchTypeRank << ", SelfAvgMatchTypeRank ="
				<< DBData.dwSelfAvgMatchTypeRank << ",EnemyAvgMatchTypeRank=" << DBData.dwEnemyAvgMatchTypeRank);
		}

		IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
		if (pDBEngine == NULL)
		{
			ErrorLn(__FUNCTION__": pDBEngine == NULL");
			return;
		}

		if (pDBEngine->executeSP(GAMEDB_REQUEST_SET_MATCHTYPE_RANK_DETAIL, m_pWar->getWarID(), (LPCSTR)&DBData, sizeof(DBData), static_cast<IDBRetSink *>(this)) == false)
		{
			ErrorLn(__FUNCTION__": GAMEDB_REQUEST_SET_MATCHTYPE_RANK_DETAIL to DB failed");
			return;
		}

		IOSSLogService* pOSSLogService = gServerGlobal->getOSSLogService();
		if (pOSSLogService != NULL)
		{
			pOSSLogService->addNumericLog(itPerson->nSrcWorldID, OSS_OPT_TYPE_PERSON, itPerson->uPDBID, m_pWar->getWarDBKey(), itPerson->nGetMatchTypeScore,
				OSS_NUMERIC_MAP_ORDER, OSS_RESOURCE_OPT_ADD, OSSRES_ADD_WAR_PRIZE, time(NULL), a2utf8("战场增加排位分"), m_pWar->getMatchTypeID());
		}
	}

}


void CWarLogRecord::recordHeroWinOrDefeat()
{
	IWarMiscManager* warMiscMgr = m_pWar->getMiscManager();
	if (warMiscMgr == NULL)
	{
		ErrorLn(__FUNCTION__":warMiscMgr == NULL");
		return;
	}

	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL");
		return;
	}
	ISchemePersonModel* pPersonModel = pSchemeCenter->getSchemePersonModel();
	if (pPersonModel == NULL)
	{
		ErrorLn(__FUNCTION__": pPersonModel == NULL");
		return;
	}
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
	{
		ErrorLn(__FUNCTION__": pDBEngine == NULL");
		return;
	}

	int nMatchType = m_pWar->getMatchTypeID();
	UID uidList[64] = { 0 };
	int nCount = m_pWar->getAllPerson(uidList, sizeof(uidList));
	for (int i = 0; i < nCount; ++i)
	{
		SWPerson* person = m_pWar->getPlayerInfo(uidList[i]);
		if (person == NULL)
			continue;

		// 英雄类型排行榜 排除掉电脑
		if (person->bIsComputer)
			continue;

		int nHeroID = person->nHeroID;

		DBREQ_PARAM_UPDATE_HEROTYPE_INFO DBData;
		DBData.bWin = (m_pWar->getWinCamp() == person->nCamp);
		DBData.byMatchType = (BYTE)nMatchType;
		DBData.nHeroID = nHeroID;
		DBData.byHeroType = pPersonModel->GetHeroType(nHeroID);
		DBData.bHaveComputer = warMiscMgr->isHaveComputer();

		ActorHelper actorHelper(person->uPDBID, ACTOR_DBID);
		IActorService *pActorService = actorHelper.m_ptr;
		if (pActorService != NULL)
		{
			DBData.nRankScore = pActorService->getMatchTypeRank((EMMatchType)nMatchType);
			DBData.nRankGrade = pActorService->getMatchTypeRankGrade((EMMatchType)nMatchType);
		}

		if (pDBEngine->executeSP(GAMEDB_UPDATE_HEROTYPE_ORDER_CLASS, m_pWar->getWarID(), (LPCSTR)&DBData, sizeof(DBData), static_cast<IDBRetSink *>(this)) == false)
		{
			ErrorLn(__FUNCTION__": GAMEDB_UPDATE_HEROTYPE_ORDER_CLASS to DB failed");
		}
	}
}