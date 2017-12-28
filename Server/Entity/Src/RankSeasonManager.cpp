#include "stdafx.h"
#include "RankSeasonManager.h"
#include "IServerGlobal.h"
#include "Event_ManagedDef.h"
#include "EventDef.h"
#include "ISchemeCenter.h"
#include "PlayerBankPart.h"
#include "Match_Manageddef.h"
#include "MailHelper.h"
#include "OssLogDef.h"
#include "IOSSLogService.h"

CRankSeasonManager::CRankSeasonManager()
{
	m_pMaster = NULL;
	m_pBankPart = NULL;
	m_mapSeasonDetailHero.clear();
	m_vecSeasonRecord.clear();
}

CRankSeasonManager::~CRankSeasonManager()
{

}

void CRankSeasonManager::OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
	if (nDBRetCode != DBRET_CODE_SUCCEED)
	{
		ErrorLn(__FUNCTION__<< "nCmdID = " << nCmdID << ", nDBRetCode = " << nDBRetCode << ", Desc = " << pszDBRetDesc);
		return;
	}

	switch (nCmdID)
	{
	case GAMEDB_REQUEST_GET_RANK_SEASON_DETAIL:
		{
			onDBReturnSeasonDetail(nQueueIndex, pOutData, nOutLen);
		}
		break;
	case GAMEDB_REQUEST_GET_RANK_SEASON_RECORD:
		{
 			onDBReturnSeasonRecord(nQueueIndex, pOutData, nOutLen);
		}
		break;
	case GAMEDB_REQUEST_SET_RANK_SEASON_DETAIL:
		{

		}
		break;
	case GAMEDB_REQUEST_ADD_RANK_SEASON_RECORD:
		{

		}
		break;
	default:
		break;
	}
}

void CRankSeasonManager::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (MAKE_EVENT_KEY(bSrcType, wEventID))
	{
	case MAKE_EVENT_KEY(SOURCE_TYPE_SYSTEM, EVENT_MATCH_RANK_SEASON_BEGIN):
		{
			if (pszContext == NULL || nLen != sizeof(event_match_season_begin))
			{
				ErrorLn(__FUNCTION__" failed ! pszContext == NULL || nLen != sizeof(event_match_season_begin)");
				return;
			}
			event_match_season_begin* pEvent = (event_match_season_begin*)pszContext;

			dealSeasonBegin(pEvent->nBeginSeasonIndex);
		}
		break;
	case MAKE_EVENT_KEY(SOURCE_TYPE_SYSTEM, EVENT_MATCH_RANK_SEASON_END):
		{
			if (pszContext == NULL || nLen != sizeof(event_match_season_end))
			{
				ErrorLn(__FUNCTION__" failed ! pszContext == NULL || nLen != sizeof(event_match_season_end)");
				return;
			}
			event_match_season_end* pEvent = (event_match_season_end*)pszContext;

			dealSeasonEnd(pEvent->nEndSeasonIndex);
		}
		break;
	default:
		break;
	}
}

bool CRankSeasonManager::onSerialize(ibuffer& in)
{
	// 当前赛季数据
	if (!in.good())
		return false;
	in >> m_seasonDetail;

	// 当前赛季英雄数据
	m_mapSeasonDetailHero.clear();
	for (int i = 0; i < m_seasonDetail.wHeroUseRecordCount; ++i)
	{
		if (!in.good())
			return false;

		SRankSeasonDetailHero sHero;
		in >> sHero;
		m_mapSeasonDetailHero[sHero.wHeroID] = sHero;
	}

	// 赛季历史数据
	if (!in.good())
		return false;

	int nRecordCount = 0;
	in >> nRecordCount;

	m_vecSeasonRecord.clear();
	for (int i = 0; i < nRecordCount; ++i)
	{
		if (!in.good())
			return false;

		SRankSeasonRecord sRecord;
		in >> sRecord;
		m_vecSeasonRecord.push_back(sRecord);
	}

	return true;
}

