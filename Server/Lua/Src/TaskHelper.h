/*******************************************************************
** 文件名:	TaskHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/24/2016
** 版  本:	1.0
** 描  述:	任务数据帮助函数
********************************************************************/

#include "stdafx.h"
#include "SceneServerAPI.h"

/**
@name          : 设置整型任务数据
@brief         : 
@param dwDBID  : 玩家DBID
@param nDataKey: 任务数据key
@param nValue  : 数据值
@return        : 是否设置成功
*/
MY_LUA_API bool setTaskIntData(DWORD dwDBID, int nDataKey, int nValue);

/**
取得任务数据
@param dwDBID    : 玩家DBID
@param nDataKey  : 任务数据key
@param 
*/
MY_LUA_API int getTaskIntData(DWORD dwDBID, int nDataKey);

/**
清除任务数据
@param dwDBID    : 玩家DBID
@param nDataKey  : 任务数据key
@param 
*/
MY_LUA_API bool removeTaskData(DWORD dwDBID, int nDataKey);

/**
注册任务数据
@param dwDBID    : 玩家DBID
@param nDataKey  : 任务数据key
@param bIsInt    : 任务数据是否是int
@param bSave     : 任务数据是否保存数据库
@param 
*/
MY_LUA_API bool declareTaskData(DWORD dwDBID, int nDataKey, bool bIsInt, bool bSave);