#ifdef MOBA_ALTAR_WAR_OPEN

#include <math.h>
#include "MobaAltarWar.h"
#include "MonsBaseExec.h"
#include "MonsSoliderExec.h"
#include "WarMessageDef.h"
#include "EntityHelper.h"
#include "MatchSceneHelper.h"
#include "MatchSchemeDef.h"
#include "ISchemeCenter.h"
#include "IServerGlobal.h"
#include "OssLogDef.h"
#include "IProgressBarPart.h"
#include "MentorshipHelper.h"
#include "IAntiAddictServer.h"
#include "XPSkill.h"
#include "ITimerService.h"



//#define __WAR__HERO_GET_EXP__TALENT__DEBUG__
#ifndef __WAR__HERO_GET_EXP__TALENT__DEBUG__

#endif

CMobaAltarWar::CMobaAltarWar()
{
    m_ChargedMap.clear();
	m_vecAltarInfo.clear();
	m_vecWatchTowerInfo.clear();
}

CMobaAltarWar::~CMobaAltarWar()
{

}

// 初始化
bool CMobaAltarWar::create(IWarService* pWarService,SWarCreateContext& context)
{
	return CWar::create(pWarService, context);
}

// 初始化
bool CMobaAltarWar::Init()
{
	CWar::Start();
	CWar::Init();

    SceneLogicEventHelper eventHelper(m_nSceneID);
    rkt::IEventEngine* pEventEngine = eventHelper.m_ptr;
    if (pEventEngine != NULL)
    {
        // 订阅 场景创建成功场景逻辑服务加载完事件
		pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_WAR_REC_DAMAGE,  TYPE_PLAYER_ROLE, 0, __FUNCTION__);
		pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DAMAGE,         TYPE_PLAYER_ROLE, 0, __FUNCTION__);
		pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_CHANGE_STATE,   TYPE_PLAYER_ROLE, 0, __FUNCTION__);		
		pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_UPGRADE_LEVEL,   TYPE_PLAYER_ROLE, 0, __FUNCTION__);
		pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_LEARN_SPELL,    TYPE_PLAYER_ROLE, 0, __FUNCTION__);
		pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_FREEZE_CHANGED, TYPE_PLAYER_ROLE, 0, __FUNCTION__);
		pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_TALENT_CHANGE,   TYPE_PLAYER_ROLE, 0, __FUNCTION__);
		pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_PREPARE_CURE,   TYPE_PLAYER_ROLE, 0, __FUNCTION__);
        pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_WAR_GOOD_LIST,   TYPE_PLAYER_ROLE, 0, __FUNCTION__);
        pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_CLEAR_MULTI_KILL,      TYPE_PLAYER_ROLE, 0, __FUNCTION__);
        pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_THRILLINGCURE,         TYPE_PLAYER_ROLE, 0, __FUNCTION__);
        pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_KILLINGCONTROL,        TYPE_PLAYER_ROLE, 0, __FUNCTION__);
        pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_GROUPCONTROL,          TYPE_PLAYER_ROLE, 0, __FUNCTION__);
        pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_CONTINUE_PLAY,   TYPE_PLAYER_ROLE, 0, __FUNCTION__);
		pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_ACTIVE_TALENT,	TYPE_PLAYER_ROLE, 0, __FUNCTION__);
		pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ADD_ASSIST_BUFF,		TYPE_PLAYER_ROLE, 0, __FUNCTION__);
    }

    // 启动定时器(这个定时器只能是战场纤程的，不能是场景那边直接调过来的!!!)
    ITimerService * pTimerService  = gServerGlobal->getTimerService();
    if(pTimerService != NULL)
    {
        pTimerService->SetTimer(ETimerEventID_WarProcess, WarDefinRTime, this);
        pTimerService->SetTimer(ETimerEventID_DeadKillCount, WarDefinRTimeKill, this);
        // 定时存储比赛过程中人物的对战信息
        SEntityXml_Server* pXmlConfig = getXMLSchemeInfo();
        if (pXmlConfig && pXmlConfig->nRecordInfo == 1 && pXmlConfig->nTime > 0)
        {
            pTimerService->SetTimer(ETimerEventID_WarActorLog, pXmlConfig->nTime, this);
        }
		pTimerService->SetTimer(ETimerEventID_EcmExpSituation,  WarEcmExpSituationTime, this );
		pTimerService->SetTimer(ETimerEventID_EcmControlRatio,  WarEcmContorlRatioTime, this );
		pTimerService->SetTimer(ETimerEventID_EcmControlLine,  WarEcmControlLineTime, this );
    }

	return true;
}


// 反初始化
void CMobaAltarWar::UnInit()
{
	gServerGlobal->getDBEngineService()->unregisterHandler(this);
	m_mapLearnSkillInfo.clear();

	SceneLogicEventHelper eventHelper(m_nSceneID);
	rkt::IEventEngine* pEventEngine = eventHelper.m_ptr;
	if (pEventEngine != NULL)
	{
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_WAR_REC_DAMAGE,    TYPE_PLAYER_ROLE, 0);
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_DAMAGE,           TYPE_PLAYER_ROLE, 0);
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_CHANGE_STATE,     TYPE_PLAYER_ROLE, 0);
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_UPGRADE_LEVEL,     TYPE_PLAYER_ROLE, 0);
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_LEARN_SPELL,      TYPE_PLAYER_ROLE, 0);
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_FREEZE_CHANGED,   TYPE_PLAYER_ROLE, 0);
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_TALENT_CHANGE,     TYPE_PLAYER_ROLE, 0);
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ENTITY_PREPARE_CURE,     TYPE_PLAYER_ROLE, 0);
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_WAR_GOOD_LIST,     TYPE_PLAYER_ROLE, 0);
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_CLEAR_MULTI_KILL,        TYPE_PLAYER_ROLE, 0);
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_THRILLINGCURE,           TYPE_PLAYER_ROLE, 0);
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_KILLINGCONTROL,          TYPE_PLAYER_ROLE, 0);
        pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_GROUPCONTROL,            TYPE_PLAYER_ROLE, 0);
		pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_CONTINUE_PLAY,		TYPE_PLAYER_ROLE, 0);
		pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_ACTIVE_TALENT,     TYPE_PLAYER_ROLE, 0);
		pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ADD_ASSIST_BUFF,			TYPE_PLAYER_ROLE, 0);
	}

	CWar::UnInit();
}

// 战场基地被摧毁
void CMobaAltarWar::End()
{
    if (m_bIsFinish)
    {
        return;
    }

    m_ChargedMap.clear();

	CWar::End();

	//停止定时器
	ITimerService * pTimerService = gServerGlobal->getTimerService();
	if(pTimerService)
	{
		pTimerService->KillTimer(ETimerEventID_WarProcess, this);
		pTimerService->KillTimer(ETimerEventID_DeadKillCount, this);
		pTimerService->KillTimer(ETimerEventID_WarActorLog, this );
		pTimerService->KillTimer(ETimerEventID_EcmExpSituation, this );
		pTimerService->KillTimer(ETimerEventID_EcmControlRatio, this );
		pTimerService->KillTimer(ETimerEventID_EcmControlLine, this );
	}
}


