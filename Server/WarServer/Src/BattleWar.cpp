/*******************************************************************
** 文件名: BattleWar.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.q1.com)
** 创建人: 李有红
** 日  期: 2017/10/31
** 版  本: 1.0
** 描  述: 大逃杀战场
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/


#include "StdAfx.h"
#include "BattleWar.h"
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
#include "BattleWar_SafetyZone.h"
#include "ExternalHelper.h"
#include "WarAddBuff.h"
#include "WarDrop.h"
#include "RandRefreshMonsMgr.h"
#include "MonsterDiedExecMgr.h"
#include "WarMonsterMgr.h"
#include "DevilKingMgr.h"
#include "HeroGenicMgr.h"
#include <math.h>

//#define __WAR__HERO_GET_EXP__TALENT__DEBUG__
#ifndef __WAR__HERO_GET_EXP__TALENT__DEBUG__

#endif

CBattleWar::CBattleWar()
{
    
}

CBattleWar::~CBattleWar()
{

}

// 初始化
bool CBattleWar::create(IWarService* pWarService,SWarCreateContext& context)
{
	return CWar::create(pWarService, context);
}

// 初始化
bool CBattleWar::Init()
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
	    pTimerService->SetTimer(ETimerEventID_EcmControlRatio,  WarEcmContorlRatioTime, this );
		pTimerService->SetTimer(ETimerEventID_EcmControlLine,  WarEcmControlLineTime, this );

		// 定时存储比赛过程中人物的对战信息
		/*	SEntityXml_Server* pXmlConfig = g_EHelper.getEntityXml();
		if (pXmlConfig && pXmlConfig->nRecordInfo==1 && pXmlConfig->nTime > 0)
		{
		pTimerService->SetTimer(ETimerEventID_WarActorLog, pXmlConfig->nTime, this);
		}*/
		//pTimerService->SetTimer(ETimerEventID_EcmExpSituation,  WarEcmExpSituationTime, this );
    }

	return true;
}


