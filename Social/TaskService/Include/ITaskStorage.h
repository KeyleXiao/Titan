/*******************************************************************
** 文件名:	ITaskStorage.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	任务数据操作接口

********************************************************************/

#ifndef __TASKSERVICE_INCLUDE_ITASKSTORAGE_H__
#define __TASKSERVICE_INCLUDE_ITASKSTORAGE_H__

#include "TaskStorageDef.h"

// 任务数据接口
struct ITaskStorage
{
    /// purpose: 释放
    virtual void release(void) = 0;

    virtual void on_stop(void) = 0;

    /// purpose: key的注册
    virtual bool registerKey(DATA_SET_TYPE nType, WORD nKey, bool bSave) = 0;

    /// purpose: 清除某个拥有者的数据
    virtual void clearHolder(DATA_SET_TYPE nType, DWORD dwHolder) = 0;

    /// purpose: 增加数据点
    virtual bool setData(DATA_SET_TYPE nType, DWORD dwHolder, WORD nKey, int nData) = 0;

    /// purpose: 移除数据点
    virtual bool removeData(DATA_SET_TYPE nType, DWORD dwHolder, WORD nKey) = 0;

    /// purpose: 取得值
    virtual int	getValue(DATA_SET_TYPE nType, DWORD dwHolder, WORD nKey) = 0;

    /// purpose: 读取任务数据
    virtual void onReturnReadTaskData(int nQueueIndex, char * pOutData, int nOutLen) = 0;
};


#endif  // __TASKSERVICE_INCLUDE_ITASKSTORAGE_H__