void CMobaAltarWar::addPlayerData()
{
	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeCenter == NULL");
		return;
	}
	ISchemeEntity *pSchemeEntity = pSchemeCenter->getSchemeEntity();
	if (pSchemeEntity == NULL)
	{
		ErrorLn(__FUNCTION__": pSchemeEntity == NULL");
		return;
	}
	SEntityXml_Server* pEntityXml = pSchemeEntity->getEntityXml();
	if ( pEntityXml == NULL )
	{
		ErrorLn(__FUNCTION__": pEntityXml == NULL");
		return;
	}
	ISchemeExpOrGoldCreate* pExpOrGoldCreate = pSchemeCenter->getSchemeExpOrGoldCreate();
	if ( pExpOrGoldCreate == NULL )
	{
		ErrorLn(__FUNCTION__": can't find ExpOrGoldCreate config");
		return;
	}
	SExpOrGoldCreateConfig* pConfig = pExpOrGoldCreate->getExpOrGoldCreateConfig(getMatchTypeID(), ENEXPCREATETYPE_PLAYER);
	if ( pConfig == NULL )
	{
		ErrorLn(__FUNCTION__": can't find typeId = " << getMatchTypeID() << " exp or gold config!");
		return;
	}

	map<PDBID,UID>::iterator it = m_mapRealPlayerUid.begin();

	bool bIsAddAward = bIsCanReachAwarkCondition();

	IAntiAddictServer * pAntiAddict = gServerGlobal->getAntiAddictServer();

	for (; it != m_mapRealPlayerUid.end(); ++it)
	{
		UID uID = it->second;
		PDBID idActor = it->first;
		bool bIsHaveSameClan = isHaveSameClanMember(idActor);
#ifdef DEBUG_HAVESAMECLAN
		bIsHaveSameClan = true;
#endif
		obuf256 obData;
		SNetMsgHead head;
		head.bySrcEndPoint = MSG_ENDPOINT_SCENE;
		head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
		head.byKeyModule = MSG_MODULEID_ENTITY;
		head.byKeyAction = SC_MSG_ENTITY_WARENDAWARK_RESULT;

		SMsgEntityHead entityHead;
		entityHead.uidMaster = uID;
		entityHead.byPartID = PART_BANK;
		obData << head << entityHead;
		msg_entity_warend_awark msgData;
		ActorHelper helper(idActor, ACTOR_DBID);
		IActorService* pActor = helper.m_ptr;
		if( pActor == NULL || pActor->getProperty_Integer(PROPERTY_AI_SCHEME_ID) > 0)
			continue;
		setPlayerGlory(it->first);
		// 增加英雄信息
		int nCamp = getProperty_Integer(uID, PROPERTY_CAMP);
		int nHeroID = getProperty_Integer(uID, PROPERTY_VOCATION);

		msgData.nHeroID = nHeroID;
		msgData.bIsWin = false;
		msgData.bIsFirstWinAwark = false;
		msgData.lDBWarID = m_dwDBWarID;
		if ( nCamp != m_nFailedCamp )
		{
			msgData.bIsWin = true;
		}
		recordTeammateHeroID(nCamp,idActor, nHeroID);
		obuf obCardData;
		if ( bIsAddAward )
		{
			ExpAndGoldCreate( idActor, bIsHaveSameClan, pEntityXml, pConfig, msgData);
			// 策划需求暂时胜负都加卡牌
			DWORD arrCard[GETCARD_MAX_NUM] = {0};
			int arrCardCount[GETCARD_MAX_NUM] = {0};
			int nCount = getWarEndCardCount(idActor, nCamp, bIsHaveSameClan, pEntityXml);
			msgData.nCount = pActor->getRandHeroCard(arrCard, arrCardCount, GETCARD_MAX_NUM, OSSRES_ADD_WAR_PRIZE, nCount);
			for (int i = 0; i < msgData.nCount; ++i)
			{
				msg_entity_hero_card msgCard;
				msgCard.nHeroID = arrCard[i];
				msgCard.nCount = arrCardCount[i];
				obCardData.push_back( &msgCard, sizeof(msgCard));
			}
		}

		// 比赛记录
		if ( nCamp != m_nFailedCamp)
			pActor->setHeroOneInfo(nHeroID, 1, ENHEROINFOTYPE_WINNUM, ENCHANGEDATAMODE_ADD);
		pActor->setHeroOneInfo(nHeroID, 1, ENHEROINFOTYPE_MATCHNUM, ENCHANGEDATAMODE_ADD);
		pActor->setHeroOneInfo(nHeroID, (DWORD)time(NULL), ENHEROINFOTYPE_USETIME, ENCHANGEDATAMODE_SET);

		pActor->removeHeroExclusiveTalentEffect();
		if ( nCamp != m_nFailedCamp && !pActor->getFirstWinTime() && bIsHaveSameClan )
		{
			msgData.dwFirstWinTime = FIRSTWINTIME_INTERVAL;
			pActor->setPlayerFirstWinFlag((DWORD)time(NULL));
			msgData.bIsFirstWinAwark = true;
		} 

		SHeroInfo heroInfo = pActor->getHeroInfo(nHeroID);
		if ( heroInfo.nHeroID > 0 ) 
		{
			msgData.nCurrentExp = heroInfo.dwHeroExp;
			msgData.nCurrentLv = heroInfo.nLevel;
		}
		msgData.nPlayerNewLv = pActor->getPlayerLevel();

		// 防沉迷衰减,百分比
		msgData.nAntiReduceRate = 100;
		if (pAntiAddict != NULL)
		{
			msgData.nAntiReduceRate = pAntiAddict->getReduceRate(it->second);
		}

		obData.push_back(&msgData, sizeof(msgData));
		obData.push_back(obCardData.data(), obCardData.size());
		send2Client(uID, PACKAGE_PTR(new std::string(obData.data(), obData.size())) );
	}
}

void CMobaAltarWar::setHeroExclusiveTalentEffect(UID uID)
{
	ActorHelper actor(uID);
	IActorService* pActor = actor.m_ptr;
	if ( pActor == NULL )
		return;
	int nHeroID = getProperty_Integer(uID, PROPERTY_VOCATION);
	pActor->setHeroExclusiveTalentEffect(nHeroID);
}

bool CMobaAltarWar::playerIsWin(PDBID idActor)
{
	ActorHelper actorHelper(idActor, ACTOR_DBID);
	IActorService * pActorService = actorHelper.m_ptr;
	if( pActorService == NULL ) return false;
	int nCamp = pActorService->getProperty_Integer(PROPERTY_CAMP);
	if ( nCamp == m_nFailedCamp )
	{
		return false;
	}
	return true;
}


void CMobaAltarWar::setPlayerGlory(PDBID idActor)
{
	ActorHelper helper(idActor, ACTOR_DBID);
	IActorService* pActor = helper.m_ptr;
	if( pActor == NULL)
		return;

	int nGlory[ENACHIEVE_MAX] = {0};
	// 判断是否胜利
	if (playerIsWin(idActor))
	{
		nGlory[ENACHIEVE_WINNUM] += 1;
	}

	int nCamp = pActor->getProperty_Integer(PROPERTY_CAMP);
	setPlayerKillGloryInfo(idActor, nCamp, nGlory);

	nGlory[ENACHIEVE_TIMEMAX] = 0;
	nGlory[ENACHIEVE_TIMEMIN] = 0;
	nGlory[ENACHIEVE_CONTINUEWINMAX] = 0;

	map<PDBID, STWarEndGraded>::iterator it = m_mapWarEndGraded.find(idActor);
	if ( it != m_mapWarEndGraded.end())
	{
		STWarEndGraded stWarEndGrade = it->second;
		nGlory[ENACHIEVE_CONTINUEKILLMAX] = stWarEndGrade.nKillTimeTitle;
		if ( stWarEndGrade.nTitle & EWM_SCORE )
		{
			nGlory[ENACHIEVE_MVPNUM] += 1;
		}
		if ( stWarEndGrade.nTitle & EWM_KILLCOUNT )
		{
			nGlory[ENACHIEVE_ONCESKILLCOUNT] += 1;
		}
		if ( stWarEndGrade.nTitle & EWM_ASSCOUNT)
		{
			nGlory[ENACHIEVE_ONCEASSISTCOUNT] += 1;
		}
		if ( stWarEndGrade.nTitle & EWM_DESTORYTOWER)
		{
			nGlory[ENACHIEVE_FIRSTKILLTOWERNUM] += 1;
		}
	}


	nGlory[ENACHIEVE_MATCHNUM] += 1;
	long long WarDBKey = getWarDBKey();
	pActor->setActorSummonerOrGlory( nGlory, ENACHIEVE_MAX, ENPLAYERINFOTYPE_GLORY, WarDBKey);
}