// 反初始化
void CBattleWar::UnInit()
{
	m_mapLearnSkillInfo.clear();

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
void CBattleWar::End()
{
    if (isEnd())
    {
        return;
    }

	SWar2ViewMsgAce msg;
	msg.nCamp = m_WarInfo.nWinCamp;
	msg.nRank = 1;
	msg.nKillNum = getCampKillNum(m_WarInfo.nWinCamp);
	msg.isWin = 1;
	broadCastDataToSameCamp(WAR2VIEW_MSG_BATTLEWAR_ACE, (PCSTR)&msg, sizeof(msg), m_WarInfo.nWinCamp, true);

	CWar::End();

	//停止定时器
	ITimerService * pTimerService = gServerGlobal->getTimerService();
	if(pTimerService)
	{
		pTimerService->KillTimer(ETimerEventID_WarProcess, this);
		pTimerService->KillTimer(ETimerEventID_DeadKillCount, this);
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


void CBattleWar::addPlayerData()
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

void CBattleWar::setBcastProcessTimer()
{
	for (int i = 0; i < m_nBroadcastProcessCount; ++i)
	{
		TraceLn(__FUNCTION__": timerID =" << ETimerEventID_Max + i);
		gServerGlobal->getTimerService()->SetTimer(ETimerEventID_Max + i, m_BroadCastTimeList[i].BroadCastTime * 1000, this, 1, __FILE__);
	}
}

void CBattleWar::addPersonExp(UID uid, int nAddExp, int nExpDropType)
{
	SWPerson* pPerson = getPlayerInfo(uid);
	if (pPerson == NULL)
	{
		return;
	}

	if (pPerson->lifeState == ELS_Dead)
	{
		return;
	}

	CWar::addPersonExp(uid, nAddExp, nExpDropType);
}

void CBattleWar::addPersonTalentPoint(UID uid, int nTalentPoint, int nTalentDropType)
{
	SWPerson* pPerson = getPlayerInfo(uid);
	if (pPerson == NULL)
	{
		return;
	}

	if (pPerson->lifeState == ELS_Dead)
	{
		return;
	}

	CWar::addPersonTalentPoint(uid, nTalentPoint, nTalentDropType);
}

void CBattleWar::onFakeDeadRelive(UID uid)
{
	SWPerson* pPerson = getPlayerInfo(uid);
	if (pPerson == NULL)
		return;

	pPerson->lifeState = ELS_Alive;

	onUpdatePersonLifeState(pPerson);

	InfoLn(__FUNCTION__":" << pPerson->szName);
}


void CBattleWar::onGetDamage(event_entity_damage damage)
{
	CWar::onGetDamage(damage);
	
	// 处理英雄收到的伤害
	if (UID_2_TYPE(damage.uidTarget) == TYPE_PLAYER_ROLE)
	{
		// 处理假死的情况
		if (damage.bFakeDead)
		{
			onPlayerFakeDie(damage.uidTarget, damage.uidOperator);
		}
	}
}


void CBattleWar::onPlayerContinuePlay(event_entity_continue_play * pEvent)
{
	CWar::onPlayerContinuePlay(pEvent);

	SWPerson* pPerson = getPlayerInfoByPDBIU(pEvent->dwDBID);
	if (pPerson == NULL)
		return;
}

void CBattleWar::setHeroExclusiveTalentEffect(UID uID)
{
	ActorHelper actor(uID);
	IActorService* pActor = actor.m_ptr;
	if ( pActor == NULL )
		return;
	int nHeroID = getProperty_Integer(uID, PROPERTY_VOCATION);
	pActor->setHeroExclusiveTalentEffect(nHeroID);
}

bool CBattleWar::playerIsWin(PDBID idActor)
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


void CBattleWar::setPlayerGlory(PDBID idActor)
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
int CBattleWar::getAchievementScore( PDBID idActor )
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
int CBattleWar::getWarScoringRewards(PDBID idActor, const SHeroAchievementScoreConfig* pScoreConfig)
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
float CBattleWar::getAchievementCoeff( int nCurrentScore, const SHeroAchievementScoreConfig* pScoreConfig )
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

void CBattleWar::recordTeammateHeroID(int nCamp,PDBID idActor, int nSelfHeroID)
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

void CBattleWar::setPlayerKillGloryInfo(PDBID idActor, int nCamp, int* pGlory)
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
void CBattleWar::OnExecute( WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen )
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

void CBattleWar::onSkillFreesChangeed(event_freeze_changed* pEntityFreezeChanged)
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

void CBattleWar::onUpdatePersonLifeState(SWPerson* pPerson)
{
	if (pPerson == NULL)
		return;

	// 没有团灭则发队友死亡消息给客户端
	SWar2ViewMsgPersonLifeState msg;
	msg.nCamp = pPerson->nCamp;
	msg.nLifeSate = pPerson->lifeState;
	msg.pdbid = pPerson->uPDBID;

	broadCastDataToSameCamp(WAR2VIEW_MSG_BATTLEWAR_LIFESTATE, (PCSTR)&msg, sizeof(msg), pPerson->nCamp, true);
}

void CBattleWar::onAce(int nCamp)
{
	int nAliveCampCount = getAliveCampCount();
	SWar2ViewMsgAce msg;
	msg.nCamp = nCamp;
	msg.nRank = nAliveCampCount + 1;
	msg.nKillNum = getCampKillNum(nCamp);
	msg.isWin = 0;

	broadCastDataToSameCamp(WAR2VIEW_MSG_BATTLEWAR_ACE, (PCSTR)&msg, sizeof(msg), nCamp, true);
}


void CBattleWar::onLearnSkill( event_learn_spell* entityLearnSpell )
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



bool CBattleWar::onWarClientMsg(UID uidActor, BYTE byKeyAction, const char* pData, size_t nLen)
{
	CWar::onWarClientMsg(uidActor, byKeyAction, pData, nLen);

	switch (byKeyAction)
	{
	case CS_MSG_WAR_REQ_EXIT_WAR:   // 玩家主动退出战场
		{
			onClientReqExitWar(uidActor);
		}
		break;
	default:break;
	}

    return true;
    
}

void CBattleWar::onWarRealStart()
{
	CWar::onWarRealStart();

	for (int i = 0; i < Mgr_Max; ++i)
	{
		__IWarMgr* pWarMgr = m_WarMgrList[i];
		if (pWarMgr != NULL)
		{
			pWarMgr->onWarRealStart();
		}
	}
}

bool CBattleWar::InitSubFunc()
{
	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
	{
		ErrorLn(__FUNCTION__ " Get ISchemeCenter failed.");
		return false;
	}

	ISchemeWarSubFunc* pWarSubFunc = pSchemeCenter->getSchemeWarSubFunc();
	if (pWarSubFunc == NULL)
	{
		ErrorLn(__FUNCTION__ " Get ISchemeWarSubFunc failed.");
		return false;
	}

	int nMatchType = getMatchTypeID();
	///////////////////////////////////以下功能不需要一定要启动成功，因为是可选///////////////////
	// 战场自动添加Buff
	IWarAddBuffMgr* pWarAddBuffMgr = new CWarAddBuff(this);
	if (pWarAddBuffMgr->Init(nMatchType) == false)
	{
		WarningLn(__FUNCTION__ " Init WarAddBuff failed Or closed this function");
		pWarAddBuffMgr->Release();
		pWarAddBuffMgr = NULL;
	}
	m_WarMgrList[Mgr_AddBuff] = pWarAddBuffMgr;
	
	////////////////////////////////////////////////////////////

	// 战场掉落
	IWarDropMgr* pWarDropMgr = new CWarDrop(this);
	if (pWarDropMgr->Init(nMatchType) == false)
	{
		ErrorLn(__FUNCTION__ " Init WarDrop failed.");
		pWarDropMgr->Release();
		pWarDropMgr = NULL;
		return false;
	}
	m_WarMgrList[Mgr_Drop] = pWarDropMgr;

	// 安全圈玩法
	ISafetyZoneMgr* pSafetyZoneMgr = new CBattleWarSafetyZone(this);
	if (pSafetyZoneMgr->Init(nMatchType) == false)
	{
		WarningLn(__FUNCTION__ " Init BattleWarSafetyZone failed.");
		pSafetyZoneMgr->Release();
		pSafetyZoneMgr = NULL;
		return false;
	}
	m_WarMgrList[Mgr_SafetyZone] = pSafetyZoneMgr;

	// 怪物管理
	IWarMonsterMgr* pWarMonsterMgr = new CWarMonsterMgr(this);
	if (pWarMonsterMgr->Init(nMatchType) == false)
	{
		WarningLn(__FUNCTION__ " Init CWarMonsterMgr failed.");
		pWarMonsterMgr->Release();
		pWarMonsterMgr = NULL;
		return false;
	}
	m_WarMgrList[Mgr_Monster] = pWarMonsterMgr;

	// 怪物死亡处理
	IMonsterDiedExecMgr* pMonsterDiedExecMgr = new CMonsterDiedExecMgr(this);
	if (pMonsterDiedExecMgr->Init(nMatchType) == false)
	{
		WarningLn(__FUNCTION__ " Init CMonsterDiedExecMgr failed.");
		pMonsterDiedExecMgr->Release();
		pMonsterDiedExecMgr = NULL;
		return false;
	}
	m_WarMgrList[Mgr_MonsterDiedExec] = pMonsterDiedExecMgr;

	// 随机刷怪
	IRandRefreshMonsMgr* pRandRefreshMonsMgr = new CRandRefreshMonsMgr(this);
	if (pRandRefreshMonsMgr->Init(nMatchType) == false)
	{
		WarningLn(__FUNCTION__ " Init CRandRefreshMonsMgr failed.");
		pRandRefreshMonsMgr->Release();
		pRandRefreshMonsMgr = NULL;
		return false;
	}
	m_WarMgrList[Mgr_RandRefreshMons] = pRandRefreshMonsMgr;

	// 魔王玩法
	IDevilKingMgr* pDevilKingMgr = new CDevilKingMgr(this);
	if (pDevilKingMgr->Init(nMatchType) == false)
	{
		WarningLn(__FUNCTION__ " Init CDevilKingMgr failed.");
		pDevilKingMgr->Release();
		pDevilKingMgr = NULL;
		return false;
	}
	m_WarMgrList[Mgr_DevilKing] = pDevilKingMgr;

	// 英雄基因玩法
	IHeroGenicMgr* pHeroGenicMgr = new CHeroGenicMgr(this);
	if (pHeroGenicMgr->Init(nMatchType) == false)
	{
		WarningLn(__FUNCTION__ " Init CHeroGenicMgr failed.");
		pHeroGenicMgr->Release();
		pHeroGenicMgr = NULL;
		return false;
	}
	m_WarMgrList[Mgr_HeroGenic] = pHeroGenicMgr;

	return true;
}

void CBattleWar::forceEndWar(int nCamp, bool bNormalEnd)
{
	m_WarInfo.bNormalEnd = bNormalEnd;

	setWinCamp(nCamp);

	// 结束战场
	End();
}

void CBattleWar::setReliveInfo(sEntityDieInfo entityDieInfo, SEntityReliveInfo & info)
{
	SWPerson* pDiedPerson = getPlayerInfo(entityDieInfo.uDiedUID);
	if (pDiedPerson == NULL)
	{
		ErrorLn(__FUNCTION__": pDiedPerson == NULL uDiedUID =" << entityDieInfo.uDiedUID);
		return;
	}

	// 设置复活信息
	info.bNoRelive = true;
}

void CBattleWar::onEntityDieEx(sEntityDieInfo entityDieInfo, SEntityReliveInfo & info)
{
	if (INVALID_UID == entityDieInfo.uDiedUID || INVALID_UID == entityDieInfo.uMurderUID)
	{
		//不知道谁杀死的
		ErrorLn(__FUNCTION__": -- uDiedUID =" << entityDieInfo.uDiedUID << " uMurderUID =" << entityDieInfo.uMurderUID);
		return;
	}

	// 玩家死亡
	if (UID_2_TYPE(entityDieInfo.uDiedUID) == TYPE_PLAYER_ROLE)
	{
		// 设置玩家复活信息
		setReliveInfo(entityDieInfo, info);
		setSumDieTime(info.nActiveInterval, entityDieInfo);

		// 玩家死亡处理
		if (UID_2_TYPE(entityDieInfo.uMurderUID) == TYPE_PLAYER_ROLE)
		{
			if (entityDieInfo.uMurderUID == entityDieInfo.uDiedUID)
			{
				// 玩家自己杀自己
				onPlayerDieBySelf(entityDieInfo);
			}
			else
			{
				// 玩家死亡
				onPlayerDie(entityDieInfo);
			}
		}
		else
		{
			// 怪物击杀
			onPlayerDieByMonster(entityDieInfo);
		}

		IDevilKingMgr* pDevilKingMgr = (IDevilKingMgr*)getWarMgr(Mgr_DevilKing);
		if (pDevilKingMgr != NULL)
		{
			// 玩家死亡
			pDevilKingMgr->onPlayerDie();
		}
	}
	else
	{
		ErrorLn(__FUNCTION__": -- UID_2_TYPE(uidMaster) != TYPE_PLAYER_ROLE uidMaster =" << entityDieInfo.uDiedUID);
	}
}

void CBattleWar::cureMember(UID uid)
{
	if (m_context.byMode != EGM_MCAMP)
		return;

	SWPerson* pPerson = getPlayerInfo(uid);
	if (pPerson == NULL)
		return;

	SWarCamp* pWarCamp = getWarCamp(pPerson->nCamp);
	if (pWarCamp == NULL)
		return;

	Vector3 selfLoc = getLocation(uid);

	WarPersonVector::iterator iter = pWarCamp->warPersVect.begin();
	for (; iter != pWarCamp->warPersVect.end(); ++iter)
	{
		if (iter->uID == uid)
			continue;

		if (iter->lifeState == ELS_FakeDead)
		{
			bool isSucces = Transport(iter->uID, selfLoc, Vector3(-1, -1, -1), true, 0, TRANSPORT_REASON_REBORN);

			//SEntityXml_Server* pEntityXml = g_EHelper.getEntityXml();
			//if (pEntityXml != NULL)
			//{
				//AddBuff(iter->uID, pEntityXml->nCureFakeDeadPlayerBuffID, 1, uid);
			//}
		}
	}
}



void CBattleWar::OnTimer(unsigned long dwTimerID)
{
	CWar::OnTimer(dwTimerID);

}

void CBattleWar::Release()
{
	//停止定时器
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_WarProcess, this);
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_DeadKillCount, this);
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_EcmControlRatio, this );
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_EcmControlLine, this );

	CWar::Release();

    delete this;
}