bool CRankSeasonManager::deSerialize(obuf& out)
{
	// 当前赛季数据
	out.push_back(&m_seasonDetail, sizeof(m_seasonDetail));

	// 当前赛季英雄数据
	unordered_map<WORD, SRankSeasonDetailHero>::iterator itHero = m_mapSeasonDetailHero.begin(); 
	for (; itHero !=  m_mapSeasonDetailHero.end(); ++itHero)
	{
		out.push_back(&(itHero->second), sizeof(itHero->second));
	}

	// 赛季历史数据
	int nRecordCount = (int)m_vecSeasonRecord.size();
	out.push_back(&nRecordCount, sizeof(nRecordCount));

	vector<SRankSeasonRecord>::iterator itRecord = m_vecSeasonRecord.begin();
	for (; itRecord != m_vecSeasonRecord.end(); ++itRecord)
	{
		out.push_back(&(*itRecord), sizeof(*itRecord));
	}

	return true;
}

bool CRankSeasonManager::init(CPlayerBankPart * pBankPart, __IEntity * pMaster, int nLoginMode)
{
	if (pBankPart == NULL || pMaster == NULL)
		return false;
	
	m_pBankPart = pBankPart;
	m_pMaster = pMaster;

	IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
	if (pEventEngine)
	{
		pEventEngine->Subscibe(this, EVENT_MATCH_RANK_SEASON_BEGIN, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);
		pEventEngine->Subscibe(this, EVENT_MATCH_RANK_SEASON_END, SOURCE_TYPE_SYSTEM, 0, __FUNCTION__);
	}

	gServerGlobal->getDBEngineService()->registerHandler(this, __FUNCTION__);

	DWORD nActorID = m_pMaster->getIntProperty(PROPERTY_ID);
	if (nLoginMode == elogin_online && !IsComputerPDBID(nActorID))
	{
		readSeasonInfoFromDB();
	}

	return true;
}

void CRankSeasonManager::restore()
{
    IEventEngine * pEventEngine = gServerGlobal->getEventEngine();
    if (pEventEngine)
    {
        pEventEngine->UnSubscibe(this, EVENT_MATCH_RANK_SEASON_BEGIN, SOURCE_TYPE_SYSTEM, 0);
        pEventEngine->UnSubscibe(this, EVENT_MATCH_RANK_SEASON_END, SOURCE_TYPE_SYSTEM, 0);
    }

    gServerGlobal->getDBEngineService()->unregisterHandler(this);
}

void CRankSeasonManager::release()
{

}

void CRankSeasonManager::setMatchRankSeasonInfo(SRankSeasonWarEndInfo pInfo)
{
	m_seasonDetail.dwBeginTime = pInfo.dwWarEndTime;
	m_seasonDetail.dwEndTime = pInfo.dwWarEndTime;
	m_seasonDetail.wMatchCount += 1;
	m_seasonDetail.wMvpCount += pInfo.wMvpCount;
	m_seasonDetail.wSuperGodCount += pInfo.wSuperGodCount;
	m_seasonDetail.wKill5Count += pInfo.wKill5Count;
	m_seasonDetail.wKill4Count += pInfo.wKill4Count;
	m_seasonDetail.wKill3Count += pInfo.wKill3Count;

	// 最大连胜
	if (pInfo.bWin)
	{
		m_seasonDetail.wWinCount += 1;
		m_seasonDetail.wTempContinueWinCount += 1;
		if (m_seasonDetail.wTempContinueWinCount > m_seasonDetail.wContinueWinCount)
		{
			m_seasonDetail.wContinueWinCount = m_seasonDetail.wTempContinueWinCount;
		}
	}
	else
	{
		m_seasonDetail.wTempContinueWinCount = 0;
	}
	
	// 最高段位
	int nRankGrade = m_pBankPart->getMatchTypeRankGrade(MatchType_Rank);
	m_seasonDetail.wTopRankGrade = nRankGrade > m_seasonDetail.wTopRankGrade ? nRankGrade : m_seasonDetail.wTopRankGrade;

	// 使用英雄
	SRankSeasonDetailHero& sHero = m_mapSeasonDetailHero[pInfo.wHeroID];
	sHero.wHeroID = pInfo.wHeroID;
	sHero.wMatchCount += 1;
	sHero.wWinCount += pInfo.bWin ? 1 : 0;
	m_seasonDetail.wHeroUseRecordCount = m_mapSeasonDetailHero.size();

	updateSeasonDetailToDB();
}