/*单局得分算法：
胜利得分＝得分基数*（1-1/（1+10^（（期望成就-当前成就）/胜率基数））+战场评分奖励
失败扣分＝得分基数*（1/（1+10^（（期望成就-当前成就）/胜率基数））+战场评分奖励

战场评分奖励对战场表现评分高于评分下限的进行奖励。

战场评分奖励＝战场评分/评分倍率系数*评分奖励基数*/
int CMobaAltarWar::getAchievementScore( PDBID idActor )
{
	ActorHelper actorHelper(idActor, ACTOR_DBID);
	IActorService* pActorHelper = actorHelper.m_ptr;
	if ( pActorHelper == NULL )
	{
		ErrorLn(__FUNCTION__": pActorHelper == NULL idActor = " << idActor);
		return 0;
	}

	ISchemeHeroAchievementConfig* pHeroAchievementInfo = gServerGlobal->getSchemeCenter()->getSchemeHeroAchievementConfig();
	if ( pHeroAchievementInfo == NULL )
	{
		ErrorLn(__FUNCTION__": pHeroAchievementInfo == NULL")
			return 0;
	}
	int nRankGrade = pActorHelper->getMatchTypeRankGrade((EMMatchType)getMatchTypeID());

	int nMatchTypeID = getMatchTypeID();
	const SHeroAchievementScoreConfig* pScoreConfig = pHeroAchievementInfo->getHeroAchievementConfig(nMatchTypeID,nRankGrade);
	if( pScoreConfig == NULL )
	{
		ErrorLn(__FUNCTION__": can't find nMatchTypeID = " << nMatchTypeID << ", nRankGrade = " << nRankGrade);
		return 0;
	}
	int nHeroID = pActorHelper->getProperty_Integer(PROPERTY_VOCATION);
	SHeroInfo heroInfo = pActorHelper->getHeroInfo(nHeroID);
	if ( heroInfo.nHeroID == 0 )
	{
		return 0;
	}
	int nCurrentScore =  heroInfo.nHeroScore;
	int nScorinRewards = getWarScoringRewards(idActor, pScoreConfig);
	float fCoeff = getAchievementCoeff(nCurrentScore, pScoreConfig);
	bool bIsWin = playerIsWin(idActor);
	fCoeff = (bIsWin ? ( 1 - fCoeff) : fCoeff);

	return (int)(fCoeff * pScoreConfig->nScoreValue + nScorinRewards);
}

// 战场评分奖励＝战场评分/评分倍率系数*评分奖励基数
int CMobaAltarWar::getWarScoringRewards ( PDBID idActor, const SHeroAchievementScoreConfig* pScoreConfig )
{
	map<PDBID, STWarEndGraded>::iterator it = m_mapWarEndGraded.find(idActor);
	if ( it == m_mapWarEndGraded.end() || pScoreConfig == NULL )
	{
		ErrorLn(__FUNCTION__": at m_mapWarEndGraded can't find player idActor = " << idActor );
		return 0;
	}
	int nScore = it->second.nComprehensiveScore;
	int nScoringRatio = (pScoreConfig->nScoringRatio == 0 ? 1 : pScoreConfig->nScoringRatio);
	return ((nScore / nScoringRatio) * pScoreConfig->nScoringRewards);
}

// 1/（1+10^（（期望成就-当前成就）/胜率基数）
float CMobaAltarWar::getAchievementCoeff( int nCurrentScore, const SHeroAchievementScoreConfig* pScoreConfig )
{
	if ( pScoreConfig == NULL )
	{
		return 0.0f;
	}
	int nWinRateValue = pScoreConfig->nWinRateValue == 0 ? 1 : pScoreConfig->nWinRateValue;

	float nValue = pow( 10.0f, (float)((float)(pScoreConfig->nExpectValue - nCurrentScore)/(float)nWinRateValue));
	nValue = (1.0f + nValue == 0 ? 1 : 1 + nValue);

	return (1.0f/ nValue);
}

void CMobaAltarWar::recordTeammateHeroID(int nCamp,PDBID idActor, int nSelfHeroID)
{
	if(!bIsCanReachAwarkCondition())
	{
		return;
	}

	if(m_mapWarCamp.find(nCamp) == m_mapWarCamp.end())
	{
		return;
	}

	ActorHelper actorHelper(idActor, ACTOR_DBID);
	IActorService *pActorService = actorHelper.m_ptr;
	if (pActorService == NULL)
	{
		ErrorLn(__FUNCTION__"pActorService == NULL, uID = " << idActor);
		return ;
	}

	int TeammateHeroID[DBDEF_CAMPUSE_HEROID_MAXCOUNT] = {0};
	SWarCamp* pWarCamp = &m_mapWarCamp[nCamp];
	int nTeammateIndex = 0;
	TeammateHeroID[nTeammateIndex++] = nSelfHeroID;
	WarPersonVector::iterator itPerBegin = pWarCamp->warPersVect.begin();
	for (; itPerBegin != pWarCamp->warPersVect.end(); ++itPerBegin )
	{
		if (itPerBegin->nHeroID == nSelfHeroID)
			continue;

		TeammateHeroID[nTeammateIndex++] = itPerBegin->nHeroID;
	}

	pActorService->setActorTeammateHeroID(TeammateHeroID);
	return;
}

void CMobaAltarWar::setPlayerKillGloryInfo(PDBID idActor, int nCamp, int* pGlory)
{
	WarCampMap::iterator it = m_mapWarCamp.find(nCamp);
	if ( it == m_mapWarCamp.end() )
	{
		return;
	}

	SWarCamp sWarCamp = it->second;

	//WarPersonVector warPersVect;
	WarPersonVector::iterator itPerson = sWarCamp.warPersVect.begin();
	for (; itPerson != sWarCamp.warPersVect.end(); ++itPerson )
	{
		if ( itPerson->uPDBID == idActor )
		{
			pGlory[ENACHIEVE_KILLNUM] = itPerson->nBaseRecord[EDT_KillCount];
			pGlory[ENACHIEVE_SCOREMAX] = itPerson->nBaseRecord[EDT_Score];
			pGlory[ENACHIEVE_KILLMAX] = itPerson->nBaseRecord[EDT_KillCount];
			pGlory[ENACHIEVE_ASSISTMAX] = itPerson->nBaseRecord[EDT_AssistCount];
			pGlory[ENACHIEVE_OUTMAX] = itPerson->nBaseRecord[EDT_OutPut];
			pGlory[ENACHIEVE_DAMAGEMAX] = itPerson->nBaseRecord[EDT_Damage];
			pGlory[ENACHIEVE_MONEYMAX] = itPerson->nBaseRecord[EDT_Money];
			pGlory[ENACHIEVE_KILLMONSTERMAX] = itPerson->nBaseRecord[EDT_KillMonster];
			pGlory[ENACHIEVE_KILLTOWETMAX] = itPerson->nBaseRecord[EDT_DestoryTower];
			pGlory[ENACHIEVE_KILLBIGDRAGONNUM] = itPerson->nBigBossMonster;
			pGlory[ENACHIEVE_KILLSMALLDRAGONNUM] = itPerson->nSmallBossMonster;

            int nDeadContKill = itPerson->nDeadContKill;
            if ( nDeadContKill >= EKNT_TYPE_8)
            {
                // 超神次数不累加
                pGlory[ENACHIEVE_SUPERGODNUM] = 1;
            }
			ContinuKillList::iterator iter = itPerson->timeContKillList.begin();
			for (;iter != itPerson->timeContKillList.end();++iter)
			{
				int nTimeContKill = *iter;
				if ( nTimeContKill >= 5 )
				{
					pGlory[ENACHIEVE_5KILLNUM] += 1;
				}
				if ( nTimeContKill >= 4 )
				{
					pGlory[ENACHIEVE_4KILLNUM] += 1;
				}
				if ( nTimeContKill >= 3 )
				{
					pGlory[ENACHIEVE_3KILLNUM] += 1;
				}
			}
			break;
		}
	}
}