#pragma region 随机英雄卡牌

// 返回可获得卡牌数量
int CBattleWar::getWarEndCardCount(PDBID idActor, int nCamp, bool bIsHaveSameClan, SEntityXml_Server* pEntityXml)
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

int CBattleWar::getHeroCardCount( int nPlayerLv, int nWarType )
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

void CBattleWar::ExpAndGoldCreate(PDBID idActor, bool bIsHaveSameClan, SEntityXml_Server* pEntityXml,SExpOrGoldCreateConfig* pConfig, msg_entity_warend_awark& msgAwark)
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
int CBattleWar::getStaticExp(PDBID idActor, int nType, SExpOrGoldCreateConfig* pConfig)
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
int CBattleWar::getStaticGold( bool bIsWin, int nType )
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

void CBattleWar::onHeroDieRemovBuff( UID uid)
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
	if(pXPSkillMgr != NULL)
		pXPSkillMgr->removeXPBuff(uid, pDBID);
}

// 会调用这个方法的情况
// 情况1：玩家A被敌方打成假死状态，后来救治时间超时，玩家A给自己加死亡Buff，该情况uidDead和uidMurder都是玩家A,nMurderOfFakeDead是敌方
// 情况2：玩家A被敌方打成假死状态，后来检测到队伍成员都是非Alive状态,则敌方会给玩家A加死亡Buff，此时uidDead是玩家A，uidMurder是敌方,nMurderOfFakeDead是敌方
// 情况3：玩家A在毒区被毒成假死状态，后来检测到队伍成员都是非Alive状态，则玩家A给自己加死亡buff，此时uidDead和uidMurder都是玩家A,nMurderOfFakeDead是玩家A
void CBattleWar::onPlayerDie(const sEntityDieInfo& entityDieInfo)
{
	UID uidDead = entityDieInfo.uDiedUID; 
	int nDeadCamp = entityDieInfo.nDiedCamp;
	UID uidMurder = entityDieInfo.uMurderUID;

	if (uidDead == uidMurder)
		return;
	
	// 处理掉落
	IWarDropMgr* pWarDropMgr = (IWarDropMgr*)getWarMgr(Mgr_Drop);
	if (pWarDropMgr != NULL)
	{
		pWarDropMgr->dealPersonDie(entityDieInfo);
	}

	SWPerson* pDeadPerson = getPlayerInfo(uidDead);
	if (pDeadPerson == NULL)
		return;

	SWPerson* pMurderPerson = getPlayerInfo(uidMurder);
	if (pMurderPerson == NULL)
		return;

	pDeadPerson->lifeState = ELS_Dead;

	// 死亡的人需要广播给其他助攻或者击杀 天赋点改变事件会广播
	if (getWarPhase() != EWP_End)
	{
		sendAllPersonInfoToAllClient();
	}

	onUpdatePersonLifeState(pDeadPerson);

	// 该阵营是否团灭，团灭则发团灭消息给客户端
	if (judgeCampIsAce(nDeadCamp))
	{
		onAce(nDeadCamp);
	}

	// 玩家杀人了
	IDevilKingMgr* pDevilKingMgr = (IDevilKingMgr*)getWarMgr(Mgr_DevilKing);
	if (pDevilKingMgr != NULL)
	{
		pDevilKingMgr->onPlayerKillPerson(uidMurder, uidDead);
	}

	// 击杀人数提示
	char szBuf[64] = { 0 };
	ssprintf_s(szBuf, sizeof(szBuf), "%d", getAliveTotalNum());
	g_EHelper.showSystemMessageToClient(pMurderPerson->uPDBID, CHAT_TIP_WAR_BATTLEWAR_KILLTIP, szBuf);

	// 击杀者获得新的基因
	IHeroGenicMgr* pHeroGenicMgr = (IHeroGenicMgr*)getWarMgr(Mgr_HeroGenic);
	if (pHeroGenicMgr != NULL)
	{
		pHeroGenicMgr->onNewGenic(uidMurder, pDeadPerson->nHeroID, pDeadPerson->nHeroSkinID, pDeadPerson->szName);
	}

	// 检测游戏结束
	checkGameOver();
}

