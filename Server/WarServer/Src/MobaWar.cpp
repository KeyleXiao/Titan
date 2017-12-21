#include "StdAfx.h"
#include "MobaWar.h"
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
#include "IOSSLogService.h"
#include "ExternalHelper.h"
#include <math.h>



//#define __WAR__HERO_GET_EXP__TALENT__DEBUG__
#ifndef __WAR__HERO_GET_EXP__TALENT__DEBUG__

#endif

CMobaWar::CMobaWar()
{
    
}

CMobaWar::~CMobaWar()
{

}

// 初始化
bool CMobaWar::create(IWarService* pWarService,SWarCreateContext& context)
{
	return CWar::create(pWarService, context);
}

// 初始化
bool CMobaWar::Init()
{
	CWar::Start();
	CWar::Init();

    SceneLogicEventHelper eventHelper(getWarSceneID());
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
		pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_ACTIVE_TALENT,   TYPE_PLAYER_ROLE, 0, __FUNCTION__);
		pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ADD_ASSIST_BUFF,		TYPE_PLAYER_ROLE, 0, __FUNCTION__);
	}

    // 启动定时器(这个定时器只能是战场纤程的，不能是场景那边直接调过来的!!!)
    ITimerService * pTimerService  = gServerGlobal->getTimerService();
    if(pTimerService != NULL)
    {
        pTimerService->SetTimer(ETimerEventID_WarProcess,       WarDefinRTime, this);
        pTimerService->SetTimer(ETimerEventID_DeadKillCount,    WarDefinRTimeKill, this);
        // 定时存储比赛过程中人物的对战信息
     /*   SEntityXml_Server* pXmlConfig = g_EHelper.getEntityXml();
        if (pXmlConfig && pXmlConfig->nRecordInfo==1 && pXmlConfig->nTime > 0)
        {
            pTimerService->SetTimer(ETimerEventID_WarActorLog, pXmlConfig->nTime, this);
        }*/
		pTimerService->SetTimer(ETimerEventID_EcmExpSituation,  WarEcmExpSituationTime, this );
	    pTimerService->SetTimer(ETimerEventID_EcmControlRatio,  WarEcmContorlRatioTime, this );
		pTimerService->SetTimer(ETimerEventID_EcmControlLine,  WarEcmControlLineTime, this );
    }

	return true;
}


// 反初始化
void CMobaWar::UnInit()
{
	SceneLogicEventHelper eventHelper(getWarSceneID());
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
		pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_ACTIVE_TALENT,		TYPE_PLAYER_ROLE, 0);
		pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ADD_ASSIST_BUFF,			TYPE_PLAYER_ROLE, 0);
	}

	//停止定时器
	ITimerService * pTimerService = gServerGlobal->getTimerService();
	if (pTimerService)
	{
		pTimerService->KillTimer(ETimerEventID_WarProcess, this);
		pTimerService->KillTimer(ETimerEventID_DeadKillCount, this);
		//pTimerService->KillTimer(ETimerEventID_WarActorLog, this );
		pTimerService->KillTimer(ETimerEventID_EcmExpSituation, this);
		pTimerService->KillTimer(ETimerEventID_EcmControlRatio, this);
		pTimerService->KillTimer(ETimerEventID_EcmControlLine, this);
	}

	// 移除流程定时器
	for (int i = 0; i < m_nBroadcastProcessCount; ++i)
	{
		TraceLn(__FUNCTION__": timerID =" << ETimerEventID_Max + i);
		gServerGlobal->getTimerService()->KillTimer(ETimerEventID_Max + i, this);
	}

	CWar::UnInit();
}

// 战场基地被摧毁
void CMobaWar::End()
{
    if (isEnd())
    {
        return;
    }

	CWar::End();

	//停止定时器
	ITimerService * pTimerService = gServerGlobal->getTimerService();
	if(pTimerService)
	{
		pTimerService->KillTimer(ETimerEventID_WarProcess, this);
		pTimerService->KillTimer(ETimerEventID_DeadKillCount, this);
		//pTimerService->KillTimer(ETimerEventID_WarActorLog, this );
		pTimerService->KillTimer(ETimerEventID_EcmExpSituation, this );
		pTimerService->KillTimer(ETimerEventID_EcmControlRatio, this );
		pTimerService->KillTimer(ETimerEventID_EcmControlLine, this );
	}

	// 移除流程定时器
	for (int i = 0; i < m_nBroadcastProcessCount; ++i)
	{
		TraceLn(__FUNCTION__": timerID =" << ETimerEventID_Max + i);
		gServerGlobal->getTimerService()->KillTimer(ETimerEventID_Max + i, this);
	}
}