/** 容器事件触发后执行函数
@param   wEventID ：事件ID
@param   bSrcType ：发送源类型
@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
@param   pszContext : 上下文
@param   nLen : 上下文长度
@return  
*/
void CMobaAltarWar::OnExecute( WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen )
{
    CWar::OnExecute(wEventID, bSrcType, dwSrcID, pszContext, nLen );

	switch (wEventID)
	{
    case EVENT_GAME_ENTITY_LEARN_SPELL:     // 学习技能事件
        {
            if ( nLen != sizeof( event_learn_spell ))
            {
                ErrorLn(__FUNCTION__": OnExecute nLen != sizeof(event_learn_spell) nLen = " << nLen << ",sizeof(event_learn_spell" << sizeof(event_learn_spell));
                return;
            }

            event_learn_spell* pEvent = (event_learn_spell*)pszContext;
            onLearnSkill(pEvent);
        }
        break;

    case EVENT_GAME_ENTITY_FREEZE_CHANGED:  // 技能累却改变事件
        {
            if ( nLen != sizeof(event_freeze_changed))
            {
                ErrorLn(__FUNCTION__": OnExecute nLen != sizeof(event_freeze_changed) ");
                return;
            }
            event_freeze_changed* pEvent = (event_freeze_changed*)pszContext;
            onSkillFreesChangeed(pEvent);
        }
        break;
    case EVENT_ENTITY_PROGRESS_OK:
        {
            // 处理读条后的逻辑
            if ( nLen != sizeof(ProgressBarContext))
            {
                ErrorLn(__FUNCTION__": nLen != sizeof(ProgressBarContext) !" << ", nLen = " << nLen << ", sizeof(ProgressBarContext) = " << sizeof(ProgressBarContext));
                return;
            }

            ProgressBarContext* pProgressProgress = (ProgressBarContext*)pszContext;
            onPlayerProgressComplete(dwSrcID,pProgressProgress->uidTarget);
        }
        break;

    case EVENT_ENTITY_PROGRESS_BREAK:
        {
            // 处理读条中断的逻辑
            if ( nLen != sizeof(ProgressBarContext))
            {
                ErrorLn(__FUNCTION__": nLen != sizeof(ProgressBarContext) !" << ", nLen = " << nLen << ", sizeof(ProgressBarContext) = " << sizeof(ProgressBarContext));
                return;
            }

            ProgressBarContext* pProgressProgress = (ProgressBarContext*)pszContext;
            onPlayerProgressBreak(dwSrcID,pProgressProgress->uidTarget);
        }
        break;

	default:
		break;
	}
}

void CMobaAltarWar::onSkillFreesChangeed(event_freeze_changed* pEntityFreezeChanged)
{
	map<UID, STLearnKillInfo>::iterator it = m_mapLearnSkillInfo.find(pEntityFreezeChanged->PersonUID);
	if ( it == m_mapLearnSkillInfo.end())
	{
		return;
	}

    STLearnKillInfo & stLearnSkillInfo = it->second;
	if ( stLearnSkillInfo.dwFreezeId != pEntityFreezeChanged->dwFreezeID )
	{
		return;
	}
	ActorHelper actorHelper(stLearnSkillInfo.uId);
	IActorService* pActorServer = actorHelper.m_ptr;
	if ( pActorServer == NULL )
	{
		return;
	}

	DWORD nFreeTime = 0;
	switch( stLearnSkillInfo.nSlotType )
	{
	case SLOT_TYPE_GOODS:
		{
			nFreeTime = pActorServer->GetFreezeLeftTime(EFreeze_ClassID_Goods, stLearnSkillInfo.dwFreezeId);
		}
		break;
	case SLOT_TYPE_SKILL:
		{
			nFreeTime = pActorServer->GetFreezeLeftTime(EFreeze_ClassID_Spell, stLearnSkillInfo.dwFreezeId);
		}
		break;
    default: break;
	}

    msg_player_skill_state msgSkillState;
	msgSkillState.nCoolTime = nFreeTime;
	msgSkillState.nID = stLearnSkillInfo.dwSkillId;
	msgSkillState.uOperator = stLearnSkillInfo.uId;

	int nCamp = getProperty_Integer(stLearnSkillInfo.uId,PROPERTY_CAMP);
	broadCastDataToSameCamp(SC_MSG_WAR_BROADCAST_SKILL_STATE, (LPCSTR)&msgSkillState, sizeof(msg_player_skill_state), nCamp  );
}


void CMobaAltarWar::onLearnSkill( event_learn_spell* entityLearnSpell )
{
	msg_player_skill_state msgSkillState;
	int nSpellFlag = entityLearnSpell->nSpellFlag;
	if ( (nSpellFlag &SpellFlag_Space) != SpellFlag_Space)
	{
		//ErrorLn(__FUNCTION__ << "nSpellFlag != SpellFlag_Space");
		return;
	}

	// 目前这个函数只有大招的用处后面再优化吧 由于不知道为什么策划把xp技能也配了大招的标志位导致这个函数功能错乱 现在加个槽位过滤 大招槽位2
	if(entityLearnSpell->nSlotIndex<0||SPELL_FINAL_SKILL_SLOT != entityLearnSpell->nSlotIndex)
	{
		return;
	}

	map<UID, STLearnKillInfo>::iterator it = m_mapLearnSkillInfo.find(entityLearnSpell->uidOperator);
	if ( it != m_mapLearnSkillInfo.end())
	{
		STLearnKillInfo& stLearnSkillInfo = it->second;
		stLearnSkillInfo.nSlotType = entityLearnSpell->nSlotType;
		stLearnSkillInfo.dwSkillId = entityLearnSpell->nID;
		stLearnSkillInfo.dwFreezeId = entityLearnSpell->nCoolID;

		return;
	}
	STLearnKillInfo stLearnSkillInfo;
	stLearnSkillInfo.uId = entityLearnSpell->uidOperator;
	stLearnSkillInfo.nSlotType = entityLearnSpell->nSlotType;
	stLearnSkillInfo.dwSkillId = entityLearnSpell->nID;
	stLearnSkillInfo.dwFreezeId = entityLearnSpell->nCoolID;
	m_mapLearnSkillInfo.insert(std::make_pair(entityLearnSpell->uidOperator, stLearnSkillInfo));

	DWORD nFreeTime = 0;
	obuf oSkillInfo;
	msgSkillState.nCoolTime = nFreeTime;
	msgSkillState.nID = entityLearnSpell->nID;
	msgSkillState.uOperator = entityLearnSpell->uidOperator;
	oSkillInfo << msgSkillState;

	int nCamp = getProperty_Integer(entityLearnSpell->uidOperator,PROPERTY_CAMP);
	broadCastDataToSameCamp(SC_MSG_WAR_BROADCAST_SKILL_STATE, oSkillInfo.data(), oSkillInfo.size(), nCamp  );
}