// 会调用这个方法的情况
// 情况1：玩家A被野怪打死，uidDead是玩家A，uidMurder是野怪，
void CBattleWar::onPlayerDieByMonster(const sEntityDieInfo & entityDieInfo)
{
	UID uidDead = entityDieInfo.uDiedUID;
	int nDeadCamp = entityDieInfo.nDiedCamp;
	UID uidMurder = entityDieInfo.uMurderUID;

	// 处理掉落
	IWarDropMgr* pWarDropMgr = (IWarDropMgr*)getWarMgr(Mgr_Drop);
	if (pWarDropMgr != NULL)
	{
		pWarDropMgr->dealPersonDie(entityDieInfo);
	}

	SWPerson* pDeadPerson = getPlayerInfo(uidDead);
	if (pDeadPerson == NULL)
		return;

	pDeadPerson->lifeState = ELS_Dead;

	// 死亡的人需要广播给其他助攻或者击杀 天赋点改变事件会广播
	if (getWarPhase() != EWP_End)
	{
		sendAllPersonInfoToAllClient();
	}

	onUpdatePersonLifeState(pDeadPerson);

	// 该阵营是否团灭，团灭则发团灭消息给客户端
	if(judgeCampIsAce(nDeadCamp))
	{
		onAce(nDeadCamp);
	}

	// 检测游戏结束
	checkGameOver();
}