void CRankSeasonManager::onMessageRequestSeasonDetail()
{
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return;
	ISchemeMatchSeasonTime * pSchemeSeasonTime = pSchemeCenter->getSchemeMatchSeasonTime();
	ISchemeMatchSeasonPrize * pSchemeSeasonPrize = pSchemeCenter->getSchemeMatchSeasonPrize();
	if (pSchemeSeasonTime == NULL || pSchemeSeasonPrize == NULL)
		return;

	SSchemeMatchSeasonTime * pSchemeTime =  pSchemeSeasonTime->getSchemeMatchSeasonTimeByTime(time(NULL));

	msg_entity_season_detail detail;
	detail.wTopRankGrade = m_seasonDetail.wTopRankGrade;
	detail.wMatchCount = m_seasonDetail.wMatchCount;
	detail.wWinCount = m_seasonDetail.wWinCount;
	detail.wContinueWinCount = m_seasonDetail.wContinueWinCount;
	detail.wMvpCount = m_seasonDetail.wMvpCount;
	detail.wSuperGodCount = m_seasonDetail.wSuperGodCount;
	detail.wKill5Count = m_seasonDetail.wKill5Count;
	detail.wKill4Count = m_seasonDetail.wKill4Count;
	detail.wKill3Count = m_seasonDetail.wKill3Count;
	
	if (pSchemeTime != NULL	)
	{
		detail.dwEndTime = pSchemeTime->tEndTime;
		detail.wPrizeConfigID = pSchemeTime->nPrizeConfigID;
	}

	__IPlayerRole *pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
	if (pPlayerRole)
	{
		pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_RET_SEASON_DETAIL, &detail, sizeof(detail));
	}
}

// 客户端请求赛季详细数据
void CRankSeasonManager::onMessageRequestSeasonRecord()
{
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return;
	ISchemeMatchSeasonTime * pSchemeSeasonTime = pSchemeCenter->getSchemeMatchSeasonTime();
	if (pSchemeSeasonTime == NULL)
		return;

	obuf obData;

	int nRecordCount = m_vecSeasonRecord.size();
	obData << nRecordCount;

	for (int i = 0; i < nRecordCount; ++i)
	{
		msg_entity_season_record_node node;
		node.wSeasonID = m_vecSeasonRecord[i].wSeasonID;
		node.wRankGrade = m_vecSeasonRecord[i].wRankGrade;
		node.wMatchCount = m_vecSeasonRecord[i].wMatchCount;
		node.wWinCount = m_vecSeasonRecord[i].wWinCount;
		memcpy(node.wHeroRecord, m_vecSeasonRecord[i].wHeroRecord, sizeof(node.wHeroRecord));

		SSchemeMatchSeasonTime * pSchemeSeason = pSchemeSeasonTime->getSchemeMatchSeasonTimeByIndex(m_vecSeasonRecord[i].wSeasonID);
		if (pSchemeSeason != NULL)
		{
			memcpy(node.szSeasonTitle, pSchemeSeason->szName, sizeof(node.szSeasonTitle));
			node.dwSeasonBeginTime = pSchemeSeason->tBeginTime;
			node.dwSeasonEndTime = pSchemeSeason->tEndTime;
		}

		obData << node;
	}

	__IPlayerRole *pPlayerRole = CAST_TYPE(__IPlayerRole*, m_pMaster);
	if (pPlayerRole)
	{
		pPlayerRole->sendMessageToClient(PART_BANK, SC_MSG_ENTITY_RET_SEASON_RECORD, obData.data(), obData.size());
	}
}