IMonsterExec* CMobaAltarWar::getWarMonsterExecByType( EWarMonsterExec WarMonsterExec )
{
	MonsterExecMap::iterator monsterExecIt = m_mapAllMonstExec.find(WarMonsterExec);
	if(monsterExecIt != m_mapAllMonstExec.end())
	{
		return monsterExecIt->second;
	}
	return NULL;
}

bool CMobaAltarWar::onWarClientMsg(UID uidActor, BYTE byKeyAction, PACKAGE_PTR msg)
{

    // 处理战场公共消息
    if (CWar::onWarClientMsg(uidActor, byKeyAction, msg))
    {
        return true;
    }

    // 客户端单独其他消息
	void*	pData	= (void*)msg->c_str();
	size_t	stLen	= msg->size();
	switch ( byKeyAction )
	{
    case CS_MSG_WAR_ENTITY_SPECIAL_SELECT:
        {
            onClientSelectSpecialEntity(pData, stLen);
        }
        break;
    case CS_MSG_WAR_ENTITY_SPECIAL_SEARCH:
        {
            onClientSpecialEntityRangeSearch(pData, stLen);
        }
        break;
	default:
		{
			ErrorLn("CMobaAltarWar::onClientMsg unknown pHead->byKeyAction= " << byKeyAction);
            return false;
		}
	}

    return true;
}

void CMobaAltarWar::onClientDisconnect( ClientID client,DWORD dwReason )
{

}





void CMobaAltarWar::OnTimer(unsigned long dwTimerID)
{
	CWar::OnTimer(dwTimerID);

    if (ETimerEventID_WarProcess == dwTimerID )
    {
        switch(getWarPhase())
        {
        case EWP_Start:
            {

                // 检测瞭望台恢复没被占领状态
                checkWatchTowerRecover();
            }
            break;
        default:
            break;
        }
    }
}

void CMobaAltarWar::Realease()
{
	//停止定时器
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_WarProcess, this);
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_DeadKillCount, this);
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_WarActorLog, this );
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_EcmExpSituation, this );
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_EcmControlRatio, this );
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_EcmControlLine, this );
	CWar::Realease();

    delete this;
}


#pragma region 随机英雄卡牌

// 返回可获得卡牌数量
int CMobaAltarWar::getWarEndCardCount(PDBID idActor, int nCamp, bool bIsHaveSameClan, SEntityXml_Server* pEntityXml)
{
	ActorHelper actorHelper(idActor, ACTOR_DBID);
	IActorService *pActorService = actorHelper.m_ptr;
	if (pActorService == NULL)
	{
		ErrorLn(__FUNCTION__" pActorService == NULL, uID = " << idActor);
		return -1;
	}

	int nPlayerLv = pActorService->getPlayerLevel();
	DWORD dwIsFirstWin = pActorService->getFirstWinTime();
	int nCount = 0;
	nCount = getHeroCardCount(nPlayerLv, getMatchTypeID());
	if ( !dwIsFirstWin && playerIsWin(idActor) && bIsHaveSameClan)
	{
		nCount += pEntityXml->nFirstWinCardCount;
	}

	return nCount;
}

int CMobaAltarWar::getHeroCardCount( int nPlayerLv, int nWarType )
{
	ISchemeHeroCardCreat* pHeroCardConfig = gServerGlobal->getSchemeCenter()->getSchemeHeroCardCreat();
	if ( pHeroCardConfig == NULL )
	{
		ErrorLn(__FUNCTION__": pHeroCardConfig  == NULL");
		return -1;
	}
	SHeroCardCreatCount* pCardCount = pHeroCardConfig->getHeroCardCreateCount(nPlayerLv, nWarType);
	if ( pCardCount == NULL )
	{
		return 0;
	}

	int nRandNum = 0;
	rktRangeRand(nRandNum, 1, PROBABILITY_ENLARGE);

	int nCount = 0;
	for ( int i = 0; i < HEROCARD_NUM_TYPE; ++i )
	{
		int nLeftNum = 0;
		for (int j = i-1; j >=0; --j )
		{
			nLeftNum += pCardCount->sCardCount[j].nPro;
		}

		if (  nRandNum > nLeftNum && nRandNum <= pCardCount->sCardCount[i].nPro + nLeftNum )
		{
			nCount = pCardCount->sCardCount[i].nCardCount;
			break;
		}
	}
	return nCount;
}

#pragma endregion 随机英雄卡牌

#pragma region 经验金币产出