void CBattleWar::onPlayerDieBySelf(const sEntityDieInfo & entityDieInfo)
{
	UID uidDead = entityDieInfo.uDiedUID;
	int nDeadCamp = entityDieInfo.nDiedCamp;
	UID uidMurder = entityDieInfo.uMurderUID;

	if (uidDead != uidMurder)
		return;

	// 过滤自己杀自己,不处理掉落
	SWPerson* pPerson = getPlayerInfo(uidDead);
	if (pPerson == NULL)
		return;

	// 处理自己杀自己的情况（情况1：毒圈中毒，情况2：假死后救治时间过了给自己加死亡buff）
	if (pPerson->lifeState == ELS_FakeDead)
	{
		SWPerson* pMurder = NULL;

		// 不是自己把自己打成假死状态，则给击杀者加成
		if (uidDead != pPerson->nMurderOfFakeDead)
		{
			pMurder = getPlayerInfo(pPerson->nMurderOfFakeDead);
		}
		else
		{
			// 有人攻击过自己，则他为击杀者
			if (isValidUID(pPerson->uLastAttackUID))
			{
				pMurder = getPlayerInfo(pPerson->uLastAttackUID);
			}
			else
			{
				InfoLn(__FUNCTION__ << a2utf8("自己杀自己且没人攻击过自己，则不处理掉落"));
			}
		}

		if (pMurder != NULL)
		{
			sEntityDieInfo tmpEntityDieInfo = entityDieInfo;
			tmpEntityDieInfo.nMurderCamp = pMurder->nCamp;
			tmpEntityDieInfo.nMurderVocation = pMurder->nHeroID;
			tmpEntityDieInfo.uMurderPropertyID = pMurder->uPDBID;
			tmpEntityDieInfo.uMurderUID = pMurder->uID;
			sstrcpyn(tmpEntityDieInfo.szMurderName, pMurder->szName, sizeof(tmpEntityDieInfo.szMurderName));

			// 处理掉落
			IWarDropMgr* pWarDropMgr = (IWarDropMgr*)getWarMgr(Mgr_Drop);
			if (pWarDropMgr != NULL)
			{
				pWarDropMgr->dealPersonDie(tmpEntityDieInfo);
			}

			// 击杀者获得新的基因
			IHeroGenicMgr* pHeroGenicMgr = (IHeroGenicMgr*)getWarMgr(Mgr_HeroGenic);
			if (pHeroGenicMgr != NULL)
			{
				pHeroGenicMgr->onNewGenic(pMurder->uID, pPerson->nHeroID, pPerson->nHeroSkinID, pPerson->szName);
			}

			// 玩家杀人了
			IDevilKingMgr* pDevilKingMgr = (IDevilKingMgr*)getWarMgr(Mgr_DevilKing);
			if (pDevilKingMgr != NULL)
			{
				pDevilKingMgr->onPlayerKillPerson(pMurder->uID, uidDead);
			}
		}
		else
		{
			// 玩家自杀死亡
			IDevilKingMgr* pDevilKingMgr = (IDevilKingMgr*)getWarMgr(Mgr_DevilKing);
			if (pDevilKingMgr != NULL)
			{
				pDevilKingMgr->onPlayerKillPerson(uidDead, uidDead);
			}

			pPerson->nDeadNum++;
		}
	}

	pPerson->lifeState = ELS_Dead;

	// 死亡的人需要广播给其他助攻或者击杀 天赋点改变事件会广播
	if (getWarPhase() != EWP_End)
	{
		sendAllPersonInfoToAllClient();
	}

	onUpdatePersonLifeState(pPerson);

	// 该阵营是否团灭，团灭则发团灭消息给客户端
	if (judgeCampIsAce(nDeadCamp))
	{
		onAce(nDeadCamp);
	}

	// 被毒死提示
	if (pPerson->nMurderOfFakeDead == uidDead)
	{
		char szBuf[128] = { 0 };
		ssprintf_s(szBuf, sizeof(szBuf), "%s", pPerson->szName);
		g_EHelper.showSystemMessageToChatBox(getWarSceneID(), CHAT_TIP_WAR_BATTLEWAR_DIE_IN_NOX, szBuf);
	}

	// 检测游戏结束
	checkGameOver();
}