void CRankSeasonManager::checkSeasonTime()
{
	// 1.先处理上赛季的结束，再处理本赛季的开始，顺序不能乱，
	// 2.因为赛季结束发奖依赖赛季成绩，而赛季成绩本赛季开始会清除

	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return;
	ISchemeMatchSeasonTime * pSchemeSeasonTime = pSchemeCenter->getSchemeMatchSeasonTime();
	if (pSchemeSeasonTime == NULL)
		return;

	// 检测上次赛季结束
	SSchemeMatchSeasonTime * pSchemeLastSeasonByEndTime = pSchemeSeasonTime->getSchemeMatchSeasonTimeByLastEndTime();
	if (pSchemeLastSeasonByEndTime != NULL)
	{
		if (m_seasonDetail.dwEndTime > 0 && m_seasonDetail.dwEndTime < pSchemeLastSeasonByEndTime->tEndTime)
		{
			dealSeasonEnd(pSchemeLastSeasonByEndTime->nIndex);
		}
	}

	// 检测本次赛季开始
	// 1.检测是否玩过排位，从第一场排位赛开始记赛季数据
	if (checkHavePlayRank() == false)
		return;
	// 2.从开始记录赛季一直重置赛季开始数据(玩过排位，m_seasonDetail.wRecordSeasonID, m_seasonDetail.dwBeginTime这两个数据不可能都为0)
	int nNeedRelfushSeason = 0;
	if (m_seasonDetail.wReflushSeasonID > 0)
	{
		nNeedRelfushSeason = m_seasonDetail.wReflushSeasonID + 1;
	}
	else if (m_seasonDetail.dwBeginTime > 0)
	{
		SSchemeMatchSeasonTime * pSchemeSeason = pSchemeSeasonTime->getSchemeMatchSeasonTimeByTime(m_seasonDetail.dwBeginTime);
		if (pSchemeSeason != NULL)
		{
			nNeedRelfushSeason = pSchemeSeason->nIndex + 1;
		}
	}
	// 3.重置本赛季数据，记录上赛季数据（所以至少从第2赛季开始执行）
	if (nNeedRelfushSeason <= 0 || nNeedRelfushSeason - 1 <= 0)
		return;

	// 4.多赛季未登陆，每赛季开始都要进行分数的重置与记录
	SSchemeMatchSeasonTime * pSchemeLastSeasonByBeginTime = pSchemeSeasonTime->getSchemeMatchSeasonTimeByLastBeginTime();
	if (pSchemeLastSeasonByBeginTime == NULL)
		return;
	for (int i = nNeedRelfushSeason; i <= pSchemeLastSeasonByBeginTime->nIndex; ++i)
	{
		dealSeasonBegin(i);
	}
}

void CRankSeasonManager::dealSeasonBegin(int nBeginSeasonIndex)
{
	// 1.记录上赛季数据 
	// 2.重置本赛季数据 
	if (nBeginSeasonIndex <= 0 || nBeginSeasonIndex - 1 <= 0)
		return;

	// 当前段位
	int nRankScore = m_pBankPart->getMatchTypeRank(MatchType_Rank);
	int nRankGrade = m_pBankPart->getMatchTypeRankGrade(MatchType_Rank);
    int nHideRankScore = m_pBankPart->getMatchTypeHideRank(MatchType_Rank);

	// 记录
	SRankSeasonRecord record;
	record.dwActorID = m_seasonDetail.dwActorID;
	record.wSeasonID = nBeginSeasonIndex - 1;			//前一赛季
	record.wRankGrade = nRankGrade;						//当前段位	
	record.wMatchCount = m_seasonDetail.wMatchCount;
	record.wWinCount = m_seasonDetail.wWinCount;
	getSeasonMostUseHero(record.wHeroRecord, sizeof(record.wHeroRecord) / sizeof(WORD));
	m_vecSeasonRecord.push_back(record);				//存缓存
	addSeasonRecordToDB(record);						//存DB

	// 重置
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return;
	ISchemeMatchSeasonPrize * pSchemeSeasonPrize = pSchemeCenter->getSchemeMatchSeasonPrize();
	if (pSchemeSeasonPrize == NULL)
		return;
	SSchemeMatchSeasonPrize * pPrize = pSchemeSeasonPrize->getSchemeMatchSeasonPrize(nBeginSeasonIndex, nRankGrade);//本赛季，当前段位	
	if (pPrize == NULL)
		return;

	m_pBankPart->setMatchTypeRank(MatchType_Rank, abs(pPrize->nAccountScore), nHideRankScore);	// 该接口会进行基础分判定
	// 记录文字日志
	IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
	if (NULL != pOSSLogService)
	{
		char szText[512] = {0};
		DWORD dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);
		sprintf_s(szText, sizeof(szText), a2utf8("(%s)玩家%s(%d)段位%s(%d)从%d分降为%d分"), pPrize->szSeasonDesc, m_pMaster->getName(), dwActorID, pPrize->szRankDesc, nRankGrade, nRankScore, pPrize->nAccountScore);
		pOSSLogService->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_RANK, dwActorID, szText);
	}

	m_seasonDetail.dwBeginTime = 0;
	m_seasonDetail.wReflushSeasonID = nBeginSeasonIndex;
	m_seasonDetail.wTopRankGrade = 0;
	m_seasonDetail.wMatchCount = 0;
	m_seasonDetail.wWinCount = 0;
	m_seasonDetail.wContinueWinCount = 0;
	m_seasonDetail.wTempContinueWinCount = 0;
	m_seasonDetail.wMvpCount = 0;
	m_seasonDetail.wSuperGodCount = 0;
	m_seasonDetail.wKill5Count = 0;
	m_seasonDetail.wKill4Count = 0;
	m_seasonDetail.wKill3Count = 0;
	m_seasonDetail.wHeroUseRecordCount = 0;
	m_mapSeasonDetailHero.clear();
	updateSeasonDetailToDB();
}