void CMobaAltarWar::ExpAndGoldCreate(PDBID idActor, bool bIsHaveSameClan, SEntityXml_Server* pEntityXml,SExpOrGoldCreateConfig* pConfig, msg_entity_warend_awark& msgAwark)
{
	if ( pEntityXml == NULL || pConfig == NULL)
	{
		ErrorLn(__FUNCTION__": pEntityXml == NULL or pConfig == NULL");
		return;
	}

	ActorHelper helper(idActor, ACTOR_DBID);
	IActorService* pActor = helper.m_ptr;
	if( pActor == NULL )
	{
		ErrorLn(__FUNCTION__": can't find actorService uID = " << idActor);
		return;
	}

	bool bIsWin = playerIsWin(idActor);

	int nPlayerExp = getStaticExp(idActor, ENEXPCREATETYPE_PLAYER,pConfig);
	int nHeroExp = getStaticExp(idActor, ENEXPCREATETYPE_HERO, pConfig);
	int nGetGold = getStaticGold(bIsWin);
	int nPrizeID = 0;

	DWORD dwIsBirstWin = pActor->getFirstWinTime();
	bool isFristWin = (bIsWin && (dwIsBirstWin == 0));      // 是否首胜

	//if (isFristWin && bIsHaveSameClan)
	//{
	//	nPlayerExp += pEntityXml->nFirstWinPlayerExp;
	//	nHeroExp += pEntityXml->nFirstWinHeroExp;
	//	nGetGold += pEntityXml->nFirstWinPlayerGold;
	//	nPrizeID = pEntityXml->nFirstWinPrizeID;        // 奖励ID
	//}

	int nGetAchievement = getAchievementScore(idActor);

	int nHeroID = pActor->getProperty_Integer(PROPERTY_VOCATION);
	nPlayerExp = nPlayerExp < 0 ? 0 : nPlayerExp;
	nHeroExp = nHeroID < 0 ? 0 : nHeroExp;
	nGetGold = nGetGold < 0 ? 0 : nGetGold;

	SHeroInfo heroInfo = pActor->getHeroInfo(nHeroID);
	if ( heroInfo.nHeroID > 0 ) 
	{
		msgAwark.nHeroOldExp = heroInfo.dwHeroExp;
		msgAwark.nHeroOldLv= heroInfo.nLevel;
	}
	msgAwark.nPlayerOldLv = pActor->getPlayerLevel();

	// 计算师徒增益
	BYTE nMentorshipPost = 0;
	bool bHaveMentorship = isHaveMentorship(idActor, nMentorshipPost);
	int nMentorhsipGoldPer = 1;
	int nMentorshipActorExpPer = 1;
	int nMentorshipHeroExpPer = 1;
	
	MentorshipHelper mentorshipHelper;
	IMentorshipSceneService *pMentorshipService = mentorshipHelper.m_ptr;
	if (pMentorshipService != NULL)
	{
		if (bHaveMentorship == true)
		{
			pMentorshipService->getMentorshipAward(idActor, &nMentorshipActorExpPer, &nMentorshipHeroExpPer, &nMentorhsipGoldPer);
		}
		pMentorshipService->sendFightResult(idActor, bIsWin, isFristWin, nMentorshipPost);
	}

// 计算增益卡效果
	int nGoldMultiple = 1;
	int nActorExpMultiple = 1;
	int nHeroExpMultiple = 1;
	pActor->calcBenefitCardMultiple(&nGoldMultiple, &nActorExpMultiple, &nHeroExpMultiple);

	// 给奖励
	char szLogDesc[256] = {0};
	ssprintf_s(szLogDesc, sizeof(szLogDesc), "战场[%I64d]获得奖励", (isFristWin?"首胜":""),  m_dwDBWarID);

	// 角色（召唤师）经验
	nPlayerExp = nPlayerExp * nActorExpMultiple + nPlayerExp * (nMentorshipActorExpPer/ZOOM_IN_MULTIPLE);
	pActor->setActorBaseInfo( nPlayerExp, ENPLAYERINFOTYPE_EXP, ENCHANGEDATAMODE_ADD, OSS_NUMERIC_EXP, OSS_RESOURCE_OPT_ADD, OSSRES_ADD_WAR_PRIZE, idActor, szLogDesc, 0);

	// 金钱
	nGetGold = nGetGold * nGoldMultiple + nGetGold * (nMentorhsipGoldPer/ZOOM_IN_MULTIPLE);
    pActor->addMoney(0, nGetGold, OSSRES_ADD_WAR_PRIZE, szLogDesc, 0, 0);

	// 英雄经验
	nHeroExp = nHeroExp * nHeroExpMultiple + nHeroExp * (nMentorshipHeroExpPer/ZOOM_IN_MULTIPLE);
	pActor->setHeroOneInfo(nHeroID, nHeroExp, ENHEROINFOTYPE_EXP, ENCHANGEDATAMODE_ADD, OSS_NUMERIC_HEROEXP, OSS_RESOURCE_OPT_ADD, OSSRES_ADD_WAR_PRIZE, idActor, szLogDesc, 0);

	// 个人奖励
	if (nPrizeID > 0) {
		pActor->prizeActor(nPrizeID, 1, OSSRES_ADD_WAR_PRIZE, 0, szLogDesc);
	}

	// 英雄成就得分
	pActor->setHeroOneInfo(nHeroID, nGetAchievement, ENHEROINFOTYPE_SCORE, ENCHANGEDATAMODE_ADD);

	msgAwark.nActorGetExp = nPlayerExp;
	msgAwark.nHeroExp = nHeroExp;
	msgAwark.nGold = nGetGold;
	msgAwark.nSuccessScore = nGetAchievement;
}

// 经验值 = (   (召唤师等级-经验_初始等级(胜利))   *    经验_斜率等级成长(胜利)+经验_初始斜率(胜利)   )*(游戏时长-经验_时间临界点(胜利))  +   经验_临界点经验值(胜利)
// 经验值 = （（ 召唤师等级-经验_初始等级(失败)）  *    经验_斜率等级成长(失败)+经验_初始斜率(失败)   )*（游戏时长  - 经验_时间临界点(失败) ）+  经验_临界点经验值(失败)
int CMobaAltarWar::getStaticExp(PDBID idActor, int nType, SExpOrGoldCreateConfig* pConfig)
{
	if ( pConfig == NULL )
	{
		return 0;
	}

	ActorHelper actorHelper(idActor, ACTOR_DBID);
	IActorService* pActorService = actorHelper.m_ptr;
	if ( pActorService == NULL )
	{
		ErrorLn(__FUNCTION__": pActorService == NULL , uID = " << idActor);
		return 0;
	}
	int nPlayerLv = pActorService->getPlayerLevel();
	int nCamp = pActorService->getProperty_Integer(PROPERTY_CAMP);

	bool bIsWin = playerIsWin(idActor);
	float fExp = 0.0f;
	if ( bIsWin )
	{
		fExp = ((float)nPlayerLv - pConfig->fWinCoeff[ENECPORGOLDCOEFF_EXP_BASELV]) * pConfig->fWinCoeff[ENECPORGOLDCOEFF_EXP_SLOPEGROW] + pConfig->fWinCoeff[ENECPORGOLDCOEFF_EXP_BASESLOPE];
		fExp = fExp * ( (float)m_continueTime/60.0f - pConfig->fWinCoeff[ENECPORGOLDCOEFF_EXP_TIMECRITICAL]);
		fExp = fExp + pConfig->fWinCoeff[ENECPORGOLDCOEFF_EXP_CRITICALEXP];
	}else
	{
		fExp = ((float)nPlayerLv - pConfig->fFailCoeff[ENECPORGOLDCOEFF_EXP_BASELV]) * pConfig->fFailCoeff[ENECPORGOLDCOEFF_EXP_SLOPEGROW] + pConfig->fFailCoeff[ENECPORGOLDCOEFF_EXP_BASESLOPE];
		fExp = fExp * ( (float)m_continueTime/60.0f - pConfig->fFailCoeff[ENECPORGOLDCOEFF_EXP_TIMECRITICAL]);
		fExp = fExp + pConfig->fFailCoeff[ENECPORGOLDCOEFF_EXP_CRITICALEXP];
	}
	return (int)fExp;
}

//金币数量 = 时间系数（胜利） * 游戏时长 + 匹配模式加成
//金币数量 = 时间系数 （失败）* 游戏时长 + 匹配模式加成
int CMobaAltarWar::getStaticGold( bool bIsWin, int nType )
{
	ISchemeExpOrGoldCreate* pExpOrGoldCreate = gServerGlobal->getSchemeCenter()->getSchemeExpOrGoldCreate();
	if ( pExpOrGoldCreate == NULL )
	{
		ErrorLn(__FUNCTION__": can't find ExpOrGoldCreate config");
		return 0;
	}
	SExpOrGoldCreateConfig* pConfig = pExpOrGoldCreate->getExpOrGoldCreateConfig(getMatchTypeID(), nType);
	if ( pConfig == NULL )
	{
		ErrorLn(__FUNCTION__": can't find typeId = " << getMatchTypeID() << " exp or gold confi");
		return 0;
	}

	int nGold = 0;
	float fCoeff = 0.0f;
	if ( bIsWin)
	{
		fCoeff = pConfig->fWinCoeff[ENECPORGOLDCOEFF_GOLD_TIME];
	}else
	{
		fCoeff = pConfig->fFailCoeff[ENECPORGOLDCOEFF_GOLD_TIME];
	}
	nGold = (int)(fCoeff * ((float)m_continueTime/60.0f) + pConfig->fGoldAddition);
#ifdef PRINTF_PLANLOG
	//WarningLn(__FUNCTION__": Planning requirements : gongshi =>  nGold = fCoeff * ((float)m_continueTime/60.0f) + pConfig->fGoldAddition");
	//WarningLn(__FUNCTION__": Planning requirements : fCoeff = " << fCoeff << ", ((float)m_continueTime/60.0f) = " << (float)m_continueTime/60.0f <<  ", pConfig->fGoldAddition = " << pConfig->fGoldAddition );
#endif // PRINTF_PLANLOG
	return nGold;
}
#pragma endregion 经验金币产出