// 假死情况
// 情况1：玩家A被敌方打成假死状态，uidTarget为玩家A，uidMurder为敌方
// 情况2：玩家A在毒区被毒成假死状态，uidTarget和uidMurder都是玩家A
void CBattleWar::onPlayerFakeDie(UID uidTarget, UID uidMurder)
{
	SWPerson* pPerson = getPlayerInfo(uidTarget);
	if (pPerson == NULL)
		return;

	// 是否已经假死
	//if (pPerson->lifeState == ELS_FakeDead)
	//{
		//WarningLn("Already ELS_FakeDead,uidTarget=" << uidTarget << ",uidMurder=" << uidMurder);
		//return;
	//}

	SWarCamp* pWarCamp = getWarCamp(pPerson->nCamp);
	if (pWarCamp == NULL)
		return;

	// 设置凶手
	pPerson->nMurderOfFakeDead = uidMurder;

	// 设置生命状态为假死
	pPerson->lifeState = ELS_FakeDead;

	int nAliveCount = getAliveNum(pPerson->nCamp);

	// 更新人物生命状态
	onUpdatePersonLifeState(pPerson);

	//团灭
	if (nAliveCount == 0)
	{
		// 把该阵营的人都自杀死亡
		WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
		for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
		{
			if (itPerson->lifeState == ELS_FakeDead)
			{
				SEntityXml_Server* pEntityXml = g_EHelper.getEntityXml();
				if (pEntityXml != NULL)
				{
					AddBuff(itPerson->uID, pEntityXml->nKillFakeDeadPlayerBuffID, 1, itPerson->nMurderOfFakeDead);
				}
				itPerson->bIsKillSelf = true;
			}
		}
	}

	// 击倒提示
	char szBuf[64] = { 0 };
	ssprintf_s(szBuf, sizeof(szBuf), "%s", pPerson->szName);
	g_EHelper.showSystemMessageToClient(pPerson->uPDBID, CHAT_TIP_WAR_BATTLEWAR_FAKED_KILL, szBuf);
}