void CMobaWar::addPlayerData()
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
	IOSSLogService* pOSSLogService = gServerGlobal->getOSSLogService();
	if (pOSSLogService == NULL)
	{
		ErrorLn(__FUNCTION__": pOSS == NULL");
		return;
	}

	map<PDBID,UID>::iterator it = m_mapRealPlayerUid.begin();

	bool bIsAddAward = bIsCanReachAwarkCondition();

	IAntiAddictServer * pAntiAddict = gServerGlobal->getAntiAddictServer();

	DWORD nNow = time(NULL);

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
		msgData.lDBWarID = getWarDBKey();
		if ( nCamp == getWinCamp())
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
		if ( nCamp == getWinCamp())
			pActor->setHeroOneInfo(nHeroID, 1, ENHEROINFOTYPE_WINNUM, ENCHANGEDATAMODE_ADD);
		pActor->setHeroOneInfo(nHeroID, 1, ENHEROINFOTYPE_MATCHNUM, ENCHANGEDATAMODE_ADD);
		pActor->setHeroOneInfo(nHeroID, (DWORD)time(NULL), ENHEROINFOTYPE_USETIME, ENCHANGEDATAMODE_SET);

		pActor->removeHeroExclusiveTalentEffect();
		if ( nCamp == getWinCamp() && !pActor->getFirstWinTime() && bIsHaveSameClan )
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

		msgData.nActorCurExp = pActor->getPlayerExp();
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

		DWORD dwWorldID = pActor->getFromGameWorldID();
		PDBID pdbid = it->first;
		pOSSLogService->addNumericLog(dwWorldID, OSS_OPT_TYPE_PERSON, pdbid, getWarDBKey(), msgData.nActorGetExp, OSS_NUMERIC_EXP, OSS_RESOURCE_OPT_ADD, OSSRES_ADD_WAR_PRIZE, nNow, a2utf8("战场增加角色经验"), m_context.dwMatchTypeID);
		pOSSLogService->addNumericLog(dwWorldID, OSS_OPT_TYPE_PERSON, pdbid, getWarDBKey(), msgData.nGold, OSS_NUMERIC_BINDTICKET, OSS_RESOURCE_OPT_ADD, OSSRES_ADD_WAR_PRIZE, nNow, a2utf8("战场增加角色金币"), m_context.dwMatchTypeID);
	}
}

void CMobaWar::setBcastProcessTimer()
{
	for (int i = 0; i < m_nBroadcastProcessCount; ++i)
	{
		TraceLn(__FUNCTION__": timerID =" << ETimerEventID_Max + i );
		gServerGlobal->getTimerService()->SetTimer(ETimerEventID_Max + i, m_BroadCastTimeList[i].BroadCastTime * 1000, this, 1, __FILE__);
	}
}

void CMobaWar::onPlayerContinuePlay(event_entity_continue_play * pEvent)
{
	CWar::onPlayerContinuePlay(pEvent);

	SWPerson* pPerson = getPlayerInfoByPDBIU(pEvent->dwDBID);
	if (pPerson == NULL)
		return;

	msg_war_player_reconnection msg;
	msg.actorID = pEvent->dwDBID;
	msg.heroID = pPerson->nHeroID;
	sstrcpyn(msg.actorName, pPerson->szName, sizeof(msg.actorName));

	g_EHelper.sendMessageToClientWar(pPerson->uID, SC_MSG_WAR_PLAYER_RECONNECTION, (LPCSTR)&msg, sizeof(msg));
}

void CMobaWar::setHeroExclusiveTalentEffect(UID uID)
{
	ActorHelper actor(uID);
	IActorService* pActor = actor.m_ptr;
	if ( pActor == NULL )
		return;
	int nHeroID = getProperty_Integer(uID, PROPERTY_VOCATION);
	pActor->setHeroExclusiveTalentEffect(nHeroID);
}

bool CMobaWar::playerIsWin(PDBID idActor)
{
	ActorHelper actorHelper(idActor, ACTOR_DBID);
	IActorService * pActorService = actorHelper.m_ptr;
	if( pActorService == NULL ) return false;
	int nCamp = pActorService->getProperty_Integer(PROPERTY_CAMP);
	if ( nCamp == getWinCamp())
	{
		return true;
	}
	
	return false;
}


