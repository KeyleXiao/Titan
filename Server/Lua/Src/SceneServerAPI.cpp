/*******************************************************************
** 文件名:	SceneServerAPI.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			场景服API封装
********************************************************************/


#include "stdafx.h"
#include "IServerGlobal.h"
#include "SceneServerAPI.h"
#include "ISceneManager.h"
#include "IGameSceneService.h"
#include "INPCService.h"
#include "ITankService.h"
#include "TaskMessageDef.h"
#include "entityHelper.h"
#include "ExternalHelper.h"
#include "OssLogDef.h"
#include "ChatHelper.h"
#include "ChatDef.h"
#include "IOSSLogService.h"
#include "MentorshipHelper.h"
#include "IMentorshipSceneService.h"
#include "TaskStorageDef.h"
#include "MailHelper.h"

// 创建NPC
// @param sceneID : 场景ID
// @param npc_id  : NPCID
// @param 
MY_LUA_API UID createNPC( int sceneID,int npc_id,float loc_x,float loc_y,float loc_z )
{
    SceneServiceHelper helper(sceneID);
	IGameSceneService * pScene = helper.m_ptr;
	if ( pScene==0 )
	{
		ErrorLn("createNPC invoke fail, pScene==0");
		return INVALID_UID;
	}
    UINT_PTR serviceID = pScene->getLogicServiceID();
	if (serviceID == INVALID_SERVICE_ID)
	{
		ErrorLn("createNPC invoke fail, serviceID==INVALID_SERVICE_ID");
		return INVALID_UID;
	}
	SERVICE_PTR pContainer = gServerGlobal->getFramework()->get_service_manager()->get_service( SERVICE_ID(serviceID) );
	if (pContainer == 0)
	{
		ErrorLn("createNPC invoke fail, pContainer==0");
		return INVALID_UID;
	}
	INPCService * pNPC = ((ISceneLogicService*)pContainer->query_interface())->getNPCService();
	if ( pNPC==0 )
	{
		ErrorLn("createNPC invoke fail, pNPC==0");
		return INVALID_UID;
	}
	CreateNpcContext context;
	return pNPC->createNPC(npc_id,Vector3(loc_x,loc_y,loc_z),context);
}

// 销毁NPC
// @param sceneID : 场景ID
// @param uid  : NPCUID
// @param 
MY_LUA_API UID destroyNPC(int sceneID, int uid)
{
    SceneServiceHelper helper(sceneID);
    IGameSceneService * pScene = helper.m_ptr;
    if (pScene == 0)
    {
        ErrorLn("destroyNPC invoke fail, pScene==0");
        return INVALID_UID;
    }
    UINT_PTR serviceID = pScene->getLogicServiceID();
    if (serviceID == INVALID_SERVICE_ID)
    {
        ErrorLn("destroyNPC invoke fail, serviceID==INVALID_SERVICE_ID");
        return INVALID_UID;
    }
    SERVICE_PTR pContainer = gServerGlobal->getFramework()->get_service_manager()->get_service(SERVICE_ID(serviceID));
    if (pContainer == 0)
    {
        ErrorLn("destroyNPC invoke fail, pContainer==0");
        return INVALID_UID;
    }
    INPCService * pNPC = ((ISceneLogicService*)pContainer->query_interface())->getNPCService();
    if (pNPC == 0)
    {
        ErrorLn("destroyNPC invoke fail, pNPC==0");
        return INVALID_UID;
    }
    CreateNpcContext context;
    return pNPC->destroyNPC(uid);
}