void CBattleWar::checkGameOver()
{
	if (m_mapWarCamp.empty())
	{
		ErrorLn(__FUNCTION__":m_mapWarCamp is empty.");
		return;
	}

	// 判断是否结束游戏（只有一个阵营存活）
	int nTotalCampCount = m_mapWarCamp.size();
	int nAceCampCount = 0;
	WarCampMap::iterator itCamp = m_mapWarCamp.begin();
	int nWinCamp = itCamp->first;
	for (; itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		if (itCamp->second.isAce || itCamp->second.warPersVect.empty())
		{
			nAceCampCount++;
		}
		else
		{
			nWinCamp = itCamp->first;
		}
	}

	// 还剩一个阵营存活，则游戏结束
	if (nTotalCampCount - nAceCampCount <= 1)
	{
		m_WarInfo.bNormalEnd = true;

		setWinCamp(nWinCamp);

		End();
	}
}

int CBattleWar::getAliveCampCount()
{
	int nCount = 0;
	
	WarCampMap::iterator iter = m_mapWarCamp.begin();
	for (; iter != m_mapWarCamp.end(); ++iter)
	{
		if (iter->second.warPersVect.empty())
			continue;

		if (iter->second.isAce == false)
		{
			nCount++;
		}
	}

	return nCount;
}