void CRankSeasonManager::dealSeasonEnd(int nEndSeasonIndex)
{
	// 1.发送赛季奖励

	if (m_seasonDetail.dwEndTime <= 0)
	{
		return;
	}

	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return;

	ISchemeMatchSeasonTime * pSchemeSeasonTime = pSchemeCenter->getSchemeMatchSeasonTime();
	ISchemeMatchSeasonPrize * pSchemeSeasonPrize = pSchemeCenter->getSchemeMatchSeasonPrize();
	if (pSchemeSeasonTime == NULL || pSchemeSeasonPrize == NULL)
		return;

	// 发奖励
	do
	{
		SSchemeMatchSeasonTime * pSchemeSeasonByEndTime =  pSchemeSeasonTime->getSchemeMatchSeasonTimeByTime((time_t)m_seasonDetail.dwEndTime);
		if (pSchemeSeasonByEndTime == NULL)
			break;

		// 按赛季最高段位奖励
		int nPrizeConfigID = pSchemeSeasonByEndTime->nPrizeConfigID;
		int nTopRankGrade = m_seasonDetail.wTopRankGrade;
		SSchemeMatchSeasonPrize * pSchemePrize = pSchemeSeasonPrize->getSchemeMatchSeasonPrize(nPrizeConfigID, nTopRankGrade);
		if(pSchemePrize == NULL)
			break;

		sendSeasonPrizeMail(pSchemePrize);
	}
	while (false);

	// 刷新数据
	m_seasonDetail.dwEndTime = 0;
	updateSeasonDetailToDB();
}

void CRankSeasonManager::sendSeasonPrizeMail(SSchemeMatchSeasonPrize * pPrize)
{	
	// 奖励
	if (pPrize == NULL)
	{
		return;
	}

	vector<int> vecPrize;
	for (int i = 0; i < MATCH_SEASON_PRIZE_COUNT; ++i)
	{
		if (pPrize->arrNormalPrize[i] <= 0)
			continue;
		vecPrize.push_back(pPrize->arrNormalPrize[i]);
	}
	// 最高段位达到配置，胜场达到配置
	if (m_seasonDetail.wTopRankGrade >= pPrize->arrExtraCondition[0] && m_seasonDetail.wWinCount >= pPrize->arrExtraCondition[1])
	{
		for (int i = 0; i < MATCH_SEASON_PRIZE_COUNT; ++i)
		{
			if (pPrize->arrExtraPrize[i] <= 0)
				continue;
			vecPrize.push_back(pPrize->arrExtraPrize[i]);
		}
	}

	int nCount = vecPrize.size();
	if (nCount <= 0)
	{
		return;
	}

	// 发邮件
	MailHelper mailHelper;
	IMailSceneService *pMailSceneService = mailHelper.m_ptr;
	if (pMailSceneService == NULL)
		return;

	SMailSendData sendData;
	sendData.nType = emMailType_System;
	sendData.nSourceType = emMailSourceType_System;
	sendData.dwSenderPDBID = 0;
	sendData.dwMasterPDBID = m_pMaster->getIntProperty(PROPERTY_ID);

	sendData.nPlusGoodsNum = nCount;						// 奖励
	for (int i = 0; i < nCount; ++i)
	{
		sendData.sGoodsList[i].dwGoodsID = vecPrize[i];
		sendData.sGoodsList[i].nCount = 1;
	}

	char  szParam[MAIL_CONTENT_MAXSIZE];					// 内容
	ssprintf_s(szParam, sizeof(szParam), "%s;%s", pPrize->szSeasonDesc, pPrize->szRankDesc);

	pMailSceneService->sendMail(sendData, emMailFill_Rank_SeasonPrize, szParam);
}

bool CRankSeasonManager::checkHavePlayRank()
{
	int nHideRankGrade = m_pBankPart->getMatchTypeHideRank(MatchType_Rank);
	if (nHideRankGrade <= 0)
		return false;
	else
		return true;
}

bool DecSortFunc( SRankSeasonDetailHero &v1, SRankSeasonDetailHero &v2)
{
	// 排序函数(降序排列)
	return v1.wMatchCount > v2.wMatchCount;
}  