// 创建Tank
// @param sceneID : 场景ID
// @param npc_id  : NPCID
// @param 
MY_LUA_API UID createTank( int sceneID,int Tank_id,float loc_x,float loc_y,float loc_z )
{
    SceneServiceHelper helper(sceneID);
	IGameSceneService * pScene = helper.m_ptr;
    if (pScene == 0)
    {
		return INVALID_UID;
    }
    UINT_PTR serviceID = pScene->getLogicServiceID();
	if (serviceID == INVALID_SERVICE_ID)
	{
		ErrorLn("createTank invoke fail, serviceID==INVALID_SERVICE_ID");
		return INVALID_UID;
	}
	SERVICE_PTR pContainer = gServerGlobal->getFramework()->get_service_manager()->get_service( SERVICE_ID(serviceID) );
	if (pContainer == 0)
	{
		ErrorLn("createTank invoke fail, pContainer==0");
		return INVALID_UID;
	}
	ITankService * pTank = ((ISceneLogicService*)pContainer->query_interface())->getTankService();
	if ( pTank==0 )
	{
		ErrorLn("createTank invoke fail, pTank==0");
		return INVALID_UID;
	}

	return pTank->createTank(Tank_id,Vector3(loc_x,loc_y,loc_z),0,0);
}

// 发送任务列表
// @param dwPDBID : 玩家DBID
// @param strTask : 任务字符串
// @param 
MY_LUA_API void sendTaskList(DWORD dwPDBID, const char *strTask)
{
    if (strTask == NULL)
    {
        return;
    }

    g_EHelper.Send2Client(dwPDBID, MSG_MODULEID_TASK, TASK_MSG_SHOW_TASK_LIST, (void *)strTask, strlen(strTask));
}

// 更新任务列表
// @param dwPDBID : 玩家DBID
// @param strTask : 任务字符串
// @param 
MY_LUA_API void updateTaskList(DWORD dwPDBID, const char *strTask)
{
    if (strTask == NULL)
    {
        return;
    }

    g_EHelper.Send2Client(dwPDBID, MSG_MODULEID_TASK, TASK_MSG_UPDATE_TASK_LIST, (void *)strTask, strlen(strTask));
}

// 删除任务列表
// @param dwPDBID : 玩家DBID
// @param strTask : 任务字符串
// @param 
MY_LUA_API void deleteTaskList(DWORD dwPDBID, const char *strTask)
{
    if (strTask == NULL)
    {
        return;
    }

    g_EHelper.Send2Client(dwPDBID, MSG_MODULEID_TASK, TASK_MSG_DELETE_TASK_LIST, (void *)strTask, strlen(strTask));
}

// 发送任务奖励
// @param dwPDBID : 玩家DBID
// @param strTask : 任务奖励字符串
// @param 
MY_LUA_API void sendTaskPrize(DWORD dwPDBID, const char *strTask)
{
    if (strTask == NULL)
    {
        return;
    }

    g_EHelper.Send2Client(dwPDBID, MSG_MODULEID_TASK, TASK_MSG_SEND_TASK_PRIZE, (void *)strTask, strlen(strTask));
}

/**
@name    增加/减少经验值(战场经验)
@param   : dwPDBID:         玩家DBID
@param   : nExp:            经验值（负值减少，正值增加）
@param   : szLogMark:       传入交易日志，供查询
*/
MY_LUA_API void addExp(DWORD dwDBID, int nExp, const char *pszRemark)
{
    ActorHelper actorHelper(dwDBID, ACTOR_DBID);
    IActorService *pActor = actorHelper.m_ptr;
    if (pActor == NULL)
    {
        return;
    }

    WORD wApproach = OSSRES_ADD_TASK;
    if (nExp <= 0)
    {
        wApproach = OSSRES_DEL_TASK;
    }

    pActor->addExp(nExp, wApproach, 0, pszRemark);
}

/**
@name    设置玩家经验(主城经验)
@param   : dwPDBID:         玩家DBID
@param   : lgValue:         要设置数值
@param   : szLogMark:       传入交易日志，供查询
*/
MY_LUA_API void setActorBaseExp(DWORD dwDBID, LONG lgValue, LPCSTR szLogMark)
{
    ActorHelper actorHelper(dwDBID, ACTOR_DBID);
    IActorService *pActor = actorHelper.m_ptr;
    if (pActor == NULL)
    {
        return;
    }

    // 资源操作类型OSS_RESOURCE_OPT_TYPE
    BYTE byOpType = OSS_RESOURCE_OPT_ADD;
    // 消耗原因（产生OSS_RESOURCE_ADD_RANGE/消耗的途径OSS_RESOURCE_OPT_DEL/转进OSS_RESOURCE_OPT_TRANSIN/转出OSS_RESOURCE_OPT_TRANSOUT）
    WORD wApproach = OSSRES_ADD_TASK;
    if (lgValue <= 0)
    {
        byOpType = OSS_RESOURCE_OPT_DEL;
        wApproach = OSSRES_DEL_TASK;
    }

    pActor->setActorBaseInfo(lgValue, ENPLAYERINFOTYPE_EXP, ENCHANGEDATAMODE_ADD, OSS_NUMERIC_EXP, byOpType, wApproach, 0, szLogMark);
}