int CBattleWar::getCampKillNum(int nCamp)
{
	SWarCamp* pWarCamp = getWarCamp(nCamp);
	if (pWarCamp == NULL)
		return 0;

	int nTotalKillNum = 0;
	WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
	for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
	{
		nTotalKillNum += itPerson->nBaseRecord[EDT_KillCount];
	}

	return nTotalKillNum;
}

int CBattleWar::getAliveNum(int nCamp)
{
	SWarCamp* pWarCamp = getWarCamp(nCamp);
	if (pWarCamp == NULL)
		return 0;

	int nAliveCount = 0;
	WarPersonVector::iterator itPerson = pWarCamp->warPersVect.begin();
	for (; itPerson != pWarCamp->warPersVect.end(); ++itPerson)
	{
		if (itPerson->lifeState == ELS_Alive)
		{
			nAliveCount++;
		}
	}

	return nAliveCount;
}

int CBattleWar::getAliveTotalNum()
{
	int nCount = 0;
	WarCampMap::iterator itCamp = m_mapWarCamp.begin();
	for (; itCamp != m_mapWarCamp.end(); ++itCamp)
	{
		WarPersonVector::iterator itPerson = itCamp->second.warPersVect.begin();
		for (; itPerson != itCamp->second.warPersVect.end(); ++itPerson)
		{
			if (itPerson->lifeState != ELS_Dead)
			{
				nCount++;
			}
		}
	}

	return nCount;
}

void CBattleWar::sendImportDieInfoToClient(UID uidTarget, UID uidMuder)
{
	msg_war_important_entiy_dieinfo msgEntiyDieInfo;
	msgEntiyDieInfo.MasterEntityType = TYPE_PLAYER_ROLE;
	msgEntiyDieInfo.MurdererEntityType = TYPE_PLAYER_ROLE;
	msgEntiyDieInfo.uidMaster = uidTarget;
	msgEntiyDieInfo.uidMurderer = uidMuder;
	msgEntiyDieInfo.nAssistCount = 0;

	SWPerson* pTargetPerson = getPlayerInfo(uidTarget);
	if (pTargetPerson == NULL)
	{
		return;
	}

	SWPerson* pMuderPerson = getPlayerInfo(uidMuder);
	if (pMuderPerson == NULL)
	{
		return;
	}

	sstrcpyn(msgEntiyDieInfo.MasterName, pTargetPerson->szName, sizeof(msgEntiyDieInfo.MasterName));
	msgEntiyDieInfo.MasterHeroID = pTargetPerson->nHeroID;

	msgEntiyDieInfo.nCamp = pMuderPerson->nCamp;
	msgEntiyDieInfo.MurdererHeroID = pMuderPerson->nHeroID;
	sstrcpyn(msgEntiyDieInfo.MurdererName, pMuderPerson->szName, sizeof(msgEntiyDieInfo.MurdererName));


	// 击杀、死亡连杀信息
	vector<msg_war_entity_assist_info> assistList;	// 助攻者列表

	sendToClientImportantEntityDie(msgEntiyDieInfo, assistList);
}

void CBattleWar::onClientReqExitWar(UID uidActor)
{
	SWPerson* pPerson = getPlayerInfo(uidActor);
	if (pPerson == NULL)
	{
		return;
	}

	if (pPerson->lifeState != ELS_Dead)
	{
		// 非死亡状态主动退出战场，把玩家当做死亡处理
		sEntityDieInfo dieInfo;
		dieInfo.uDiedUID = uidActor;
		dieInfo.nDiedCamp = pPerson->nCamp;
		dieInfo.uMurderUID = uidActor;
		dieInfo.nMurderCamp = pPerson->nCamp;
		onPlayerDieBySelf(dieInfo);
	}

	MatchSceneHelper matchHelper;
	IMatchSceneService* pMatchService = matchHelper.m_ptr;
	if (pMatchService != NULL)
	{
		pMatchService->accordLeaveWar(pPerson->uPDBID);
	}
}