void CMobaAltarWar::onHeroDieRemovBuff( UID uid)
{
	if(UID_2_TYPE(uid) != TYPE_PLAYER_ROLE || getXMLSchemeInfo()->nDeadIsDisappear )
	{
		return;
	}
	PDBID pDBID = getProperty_Integer(uid, PROPERTY_ID);

	if (m_pXPSkill != NULL)
		m_pXPSkill->removeXPBuff(uid, pDBID);
}


// 客户端选取特殊实体 目前有祭坛
void CMobaAltarWar::onClientSelectSpecialEntity(void* pData, int stLen)
{
    if ( stLen < sizeof(msg_war_select_special_entity) )
    {
        ErrorLn(__FUNCTION__": message length is too short! stLen=" << stLen);
        return;
    }

    msg_war_select_special_entity* pSelectData = (msg_war_select_special_entity*)pData;
    switch((ESpecial_Select_Entity_Function_Type)pSelectData->nFunctionType)
    {
    case ESSEFT_ALTARHANDIN:
        {
            // 处理祭坛进度条
            if(!canStartPlayerAltarProgress(pSelectData->uidOperator,pSelectData->uidTarget))
            {
                break;
            }

            startPlayerProgress(pSelectData->uidOperator,pSelectData->uidTarget,EPPT_ALTARPROGRESS);
        }
        break;
    default:
        break;
    }

    return;
}

bool CMobaAltarWar::canStartPlayerAltarProgress(UID uidActor,UID uidNpc)
{
    if(UID_2_TYPE(uidActor)!= TYPE_PLAYER_ROLE)
    {
        return false;
    }

    if(UID_2_TYPE(uidNpc)!= TYPE_MONSTER)
    {
        return false;
    }

    SceneLogicHelper helper(m_nSceneID);
    ISceneLogicService* pSceneLogicService = helper.m_ptr;
    if(pSceneLogicService == NULL)
    {
        return false;
    }

    INPCService* pNpcService = pSceneLogicService->getNPCService();
    if(pNpcService == NULL)
    {
        return false;
    }

    if((pNpcService->getProperty_Integer(uidNpc,PROPERTY_MASK)&MASKFLAG_SELECT_SPECIAL) != MASKFLAG_SELECT_SPECIAL)
    {
        return false;
    }

    ActorHelper actorHelper(uidActor,ACTOR_UID);
    IActorService* pActorServer = actorHelper.m_ptr;
    if(pActorServer == NULL)
    {
        return false;
    }

    int nCamp = pActorServer->getProperty_Integer(PROPERTY_CAMP);
    // 祭坛结构有数组 效验下
    if(nCamp < 0 || nCamp >= WarAltarTimeCampCount)
    {
        return false;
    }

    SWarAltarInfo* pAltarInfo = getWarAltarInfo(uidNpc);
    if(pAltarInfo == NULL)
    {
        SWarAltarInfo sAltarInfo;
        sAltarInfo.uidAltar = uidNpc;
        m_vecAltarInfo.push_back(sAltarInfo);
        pAltarInfo = &m_vecAltarInfo.back();
    }

    // 效验相同阵营祭坛上交间隔 脚本读取
    // 脚本配置
    int nHandInInterval = 60000;
    DWORD dwCuTickCount = gServerGlobal->getTimeSyncService()->GetTick();
    if(dwCuTickCount - pAltarInfo->dwCampLastAltarTime[nCamp] < nHandInInterval)
    {
        return false;
    }

    if(pAltarInfo->bHandIning)
    {
        return false;
    }

    SWPerson* pPersonInfo =getPlayerInfo(uidActor,nCamp);
    if(pPersonInfo == NULL)
    {
        return false;
    }

    // 在此看需求要不要判断下该玩家拥有的宝物数量 为0的话 不开始进度条 
    pPersonInfo->eProgressType = EPPT_ALTARPROGRESS;
    // 这两个变量的设置是为了以后可以做同阵营的玩家一起交宝物
    pAltarInfo->bHandIning =true;
    pAltarInfo->nHandIningCamp = nCamp;

    return true;
}

void CMobaAltarWar::playerHandInAltarSuccess(int nCamp ,UID uidNpc)
{
    // 目前只做了更新阵营祭坛上交成功后的祭坛信息 
    // 祭坛结构有数组 效验下
    if(nCamp < 0 || nCamp >= WarAltarTimeCampCount)
    {
        return ;
    }

    SWarAltarInfo* pAltarInfo = getWarAltarInfo(uidNpc);
    if(pAltarInfo == NULL)
    {
        return;
    }
    // 更新上交状态
    pAltarInfo->bHandIning = false;
    pAltarInfo->nHandIningCamp = 0;
    // 如果宝物数量满足条件 记录满足时间 发起攻击 目前没有判断满足 直接记录
    pAltarInfo->dwCampLastAltarTime[nCamp] = gServerGlobal->getTimeSyncService()->GetTick();	
}

void CMobaAltarWar::playerHandInAltarFail(  int nCamp,  UID uidNpc )
{
    // 目前只做了更新阵营祭坛上交时间
    SWarAltarInfo* pAltarInfo = getWarAltarInfo(uidNpc);
    if(pAltarInfo == NULL)
    {
        return;
    }

    pAltarInfo->bHandIning =false;
    pAltarInfo->nHandIningCamp = 0;
}

SWarAltarInfo* CMobaAltarWar::getWarAltarInfo(UID uidAltar)
{
    // 目前只做了更新阵营祭坛上交时间
    vector<SWarAltarInfo>::iterator iterAltar  = m_vecAltarInfo.begin();
    for (; iterAltar != m_vecAltarInfo.end(); ++iterAltar)
    {
        if(iterAltar->uidAltar == uidAltar)
        {
			return &(*iterAltar);
        }
    }

    return NULL;
}

// 客户端特殊实体范围搜寻 目前有瞭望台
void CMobaAltarWar::onClientSpecialEntityRangeSearch( void* pData, int stLen)
{
    if ( stLen < sizeof(msg_war_special_entity_range_search) )
    {
        ErrorLn(__FUNCTION__": message length is too short! stLen=" << stLen);
        return;
    }

    msg_war_special_entity_range_search* pSearchData = (msg_war_special_entity_range_search*)pData;
    switch((ESpecial_Entity_Range_Search_Function_Type)pSearchData->nFunctionType)
    {
    case ESRSFT_WATCHOUTTOWER:
        {
            // 处理瞭望台进度条
            if(pSearchData->bSearchOrBreak)
            {
                if(!canStartPlayerWatchOutTowerProgress(pSearchData->uidTarget,pSearchData->uidOperator))
                {
                    break;
                }

                startPlayerProgress(pSearchData->uidTarget,pSearchData->uidOperator,EPPT_WATCHTOWERPROGRESS);
            }
            else
            {
                onPlayerProgressBreak(pSearchData->uidTarget,pSearchData->uidOperator);
            }
        }
        break;
    default:
        break;
    }

    return;
}