/** 是否可增加或扣除货币
@param  : dwPDBID:        玩家DBID
@param  nTicket: 添加的点券，负数表示扣除点卷
@param  nGlod  : 添加的金币，负数表示扣除金币
@return : 是否可以添加点券
*/
MY_LUA_API bool canAddMoney(DWORD dwDBID, int nTicket, int nGlod)
{
    ActorHelper actorHelper(dwDBID, ACTOR_DBID);
    IActorService *pActor = actorHelper.m_ptr;
    if (pActor == NULL)
    {
        return false;
    }

    return pActor->canAddMoney(nTicket, nGlod);
}

/** 增加或扣除货币
@param  : dwPDBID:       玩家DBID
@param  nTicket: 添加的点券，负数表示扣除点卷
@param  nGlod  : 添加的金币，负数表示扣除金币
@param  : pszRemark:     添加的点券日志的备注（记日志用）
@note   : 在添加点券操作提交数据库（包括其他模块提交的）还没有返回前，这时候你提交点券是会失败的
*/
MY_LUA_API void addMoney(DWORD dwDBID, int nTicket, int nGlod, const char *pszRemark)
{
    ActorHelper actorHelper(dwDBID, ACTOR_DBID);
    IActorService *pActor = actorHelper.m_ptr;
    if (pActor == NULL)
    {
        return;
    }

    WORD wApproach = OSSRES_ADD_TASK;
    if (nTicket < 0 || nGlod < 0)
    {
        wApproach = OSSRES_DEL_TASK;
    }

    pActor->addMoney(nTicket, nGlod, wApproach, pszRemark);
}

/** 是否可以给角色奖励
@param  : dwPDBID:       玩家DBID
@param  : nPrizeID:      奖励物品ID
@param  : nNum:          奖励物品数量
@note   : 是否可以给角色奖励
*/
MY_LUA_API bool canPrizeActor(DWORD dwDBID, int nPrizeID, int nNum)
{
    ActorHelper actorHelper(dwDBID, ACTOR_DBID);
    IActorService *pActor = actorHelper.m_ptr;
    if (pActor == NULL)
    {
        return false;
    }

    return pActor->canPrizeActor(nPrizeID, nNum);
}

/** 给角色奖励
@param  : dwPDBID:       玩家DBID
@param  : nPrizeID:      奖励物品ID
@param  : nNum:          奖励物品数量
@param  : dwTargetID:    如果有目标者，这个就填入目标者ID
@param  : szLogMark:     传入交易日志，供查询 
@note   : 给角色奖励
*/
MY_LUA_API bool prizeActor(DWORD dwDBID, int nPrizeID, int nNum, DWORD dwTargetID, LPCSTR szLogDesc)
{
    ActorHelper actorHelper(dwDBID, ACTOR_DBID);
    IActorService *pActor = actorHelper.m_ptr;
    if (pActor == NULL)
    {
        return false;
    }

    return pActor->prizeActor(nPrizeID, nNum, OSSRES_ADD_TASK, dwTargetID, szLogDesc);
}

/** 显示提示文字
@param  : dwPDBID:       玩家DBID
@param  : nPrizeID:      奖励物品ID
@param  : szParam:       描述文字
*/
MY_LUA_API void showSystemMessage(DWORD dwDBID, int nTipID, LPCSTR szParam)
{
    ChatHelper chatHelper;
    IChatSceneService *pChatSceneService = chatHelper.m_ptr;
    if (pChatSceneService == NULL)
    {
        return;
    }

    pChatSceneService->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, dwDBID, (EMChatTipID)nTipID, szParam);
}