void CRankSeasonManager::getSeasonMostUseHero(WORD * arrHero, WORD nlen)
{
	vector<SRankSeasonDetailHero> vecHero(m_mapSeasonDetailHero.size());
	unordered_map<WORD, SRankSeasonDetailHero>::iterator it = m_mapSeasonDetailHero.begin();
	for (; it != m_mapSeasonDetailHero.end(); ++it)
	{
		vecHero.push_back(it->second);
	}

	std::sort(vecHero.begin(), vecHero.end(), DecSortFunc);

	for (int i = 0; i < vecHero.size() && 3 * i < nlen; ++i)
	{
		arrHero[3*i]		= vecHero[i].wHeroID;
		arrHero[3*i + 1]	= vecHero[i].wMatchCount;
		arrHero[3*i + 2]	= vecHero[i].wWinCount;
	}
}

void CRankSeasonManager::updateSeasonDetailToDB()
{
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
		return;

	obuf oBData;

	DBREQ_PARAM_SET_RANK_SEASON_DETAIL detailInfo;
	memcpy(&detailInfo, &m_seasonDetail, sizeof(detailInfo));
	oBData << detailInfo;

	int nHeroBuffLen = m_mapSeasonDetailHero.size() * 3 * sizeof(WORD);
	oBData << nHeroBuffLen;

	unordered_map<WORD, SRankSeasonDetailHero>::iterator it = m_mapSeasonDetailHero.begin();
	for (it; it != m_mapSeasonDetailHero.end(); ++it)
	{
		oBData << it->second.wHeroID << it->second.wMatchCount << it->second.wWinCount;
	}

	pDBEngine->executeSP(GAMEDB_REQUEST_SET_RANK_SEASON_DETAIL, m_seasonDetail.dwActorID, oBData.data(), oBData.size(), static_cast<IDBRetSink *>(this));
}

void CRankSeasonManager::addSeasonRecordToDB(SRankSeasonRecord& record)
{
	IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
		return;

	DBREQ_PARAM_ADD_RANK_SEASON_RECORD recordInfo;
	memcpy(&recordInfo, &record, sizeof(recordInfo));

	pDBEngine->executeSP(GAMEDB_REQUEST_ADD_RANK_SEASON_RECORD, m_seasonDetail.dwActorID, LPCSTR(&recordInfo), sizeof(recordInfo), static_cast<IDBRetSink *>(this));
}

void CRankSeasonManager::readSeasonInfoFromDB()
{
	IDBEngineService * pDBEngine = (IDBEngineService *)gServerGlobal->getDBEngineService();
	if (pDBEngine == NULL)
		return;
	
	DWORD dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);

	// 先取历史赛季数据，否则当前赛季新添加的记录会被历史赛季清除时擦掉

	// 历史赛季数据
	DBREQ_PARAM_GET_RANK_SEASON_RECORD paramRecord;
	paramRecord.dwActorID = dwActorID;
	if (pDBEngine->executeSP(GAMEDB_REQUEST_GET_RANK_SEASON_RECORD, dwActorID, (LPCSTR)&paramRecord, sizeof(paramRecord), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP() GAMEDB_REQUEST_GET_RANK_SEASON_RECORD failed");
		return;
	}

	// 当前赛季数据
	DBREQ_PARAM_GET_RANK_SEASON_DETAIL paramDetail;
	paramDetail.dwActorID = dwActorID;
	if (pDBEngine->executeSP(GAMEDB_REQUEST_GET_RANK_SEASON_DETAIL, dwActorID, (LPCSTR)&paramDetail, sizeof(paramDetail), static_cast<IDBRetSink *>(this)) == false)
	{
		ErrorLn(__FUNCTION__": pDBEngine->executeSP() GAMEDB_REQUEST_GET_RANK_SEASON_DETAIL failed");
		return;
	}
}