void CMobaWar::setPlayerGlory(PDBID idActor)
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

	SWPerson *pPerson = getPlayerInfoByPDBIU(idActor);
	if (pPerson != NULL)
	{
		nGlory[ENACHIEVE_CONTINUEKILLMAX] = pPerson->nKillTimeTitle;
		if (pPerson->nTitle & EWM_SCORE)
		{
			nGlory[ENACHIEVE_MVPNUM] += 1;
		}
		if (pPerson->nTitle & EWM_KILLCOUNT)
		{
			nGlory[ENACHIEVE_ONCESKILLCOUNT] += 1;
		}
		if (pPerson->nTitle & EWM_ASSCOUNT)
		{
			nGlory[ENACHIEVE_ONCEASSISTCOUNT] += 1;
		}
		if (pPerson->nTitle & EWM_DESTORYTOWER)
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
int CMobaWar::getAchievementScore( PDBID idActor )
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
int CMobaWar::getWarScoringRewards(PDBID idActor, const SHeroAchievementScoreConfig* pScoreConfig)
{
	SWPerson *pPerson = getPlayerInfoByPDBIU(idActor);
	if (pPerson == NULL || pScoreConfig == NULL)
	{
		ErrorLn(__FUNCTION__": at m_mapWarEndGraded can't find player idActor = " << idActor);
		return 0;
	}
	int nScore = pPerson->nComprehensiveScore;
	int nScoringRatio = (pScoreConfig->nScoringRatio == 0 ? 1 : pScoreConfig->nScoringRatio);
	return ((nScore / nScoringRatio) * pScoreConfig->nScoringRewards);
}

// 1/（1+10^（（期望成就-当前成就）/胜率基数）
float CMobaWar::getAchievementCoeff( int nCurrentScore, const SHeroAchievementScoreConfig* pScoreConfig )
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

void CMobaWar::recordTeammateHeroID(int nCamp,PDBID idActor, int nSelfHeroID)
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

void CMobaWar::setPlayerKillGloryInfo(PDBID idActor, int nCamp, int* pGlory)
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
void CMobaWar::OnExecute( WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen )
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

	case EVENT_GAME_ENTITY_FREEZE_CHANGED:  // 技能冷却改变事件
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

	default:
		break;
	}
}

void CMobaWar::onSkillFreesChangeed(event_freeze_changed* pEntityFreezeChanged)
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





void CMobaWar::onLearnSkill( event_learn_spell* entityLearnSpell )
{
	msg_player_skill_state msgSkillState;
	int nSpellFlag = entityLearnSpell->nSpellFlag;
	if ((nSpellFlag &SpellFlag_Space) != SpellFlag_Space)
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

bool CMobaWar::onWarClientMsg(UID uidActor, BYTE byKeyAction, PACKAGE_PTR msg)
{
    if (!CWar::onWarClientMsg(uidActor, byKeyAction, msg))
    {
        // 此类战场单独处理的客户端消息
        return false;
    }

    return true;
    
}

void CMobaWar::onWarRealStart()
{
	CWar::onWarRealStart();
}

bool CMobaWar::InitSubFunc()
{
	return CWar::InitSubFunc();
}

void CMobaWar::OnTimer(unsigned long dwTimerID)
{
	CWar::OnTimer(dwTimerID);
}

void CMobaWar::Release()
{
	//停止定时器
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_WarProcess, this);
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_DeadKillCount, this);
	//gServerGlobal->getTimerService()->KillTimer(ETimerEventID_WarActorLog, this );
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_EcmExpSituation, this );
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_EcmControlRatio, this );
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_EcmControlLine, this );

	CWar::Release();

    delete this;
}


#pragma region 随机英雄卡牌

// 返回可获得卡牌数量
int CMobaWar::getWarEndCardCount(PDBID idActor, int nCamp, bool bIsHaveSameClan, SEntityXml_Server* pEntityXml)
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

int CMobaWar::getHeroCardCount( int nPlayerLv, int nWarType )
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