/** 增加任务文字日志
@param  : dwPDBID:       玩家DBID
@param  : szParam:       日志文字
*/
MY_LUA_API void addTaskGameNoteLog(DWORD dwDBID, LPCSTR szParam)
{
    if (szParam == NULL)
    {
        return;
    }

    IOSSLogService * pOSSLogServer = gServerGlobal->getOSSLogService();
    if ( pOSSLogServer == NULL )
    {
        return;
    }

    pOSSLogServer->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TASK, dwDBID, szParam);
}

/** 获取玩家等级
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerLevel(DWORD dwDBID)
{
	ActorHelper actorHelper(dwDBID, ACTOR_DBID);
	IActorService *pActor = actorHelper.m_ptr;
	if (pActor == NULL)
	{
		return 0;
	}

	return pActor->getProperty_Integer(PROPERTY_LEVEL);
}

/** 获取玩家联盟
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerClanID(DWORD dwDBID)
{
	ActorHelper actorHelper(dwDBID, ACTOR_DBID);
	IActorService *pActor = actorHelper.m_ptr;
	if (pActor == NULL)
	{
		return 0;
	}

	return pActor->getProperty_Integer(PROPERTY_CLAN);
}

/** 获取玩家比赛场数
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerMatchCount(DWORD dwDBID)
{
	ActorHelper actorHelper(dwDBID, ACTOR_DBID);
	IActorService *pActor = actorHelper.m_ptr;
	if (pActor == NULL)
	{
		return 0;
	}

	SPlayerInfo playerInfo = pActor->getPlayerInfo();
	if ( playerInfo.dwActorID == 0 )
	{
		return 0;
	}

	int nCount = 0;
	memcpy( &nCount, (playerInfo.byGlory + ENACHIEVE_MATCHNUM*12), sizeof(nCount));
	
	return nCount;
}

/** 获取玩家英雄个数
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerHeroCount(DWORD dwDBID)
{
	ActorHelper actorHelper(dwDBID, ACTOR_DBID);
	IActorService *pActor = actorHelper.m_ptr;
	if (pActor == NULL)
	{
		return 0;
	}

	return pActor->getPermanentHeroCount();
}

/** 获取玩家英雄皮肤个数
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerSkinCount(DWORD dwDBID)
{
	ActorHelper actorHelper(dwDBID, ACTOR_DBID);
	IActorService *pActor = actorHelper.m_ptr;
	if (pActor == NULL)
	{
		return 0;
	}

	return pActor->getTotalSkinCount();
}

/** 获取玩家师傅个数
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerMasterCount(DWORD dwDBID)
{
	MentorshipHelper mentorshipHelper;
	IMentorshipSceneService *pMentorshipService = mentorshipHelper.m_ptr;

	if (pMentorshipService == NULL)
	{
		return 0;
	}

	return pMentorshipService->getPlayerMasterCount(dwDBID);
}

/** 获取玩家学徒个数
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerPrenticeCount(DWORD dwDBID)
{
	MentorshipHelper mentorshipHelper;
	IMentorshipSceneService *pMentorshipService = mentorshipHelper.m_ptr;

	if (pMentorshipService == NULL)
	{
		return 0;
	}

	return pMentorshipService->getPlayerPrenticeCount(dwDBID);
}

/** 检测目标是否为玩家徒弟
@param  : dwPDBID:       玩家DBID
@param  : dwTargetDBID	 目标DBID
*/
MY_LUA_API int checkIsPlayerPrentice(DWORD dwDBID, DWORD dwTargetDBID)
{
	MentorshipHelper mentorshipHelper;
	IMentorshipSceneService *pMentorshipService = mentorshipHelper.m_ptr;

	if (pMentorshipService == NULL)
	{
		return 0;
	}

	return pMentorshipService->checkIsPlayerPrentice(dwDBID, dwTargetDBID);
}

