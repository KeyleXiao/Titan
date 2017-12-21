/*******************************************************************
** 文件名: MobaGuideWar.cpp
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/4/1
** 版  本: 1.0
** 描  述: 新手引导战场
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#include <math.h>
#include "MobaGuideWar.h"
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
#include "TaskStorageDef.h"
#include "ITimerService.h"
#include "ExternalHelper.h"

//#define __WAR__HERO_GET_EXP__TALENT__DEBUG__
#ifndef __WAR__HERO_GET_EXP__TALENT__DEBUG__

#endif

CMobaGuideWar::CMobaGuideWar()
{
}

CMobaGuideWar::~CMobaGuideWar()
{

}

// 初始化
bool CMobaGuideWar::create(IWarService* pWarService,SWarCreateContext& context)
{
	return CWar::create(pWarService, context);
}

// 初始化
bool CMobaGuideWar::Init()
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
		pEventEngine->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_CONTINUE_PLAY,	TYPE_PLAYER_ROLE, 0, __FUNCTION__);
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
        //SEntityXml_Server* pXmlConfig = g_EHelper.getEntityXml();
        //if (pXmlConfig && pXmlConfig->nRecordInfo==1 && pXmlConfig->nTime > 0)
        //{
        //    pTimerService->SetTimer(ETimerEventID_WarActorLog, pXmlConfig->nTime, this);
        //}
		pTimerService->SetTimer(ETimerEventID_EcmExpSituation,  WarEcmExpSituationTime, this );
	    pTimerService->SetTimer(ETimerEventID_EcmControlRatio,  WarEcmContorlRatioTime, this );
		pTimerService->SetTimer(ETimerEventID_EcmControlLine,  WarEcmControlLineTime, this );
    }

	return true;
}


// 反初始化
void CMobaGuideWar::UnInit()
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
		pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ACTOR_ACTIVE_TALENT,     TYPE_PLAYER_ROLE, 0);
		pEventEngine->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_GAME_ADD_ASSIST_BUFF,			TYPE_PLAYER_ROLE, 0);
	}

	//停止定时器
	ITimerService * pTimerService = gServerGlobal->getTimerService();
	if (pTimerService)
	{
		pTimerService->KillTimer(ETimerEventID_WarProcess, this);
		pTimerService->KillTimer(ETimerEventID_DeadKillCount, this);
		//pTimerService->KillTimer(ETimerEventID_WarActorLog, this);
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
void CMobaGuideWar::End()
{
    if (isEnd())
    {
        return;
    }

	CWar::End();

	//停止定时器
	ITimerService * pTimerService = gServerGlobal->getTimerService();
	if (pTimerService)
	{
		pTimerService->KillTimer(ETimerEventID_WarProcess, this);
		pTimerService->KillTimer(ETimerEventID_DeadKillCount, this);
		//pTimerService->KillTimer(ETimerEventID_WarActorLog, this);
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

	// 完成新手战场
	FinishGuidWar();
}



/** 容器事件触发后执行函数
@param   wEventID ：事件ID
@param   bSrcType ：发送源类型
@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
@param   pszContext : 上下文
@param   nLen : 上下文长度
@return  
*/
void CMobaGuideWar::OnExecute( WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen )
{
    CMobaWar::OnExecute(wEventID, bSrcType, dwSrcID, pszContext, nLen );
}

void CMobaGuideWar::FinishGuidWar()
{
    // 是否正常结束
	int nMatchType = getMatchTypeID();
	int dwNoviceGuideType = 0;

	WarCampMap::iterator itBegin = m_mapWarCamp.begin();
	for (; itBegin != m_mapWarCamp.end(); ++itBegin)
	{
		SWarCamp* pWarCamp = &itBegin->second;
		WarPersonVector::iterator itPerBegin = pWarCamp->warPersVect.begin();
		for (; itPerBegin != pWarCamp->warPersVect.end(); ++itPerBegin)
		{
			if (itPerBegin->bIsComputer)
				continue;

			ActorHelper actorHelper(itPerBegin->uID);
			IActorService* pActor = actorHelper.m_ptr;
			if (pActor == NULL)
				continue;

			if (nMatchType == MatchType_NewPlayerGuide1)
			{
				dwNoviceGuideType = ENNOVICEGUIDE_TYPE_GUIDE_ONESTEP;
			}
			else if (nMatchType == MatchType_NewPlayerGuide2)
			{
				dwNoviceGuideType = ENNOVICEGUIDE_TYPE_GUIDE_TWOSTEP;
			}
			
			pActor->addTaskDataInt(TASKKEY_ACTOR_NOVICE_GUIDE, dwNoviceGuideType);
		}
	}
}


void CMobaGuideWar::OnTimer(unsigned long dwTimerID)
{
	CMobaWar::OnTimer(dwTimerID);
}

void CMobaGuideWar::Release()
{
	//停止定时器
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_WarProcess, this);
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_DeadKillCount, this);
	//gServerGlobal->getTimerService()->KillTimer(ETimerEventID_WarActorLog, this );
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_EcmExpSituation, this );
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_EcmControlRatio, this );
	gServerGlobal->getTimerService()->KillTimer(ETimerEventID_EcmControlLine, this );
	//CMobaWar::Realease();

    delete this;
}
