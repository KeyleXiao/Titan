/*******************************************************************
** 文件名:	TaskHelper.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/24/2016
** 版  本:	1.0
** 描  述:	任务数据帮助函数
********************************************************************/

#include "stdafx.h"
#include "TaskHelper.h"
#include "EntityHelper.h"
#include "TaskStorageDef.h"

/**
@name          : 设置整型任务数据
@brief         : 
@param dwDBID  : 玩家DBID
@param nDataKey: 任务数据key
@param nValue  : 数据值
@return        : 是否设置成功
*/
MY_LUA_API bool setTaskIntData(DWORD dwDBID, int nDataKey, int nValue)
{
    ActorHelper actorHelper(dwDBID, ACTOR_DBID);
    IActorService *pActor = actorHelper.m_ptr;
    if (pActor == NULL)
    {
        return false;
    }

    return pActor->addTaskDataInt(nDataKey, nValue);
}

/**
取得任务数据
@param dwDBID    : 玩家DBID
@param nDataKey  : 任务数据key
@param 
*/
MY_LUA_API int getTaskIntData(DWORD dwDBID, int nDataKey)
{
    ActorHelper actorHelper(dwDBID, ACTOR_DBID);
    IActorService *pActor = actorHelper.m_ptr;
    if (pActor == NULL)
    {
        return TASKDATA_DEFAULT_VALUE;
    }

    return pActor->getTaskDataInt(nDataKey);
}

/**
清除任务数据
@param dwDBID    : 玩家DBID
@param nDataKey  : 任务数据key
@param 
*/
MY_LUA_API bool removeTaskData(DWORD dwDBID, int nDataKey)
{
    ActorHelper actorHelper(dwDBID, ACTOR_DBID);
    IActorService *pActor = actorHelper.m_ptr;
    if (pActor == NULL)
    {
        return false;
    }

    return pActor->removeTaskDataInt(nDataKey);
}

/**
注册任务数据
@param dwDBID    : 玩家DBID
@param nDataKey  : 任务数据key
@param bIsInt    : 任务数据是否是int
@param bSave     : 任务数据是否保存数据库
@param 
*/
MY_LUA_API bool declareTaskData(DWORD dwDBID, int nDataKey, bool bIsInt, bool bSave)
{
    ActorHelper actorHelper(dwDBID, ACTOR_DBID);
    IActorService *pActor = actorHelper.m_ptr;
    if (pActor == NULL)
    {
        return false;
    }

    return pActor->declareTaskData(nDataKey, bIsInt, bSave);
}