/** 获取玩家英雄某个等级个数
@param  : dwPDBID:       玩家DBID
@param  : nLevel:        玩家英雄等级
*/
MY_LUA_API int getPlayerHeroCountByLevel(DWORD dwDBID, int nLevel)
{
	ActorHelper actorHelper(dwDBID, ACTOR_DBID);
	IActorService *pActor = actorHelper.m_ptr;
	if (pActor == NULL)
	{
		return 0;
	}

	return pActor->getHeroCountByLevel(nLevel);
}

/** 获取玩家击杀大龙个数
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerKillBigDragoCount(DWORD dwDBID)
{
	ActorHelper actorHelper(dwDBID, ACTOR_DBID);
	IActorService *pActor = actorHelper.m_ptr;
	if (pActor == NULL)
	{
		return 0;
	}

	SPlayerInfo playerInfo = pActor->getPlayerInfo();
	if ( playerInfo.dwActorID == 0 )
	{
		return 0;
	}

	int nCount = 0;
	memcpy( &nCount, (playerInfo.byGlory+ ENACHIEVE_KILLBIGDRAGONNUM*12), sizeof(nCount));

	return nCount;
}

/** 获取玩家击杀小龙个数
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerKillSmallDragoCount(DWORD dwDBID)
{
	ActorHelper actorHelper(dwDBID, ACTOR_DBID);
	IActorService *pActor = actorHelper.m_ptr;
	if (pActor == NULL)
	{
		return 0;
	}

	SPlayerInfo playerInfo = pActor->getPlayerInfo();
	if ( playerInfo.dwActorID == 0 )
	{
		return 0;
	}

	int nCount = 0;
	memcpy( &nCount, (playerInfo.byGlory + ENACHIEVE_KILLSMALLDRAGONNUM*12), sizeof(nCount));

	return nCount;
}

/** 获取玩家战场使用翅膀记录
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerWarUseWringCount(DWORD dwDBID)
{
	ActorHelper actorHelper(dwDBID, ACTOR_DBID);
	IActorService *pActor = actorHelper.m_ptr;
	if (pActor == NULL)
	{
		return 0;
	}

	int nUseCount = pActor->getTaskDataInt(TASKKEY_ACTOR_WAR_USEWRING);
	if (nUseCount == TASKDATA_DEFAULT_VALUE)
		return 0;

	return nUseCount;
}

/** 获取玩家战场使用XP
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerWarUseXPCount(DWORD dwDBID)
{
	ActorHelper actorHelper(dwDBID, ACTOR_DBID);
	IActorService *pActor = actorHelper.m_ptr;
	if (pActor == NULL)
	{
		return 0;
	}

	int nUseCount = pActor->getTaskDataInt(TASKKEY_ACTOR_WAR_USEXP);
	if (nUseCount == TASKDATA_DEFAULT_VALUE)
		return 0;

	return nUseCount;
}

/** 获取玩家当前的新手引导步骤
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerGuideStep(DWORD dwDBID)
{
    ActorHelper actorHelper(dwDBID, ACTOR_DBID);
    IActorService *pActor = actorHelper.m_ptr;
    if (pActor == NULL)
    {
        return 0;
    }

    int nStep = pActor->getTaskDataInt(TASKKEY_ACTOR_NOVICE_GUIDE);
    return (nStep == TASKDATA_DEFAULT_VALUE) ? 0 : nStep;
}

/** 监控任务接收，完成等情况
@ param nTaskID	   `: 任务id
@ param nAcceptNum	: 接受次数
@ param nFinishNum	: 完成次数
@ param nGiveUpNum  : 放弃次数
*/
MY_LUA_API void OssMonitorTask(int nTaskID, int nAcceptNum, int nFinishNum, int nGiveUpNum)
{
	IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
	if (pOSSLogService == NULL)
		return;

	pOSSLogService->addTaskInfo(nTaskID, nAcceptNum, nFinishNum, nGiveUpNum);

}