bool CMobaAltarWar::canStartPlayerWatchOutTowerProgress( UID uidActor, UID uidNpc)
{
    if(UID_2_TYPE(uidActor)!= TYPE_PLAYER_ROLE)
    {
        return false ;
    }

    if(UID_2_TYPE(uidNpc)!= TYPE_MONSTER)
    {
        return false;
    }

    SceneLogicHelper helper(m_nSceneID);
    ISceneLogicService* pSceneLogicService = helper.m_ptr;
    if(pSceneLogicService == NULL)
    {
        return false;
    }

    INPCService* pNpcService = pSceneLogicService->getNPCService();
    if(pNpcService == NULL)
    {
        return false;
    }

    if((pNpcService->getProperty_Integer(uidNpc,PROPERTY_MASK)&MASKFLAG_SEARCH_SPECIAL) != MASKFLAG_SEARCH_SPECIAL)
    {
        return false;
    }

    SWarWatchTowerInfo* pWatchTowerInfo = getWarWatchTowerInfo(uidNpc);
    if(pWatchTowerInfo == NULL)
    {
        SWarWatchTowerInfo sWatchTowerInfo;
        sWatchTowerInfo.uidWatchTower = uidNpc;
        m_vecWatchTowerInfo.push_back(sWatchTowerInfo);
        pWatchTowerInfo = &m_vecWatchTowerInfo.back();
    }

    if(pWatchTowerInfo->bCapture)
    {
        return false;
    }

    ActorHelper actorHelper(uidActor,ACTOR_UID);
    IActorService* pActorServer = actorHelper.m_ptr;
    if(pActorServer == NULL)
    {
        return false;
    }

    int nCamp = pActorServer->getProperty_Integer(PROPERTY_CAMP);
    SWPerson* pPersonInfo =getPlayerInfo(uidActor,nCamp);
    if(pPersonInfo == NULL)
    {
        return false;
    }

    pPersonInfo->eProgressType = EPPT_WATCHTOWERPROGRESS;
    return true;
}

void CMobaAltarWar::playerCaptureWatchTowerSuccess( int nCamp ,UID uidNpc )
{
    // 目前只做了更新瞭望台被占领信息
    SWarWatchTowerInfo* pWatchTowerInfo = getWarWatchTowerInfo(uidNpc);
    if(pWatchTowerInfo == NULL)
    {
        return;
    }

    pWatchTowerInfo->nCaptureCamp = nCamp;
    pWatchTowerInfo->bCapture = true;
    pWatchTowerInfo->dwCaptureTick = gServerGlobal->getTimeSyncService()->GetTick();
    // 瞭望台占领状态 告知客户端 不要在搜索了
    bool bSearch = pWatchTowerInfo->bCapture ? false : true;
    SendClientSpecialEntitySearchStateChange(pWatchTowerInfo->uidWatchTower,bSearch);
}

void CMobaAltarWar::checkWatchTowerRecover()
{
    DWORD dwTickCount = gServerGlobal->getTimeSyncService()->GetTick();
    int nRecoverTime = 60000;// 取自配置

    vector<SWarWatchTowerInfo>::iterator iterWarWatch  = m_vecWatchTowerInfo.begin();
    vector<SWarWatchTowerInfo>::iterator iterWarWatchEnd  = m_vecWatchTowerInfo.end();
    for (;iterWarWatch!=iterWarWatchEnd;++iterWarWatch)
    {
        if(!iterWarWatch->bCapture )
        {
            continue;
        }

        if(dwTickCount - iterWarWatch->dwCaptureTick < nRecoverTime)
        {
            continue;
        }

        iterWarWatch->nCaptureCamp = 0;
        iterWarWatch->bCapture = false;
        iterWarWatch->dwCaptureTick = 0;
        // 恢复瞭望台无人占领状态 告知客户端 启动搜索
        bool bSearch = iterWarWatch->bCapture ? false : true;
        SendClientSpecialEntitySearchStateChange(iterWarWatch->uidWatchTower,bSearch);
    }
}

void CMobaAltarWar::SendClientSpecialEntitySearchStateChange(UID uidNpc,bool bSearch)
{
    // 增加搜索停止和启动功能 方便控制和提高性能
    msg_war_special_entity_search_state_change msgSearchData;
    msgSearchData.uidOperator = uidNpc;
    msgSearchData.bSearch = bSearch;
    broadCastDataToAllPlayer(SC_MSG_WAR_ENTITY_SPECIAL_SEARCH, (char*)&msgSearchData,sizeof(msgSearchData));
}

SWarWatchTowerInfo* CMobaAltarWar::getWarWatchTowerInfo(UID uidWatchTower)
{
    vector<SWarWatchTowerInfo>::iterator iterWarWatch  = m_vecWatchTowerInfo.begin();
    vector<SWarWatchTowerInfo>::iterator iterWarWatchEnd  = m_vecWatchTowerInfo.end();
    for (;iterWarWatch!=iterWarWatchEnd;++iterWarWatch)
    {
        if(iterWarWatch->uidWatchTower == uidWatchTower)
        {
            break;
        }
    }

    if(iterWarWatch == iterWarWatchEnd)
    {
        return NULL;
    }

    return &(*iterWarWatch);
}

// 人物读条
void CMobaAltarWar::startPlayerProgress(UID uidActor,UID uidNpc,EPlayerProgressType eType)
{
    ActorHelper actorHelper(uidActor,ACTOR_UID);
    IActorService* pActorServer = actorHelper.m_ptr;
    if(pActorServer == NULL)
    {
        return ;
    }
    // 取自配置
    int nProgressTime = 10000;
    int nBreakFlag = 0;

    ProgressBarContext context;
    context.nWndStyle = 0;
    context.szWndTxt  = "";
    context.nTime     = nProgressTime;
    context.nBreakFlag= nBreakFlag;
    context.pCallBack = this;
    context.bSendClient= false;       // 不依赖他的同步包，可减少2次数据包通讯
    context.uidTarget  = uidNpc;
    context.fDistanceRange = 0;
    context.nSpellID = 0;

    pActorServer->StartProgressBar(&context);
}

void CMobaAltarWar::onPlayerProgressComplete(UID uidActor,UID uidNpc)
{
    ActorHelper actorHelper(uidActor);
    IActorService* pActorService = actorHelper.m_ptr;

    int nCamp = pActorService->getProperty_Integer(PROPERTY_CAMP);
    SWPerson* pPersonInfo =getPlayerInfo(uidActor,nCamp);
    if(pPersonInfo == NULL)
    {
        return;
    }

    switch(pPersonInfo->eProgressType)
    {
    case EPPT_ALTARPROGRESS:
        {
            // 处理祭坛上交宝物是否充满 上交宝物 充满后处理攻击敌方水晶逻辑 先做一部分
            playerHandInAltarSuccess(nCamp,uidNpc);
        }
        break;
    case EPPT_WATCHTOWERPROGRESS:
        {
            // 处理瞭望台占领事件 先做一部分
            playerCaptureWatchTowerSuccess(nCamp,uidNpc);
        }
        break;
    default:
        break;
    }
}

void CMobaAltarWar::onPlayerProgressBreak(UID uidActor,UID uidNpc)
{
    ActorHelper actorHelper(uidActor);
    IActorService* pActorService = actorHelper.m_ptr;

    int nCamp = pActorService->getProperty_Integer(PROPERTY_CAMP);
    SWPerson* pPersonInfo =getPlayerInfo(uidActor,nCamp);
    if(pPersonInfo == NULL)
    {
        return ;
    }

    switch(pPersonInfo->eProgressType)
    {
    case EPPT_ALTARPROGRESS:
        {
            playerHandInAltarFail( nCamp , uidNpc);
        }
        break;
    case EPPT_WATCHTOWERPROGRESS:
        {

        }
        break;
    default:
        break;
    }

    pPersonInfo->eProgressType  = EPPT_INVALID;
}

#endif