void CRankSeasonManager::onDBReturnSeasonDetail(int nQueueIndex, void * pOutData, int nOutLen)
{
	DWORD dwActorID = nQueueIndex;
	DWORD dwSelfID = m_pMaster->getIntProperty(PROPERTY_ID);
	if (dwActorID == INVALID_PDBID || dwActorID != dwSelfID)
	{
		ErrorLn(__FUNCTION__ << " Invalid ActorID");
		return;
	}

	if (pOutData == NULL)
	{
		ErrorLn(__FUNCTION__ << "failed: pOutData == NULL");
		return;
	}

	if (nOutLen == 0)		// 还未有数据存储
	{
		m_seasonDetail.dwActorID = dwActorID;
	}
	else
	{
		if (nOutLen < sizeof(DBREQ_RESULT_GET_RANK_SEASON_DETAIL))
		{
			ErrorLn(__FUNCTION__ << "failed: nOutLen < sizeof(DBREQ_RESULT_GET_RANK_SEASON_DETAIL)");
			return;
		}

		DBREQ_RESULT_GET_RANK_SEASON_DETAIL * pDetail = (DBREQ_RESULT_GET_RANK_SEASON_DETAIL *)pOutData;

		int * pBufLen = (int *)(pDetail + 1);

		if (nOutLen != sizeof(DBREQ_RESULT_GET_RANK_SEASON_DETAIL) + sizeof(int) + 3 * pDetail->wHeroUseRecordCount * sizeof(WORD))
		{
			ErrorLn(__FUNCTION__ << "failed: nOutLen != sizeof(DBREQ_RESULT_GET_RANK_SEASON_DETAIL) + 3 * pDatail->wHeroUseRecordCount * sizeof(WORD)");
			return;
		}

		// 当前赛季数据
		memcpy(&m_seasonDetail, pDetail, sizeof(m_seasonDetail));

		// 当前赛季英雄数据
		char * pDetailHero = (char *)(pBufLen + 1);
		m_mapSeasonDetailHero.clear();
		for (int i = 0; i < pDetail->wHeroUseRecordCount; ++i)
		{
			WORD * pHeroID		= (WORD *)(pDetailHero + i * 3 * sizeof(WORD));
			WORD * pMatchCount	= (WORD *)(pDetailHero + i * 3 * sizeof(WORD) + sizeof(WORD));
			WORD * pWinCount	= (WORD *)(pDetailHero + i * 3 * sizeof(WORD) + 2 * sizeof(WORD));

			SRankSeasonDetailHero hero;
			hero.wHeroID		= *pHeroID;
			hero.wMatchCount	= *pMatchCount;
			hero.wWinCount		= *pWinCount;
			m_mapSeasonDetailHero[hero.wHeroID] = hero;
		}
	}

	// 检测赛季时间
	checkSeasonTime();
}

void CRankSeasonManager::onDBReturnSeasonRecord(int nQueueIndex, void * pOutData, int nOutLen)
{
	DWORD dwActorID = nQueueIndex;
	DWORD dwSelfID = m_pMaster->getIntProperty(PROPERTY_ID);
	if (dwActorID == INVALID_PDBID || dwActorID != dwSelfID)
	{
		ErrorLn(__FUNCTION__ << " Invalid ActorID");
		return;
	}

	if (pOutData == NULL)
	{
		ErrorLn(__FUNCTION__ << "failed: pOutData == NULL");
		return;
	}

	if (nOutLen < sizeof(DBREQ_RESULT_GET_RANK_SEASON_RECORD))
	{
		ErrorLn(__FUNCTION__ << "failed: nOutLen < sizeof(DBREQ_RESULT_GET_RANK_SEASON_RECORD)");
		return;
	}

	DBREQ_RESULT_GET_RANK_SEASON_RECORD * pHead = (DBREQ_RESULT_GET_RANK_SEASON_RECORD *)pOutData;
	if (nOutLen != sizeof(DBREQ_RESULT_GET_RANK_SEASON_RECORD) + pHead->nRecordCount * sizeof(DBREQ_TABLE_RANK_SEASON_RECORD))
	{
		ErrorLn(__FUNCTION__ << "failed: nOutLen != sizeof(DBREQ_RESULT_GET_RANK_SEASON_RECORD) + pRecord->wRecordCount * sizeof(DBREQ_TABLE_RANK_SEASON_RECORD)");
		return;
	}

	// 赛季历史数据
	char * pDetailHero = (char *)(pHead + 1);
	m_vecSeasonRecord.clear();
	for (int i = 0; i < pHead->nRecordCount; ++i)
	{
		DBREQ_TABLE_RANK_SEASON_RECORD * pNode = (DBREQ_TABLE_RANK_SEASON_RECORD *)(pDetailHero + i * sizeof(DBREQ_TABLE_RANK_SEASON_RECORD));

		SRankSeasonRecord record;
		memcpy(&record, pNode, sizeof(record));
		m_vecSeasonRecord.push_back(record);
	}
}