/** 添加交互任务
@ param dwDBID		: 玩家DBID
@ param nTaskID		: 任务ID
*/
MY_LUA_API void onCheckAddContactTask(DWORD dwDBID, int bySubType, int bySpecificType, int nTagIDHigh32, int nTagIDLow32, LPCSTR szEmotionWords, int nResult)
{
	if (nResult > 0)
	{
		LONGLONG llTagID = MAKELONGLONG(nTagIDLow32, nTagIDHigh32);

		SContactTaskAdd_SO data;
		data.dwPDBID		= dwDBID;
		data.bySubType		= (BYTE)bySubType;
		data.bySpecificType	= (BYTE)bySpecificType;
		data.llTagID		= llTagID;
		sstrcpyn(data.szEmotionWords, szEmotionWords, sizeof(data.szEmotionWords));
		g_EHelper.Send2Social(MSG_MODULEID_TASKSTORAGE, TASK_MSG_ADD_CONTACT_TASK_SO, &data, sizeof(data));
	}
	else
	{
		g_EHelper.showSystemMessage(dwDBID, CHAT_TIP_LUA_TASK_PUBLISH_COUNT_MAX);
	}
}

/** 发送添加成功
@ param dwDBID			: 玩家DBID
@ param strTask			: 任务字符串
*/
MY_LUA_API void sendAddTaskSuccess(DWORD dwPDBID, const char *strTask)
{
	if (strTask == NULL)
	{
		return;
	}

	g_EHelper.Send2Client(dwPDBID, MSG_MODULEID_TASK, TASK_MSG_ADD_CONTACT_TASK_SUCCESS_SC, (void *)strTask, strlen(strTask));

	g_EHelper.showSystemMessage(dwPDBID, CHAT_TIP_LUA_TASK_PUBLISH_SUCCESSS);
}

/** 发送预获取任务结果
@ param dwDBID			: 玩家DBID
@ param strTask			: 任务字符串
*/
MY_LUA_API void sendPreCatchTaskResult(DWORD dwPDBID, const char *strTask)
{
	if (strTask == NULL)
	{
		return;
	}

	g_EHelper.Send2Client(dwPDBID, MSG_MODULEID_TASK, TASK_MSG_PRE_CATCH_TASK_SC, (void *)strTask, strlen(strTask));
}

/** 获取交互任务
@ param dwDBID			: 玩家DBID
@ param strTask			: 任务字符串
*/
MY_LUA_API void sendCatchTaskResult(DWORD dwDBID, const char *strTask)
{
	if (strTask == NULL)
	{
		return;
	}

	g_EHelper.Send2Client(dwDBID, MSG_MODULEID_TASK, TASK_MSG_CATCH_TASK_SC, (void *)strTask, strlen(strTask));
}

/** 刷新操作信息
@ param dwDBID			: 玩家DBID
@ param strTask			: 任务字符串
*/
MY_LUA_API void sendReflushOptInfo(DWORD dwDBID, const char *strTask)
{
	if (strTask == NULL)
	{
		return;
	}

	g_EHelper.Send2Client(dwDBID, MSG_MODULEID_TASK, TASK_MSG_REFLUSH_OPT_SC, (void *)strTask, strlen(strTask));
}