void CMobaWar::ExpAndGoldCreate(PDBID idActor, bool bIsHaveSameClan, SEntityXml_Server* pEntityXml,SExpOrGoldCreateConfig* pConfig, msg_entity_warend_awark& msgAwark)
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
	int nCrownGemstoneExp = 0;

	const SchemeWarPersonConfigInfo* pSchemeWarPersonConfigInfo = gServerGlobal->getSchemeCenter()->getSchemeWarPersonConfigInfo()->GetSchemeWarPersonConfigInfo(getStaticID());
	if(pSchemeWarPersonConfigInfo)
		 nCrownGemstoneExp = pSchemeWarPersonConfigInfo->nAddGemstoneExp;

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
	msgAwark.nActorOldExp = pActor->getPlayerExp();
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
	ssprintf_s(szLogDesc, sizeof(szLogDesc), "战场[%I64d]获得奖励", (isFristWin?"首胜":""), getWarDBKey());

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

	pActor->setActorBaseInfo( nCrownGemstoneExp, ENPLAYERINFOTYPE_GEMSTONEEXP, ENCHANGEDATAMODE_ADD, OSS_NUMERIC_EXP, OSS_RESOURCE_OPT_ADD, OSSRES_ADD_WAR_PRIZE, idActor, szLogDesc, 0);


	msgAwark.nActorGetExp = nPlayerExp;
	msgAwark.nHeroExp = nHeroExp;
	msgAwark.nGold = nGetGold;
	msgAwark.nSuccessScore = nGetAchievement;
}

// 经验值 = (   (召唤师等级-经验_初始等级(胜利))   *    经验_斜率等级成长(胜利)+经验_初始斜率(胜利)   )*(游戏时长-经验_时间临界点(胜利))  +   经验_临界点经验值(胜利)
// 经验值 = （（ 召唤师等级-经验_初始等级(失败)）  *    经验_斜率等级成长(失败)+经验_初始斜率(失败)   )*（游戏时长  - 经验_时间临界点(失败) ）+  经验_临界点经验值(失败)
int CMobaWar::getStaticExp(PDBID idActor, int nType, SExpOrGoldCreateConfig* pConfig)
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
		fExp = fExp * ( (float)getWarContinueTime()/60.0f - pConfig->fWinCoeff[ENECPORGOLDCOEFF_EXP_TIMECRITICAL]);
		fExp = fExp + pConfig->fWinCoeff[ENECPORGOLDCOEFF_EXP_CRITICALEXP];
	}else
	{
		fExp = ((float)nPlayerLv - pConfig->fFailCoeff[ENECPORGOLDCOEFF_EXP_BASELV]) * pConfig->fFailCoeff[ENECPORGOLDCOEFF_EXP_SLOPEGROW] + pConfig->fFailCoeff[ENECPORGOLDCOEFF_EXP_BASESLOPE];
		fExp = fExp * ( (float)getWarContinueTime()/60.0f - pConfig->fFailCoeff[ENECPORGOLDCOEFF_EXP_TIMECRITICAL]);
		fExp = fExp + pConfig->fFailCoeff[ENECPORGOLDCOEFF_EXP_CRITICALEXP];
	}
	return (int)fExp;
}

//金币数量 = 时间系数（胜利） * 游戏时长 + 匹配模式加成
//金币数量 = 时间系数 （失败）* 游戏时长 + 匹配模式加成
int CMobaWar::getStaticGold( bool bIsWin, int nType )
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
	nGold = (int)(fCoeff * ((float)getWarContinueTime()/60.0f) + pConfig->fGoldAddition);
#ifdef PRINTF_PLANLOG
	//WarningLn(__FUNCTION__": Planning requirements : gongshi =>  nGold = fCoeff * ((float)m_continueTime/60.0f) + pConfig->fGoldAddition");
	//WarningLn(__FUNCTION__": Planning requirements : fCoeff = " << fCoeff << ", ((float)m_continueTime/60.0f) = " << (float)m_continueTime/60.0f <<  ", pConfig->fGoldAddition = " << pConfig->fGoldAddition );
#endif // PRINTF_PLANLOG
	return nGold;
}
#pragma endregion 经验金币产出

void CMobaWar::onHeroDieRemovBuff( UID uid)
{
	SEntityXml_Server* pEntityXml = g_EHelper.getEntityXml();
	if (pEntityXml == NULL)
		return;

	if(UID_2_TYPE(uid) != TYPE_PLAYER_ROLE || pEntityXml->nDeadIsDisappear )
	{
		return;
	}
	PDBID pDBID = getProperty_Integer(uid, PROPERTY_ID);

	IXPSkillMgr* pXPSkillMgr = (IXPSkillMgr*)getWarMgr(Mgr_XPSKill);
	if (pXPSkillMgr != NULL)
	{
		pXPSkillMgr->removeXPBuff(uid, pDBID);
	}
}