/** 检测刷新任务操作扣钱是否成功
@ param dwDBID			: 玩家DBID
*/
MY_LUA_API bool checkAddMoneyOfReflushOpt(DWORD dwDBID)
{
	ActorHelper actorHelper(dwDBID, ACTOR_DBID);
	IActorService *pActor = actorHelper.m_ptr;
	if (pActor == NULL)
		return false;

	ISchemeCenter *pSchemeCenter = gServerGlobal->getSchemeCenter();
	if (pSchemeCenter == NULL)
		return false;
	ISchemePointShop *pSchemeShop = pSchemeCenter->getSchemePointShop();
	if (pSchemeShop == NULL)
		return false;
	vector<SPointShopGoodsConfig_S*> listSellConfig;
	pSchemeShop->getPointShopSellConfigList(POINTSHOP_GOODSTYPE_SPECIAL, listSellConfig);
	if (listSellConfig.empty())
		return false;
	SPointShopGoodsConfig_S* pConfig = NULL;
	for (int i = 0; i < listSellConfig.size(); ++i)
	{
		if (listSellConfig[i]->nConsumpApproach == (int)CONSUMP_APPROACH_TASKOPT_REFLUSH)
		{
			pConfig = listSellConfig[i];
			break;
		}
	}
	if (pConfig == NULL)
		return false;
	
	msg_entity_pointshop_buygoods msgInfo;
	char szMoneyType[32] = {0};
	if (pConfig->nPointPrice > 0)
	{
		msgInfo.nResType = (int)ENPOINTSHOPRESTYPE_POINT;
		msgInfo.nClientPrice = pConfig->nPointPrice;
		sprintf(szMoneyType, "%s", a2utf8("点券"));
	}
	else	
	{
		msgInfo.nResType = (int)ENPOINTSHOPRESTYPE_MONEY;
		msgInfo.nClientPrice = pConfig->nMoneyPrice;
		sprintf(szMoneyType, "%s", a2utf8("金币"));
	}
	msgInfo.nSellID = pConfig->nSellID;
	msgInfo.nNum = 1;
	if (msgInfo.nClientPrice <= 0)
		return false;
	if (!pActor->onMessagePointShopBuyGoods((void*)&msgInfo, sizeof(msgInfo)))
		return false;

	// 记录文字日志
	IOSSLogService *pOSSLogService = gServerGlobal->getOSSLogService();
	if (NULL != pOSSLogService)
	{
		char szText[256] = {0};
		sprintf_s(szText, sizeof(szText), a2utf8("%s刷新任务，消耗%s%d"), pActor->getName(), szMoneyType, msgInfo.nClientPrice);
		pOSSLogService->addGameNoteLog(pActor->getFromGameWorldID(), OSS_NOTE_TRACK_PERSON, pActor->getProperty_Integer(PROPERTY_ID), szText);
	}
	
	return true;
}

/** 获取玩家战场5杀战绩
@param  : dwPDBID:       玩家DBID
*/
MY_LUA_API int getPlayerWar5Kill(DWORD dwDBID)
{
	ActorHelper actorHelper(dwDBID, ACTOR_DBID);
	IActorService *pActor = actorHelper.m_ptr;
	if (pActor == NULL)
	{
		return 0;
	}

	SPlayerInfo playerInfo = pActor->getPlayerInfo();
	if ( playerInfo.dwActorID == 0 )
	{
		return 0;
	}

	int nCount = 0;
	memcpy( &nCount, (playerInfo.byGlory+ ENACHIEVE_KILLBIGDRAGONNUM*12), sizeof(nCount));

	return nCount;
}

/** 给玩家发送奖励邮件
@param  : dwPDBID		玩家DBID
@param  : nGold			金币
@param  : nExp			经验
@param  : nPrizeID1		奖励ID1
@param  : nPrizeID2		奖励ID2
*/
MY_LUA_API void sendPlayerMail(DWORD dwDBID, int nGold, int nExp, int nPrizeID1, int nPrizeID2)
{
	MailHelper mailHelper;
	IMailSceneService *pMailSceneService = mailHelper.m_ptr;
	if (pMailSceneService == NULL)
		return;

	SMailSendData sendData;
	sendData.nType = emMailType_System;
	sendData.nSourceType = emMailSourceType_System;
	sendData.dwSenderPDBID = 0;
	sendData.dwMasterPDBID = dwDBID;

	sendData.nPlusMoney = nGold;
	sendData.nPlusExp = nExp;
	sendData.nPlusGoodsNum = 0;
	if (nPrizeID1 > 0)
	{
		sendData.sGoodsList[1].dwGoodsID = nPrizeID1;
		sendData.sGoodsList[1].nCount = 1;
		++sendData.nPlusGoodsNum;
	}
	if (nPrizeID2 > 0)
	{
		sendData.sGoodsList[1].dwGoodsID = nPrizeID2;
		sendData.sGoodsList[1].nCount = 1;
		++sendData.nPlusGoodsNum;
	}

	pMailSceneService->sendMail(sendData, emMailFill_Rank_TaskPrize);
}