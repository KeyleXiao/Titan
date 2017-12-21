/*******************************************************************
** 文件名:	TriggerManager.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/25/2016
** 版  本:	1.0
** 描  述:	触发器管理类
********************************************************************/

#include "stdafx.h"
#include "TriggerManager.h"
#include "TimerTrigger.h"
#include "DateTrigger.h"
#include "LoginTrigger.h"
#include "WarPersonTaskTrigger.h"
#include "NPCTrigger.h"
#include "NPCFuncTrigger.h"
#include "ActorTaskTrigger.h"
#include "Simple_Atom_Lock.h"

TriggerManager::TriggerManager():m_mutex(0)
{
    m_triggerMap.clear();
}

TriggerManager::~TriggerManager()
{

}

TRIGGER_ID TriggerManager::addTrigger(ITrigger *pTrigger)
{
    if (pTrigger == NULL)
    {
        ErrorLn(__FUNCTION__<<"pTrigger竟然为空????");
        debugBreak;
        return -1;
    }

    // 加锁保护
    Simple_Atom_Lock lock(&m_mutex);

    static TRIGGER_ID s_nTriggerID = 0;

    int loop = 0;
    while( m_triggerMap.find(++s_nTriggerID)!=m_triggerMap.end() )
    {
        if ( ++loop > 1000000 )
        {
            ErrorLn("触发器ID竟然全部被占用????");
            debugBreak;
            return -1;
        }
    }

    if ( m_triggerMap.size()>10000 )
    {
        static DWORD dwLastAlertTimes = 0;
        DWORD dwNowTimes = (DWORD)time(NULL);
        if (dwNowTimes>dwLastAlertTimes+60)
        {
            // 输出触发器信息
            outputStatTriggerInfo();

            dwLastAlertTimes = dwNowTimes;
            ErrorLn("触发器数目过多,请检查原因"<<m_triggerMap.size() );
        }
    }

    m_triggerMap[s_nTriggerID] = pTrigger;
    pTrigger->SetID(s_nTriggerID);

    return s_nTriggerID;
}

bool TriggerManager::deleteTrigger(TRIGGER_ID id)
{
    // 加锁保护
    Simple_Atom_Lock lock(&m_mutex);

    TRIGGER_MAP::iterator it = m_triggerMap.find(id);
    if (it == m_triggerMap.end())
    {
        return false;
    }

    ITrigger *pTrigger = it->second;
    if (pTrigger != NULL)
    {
        pTrigger->Release();
    }
    // 表中移除
    m_triggerMap.erase(it);

    return true;
}

ITrigger *TriggerManager::findTrigger(TRIGGER_ID id)
{
    // 加锁保护
    Simple_Atom_Lock lock(&m_mutex);

    TRIGGER_MAP::iterator it = m_triggerMap.find(id);
    if (it == m_triggerMap.end())
    {
        return NULL;
    }

    return it->second;
}

void TriggerManager::outputStatTriggerInfo(void)
{
    static char szTriggerTypeName[ETriggerType_Max][64]= 
    {
        "定时触发器:TimerTrigger",
        "日期触发器:DateTrigger",
        "登陆触发器:LoginTrigger",
        "下线触发器:LogoutTrigger",
    };

    int TriggerCount[ETriggerType_Max] = {0};
    int nOtherCount = 0;

    TRIGGER_MAP::iterator it = m_triggerMap.begin();
    for( ; it != m_triggerMap.end(); ++it)
    {
        int nType = ((*it).second)->GetTriggerType();
        if(nType < 0 || nType >= ETriggerType_Max)
        {
            nOtherCount++;
        }
        else
        {
            TriggerCount[nType]++;
        }
    }

    WarningLn("-------------------- 开始输出触发器统计信息 ----------------------");
    for(int nOut = 0; nOut < ETriggerType_Max; nOut++)
    {
        if(TriggerCount[nOut] <= 0)
        {
            continue;
        }

        WarningLn("个数="<<TriggerCount[nOut]<<" 触发器名："<<szTriggerTypeName[nOut]);
    }

    if(nOtherCount > 0)
    {
        WarningLn("个数="<<nOtherCount<<" 倒，怎么还有未知的触发器，应该是版本问题.");
    }

    WarningLn("*******总触发器个数："<<m_triggerMap.size());

    WarningLn("-------------------- 完成输出触发器统计信息 ----------------------");
}

int TriggerManager::createTimerTrigger(int nSceneID, const char *szCallFunc, int nInterval, int nTimes)
{
	ITrigger *pTrigger = new TimerTrigger(nSceneID, szCallFunc, nInterval, nTimes);
	if ( !pTrigger->Setup() )
	{
		pTrigger->Release();
		return INVALID_TRIGGER_ID;
	}

	return addTrigger(pTrigger);	
}

int TriggerManager::createDateTrigger(int nSceneID, const char *szCallFunc, int nYear, int nMonth, int nDay, int nHour, int nMinute, int nWDay)
{
	ITrigger *pTrigger = new DateTrigger(nSceneID, szCallFunc, nYear, nMonth, nDay, nHour, nMinute, nWDay);
	if ( !pTrigger->Setup() )
	{
		pTrigger->Release();
		return INVALID_TRIGGER_ID;
	}

	return addTrigger(pTrigger);	
}

int TriggerManager::createLoginTrigger(int nSceneID, const char * szCallFunc)
{
    ITrigger * pTrigger = new LoginTrigger(nSceneID, szCallFunc);
    if ( !pTrigger->Setup() )
    {
        pTrigger->Release();
        return INVALID_TRIGGER_ID;
    }

    return addTrigger(pTrigger);
}

/**
@name            : 创建战场人物任务数据触发器 
@brief           : 
@param nSceneID  : 场景ID
@param szCallFunc: 回调函数名
@return          : 返回新创建的触发器ID
*/
int TriggerManager::createWarPersonTaskTrigger(int nSceneID, const char *szCallFunc)
{
    ITrigger * pTrigger = new WarPersonTaskTrigger(nSceneID, szCallFunc);
    if ( !pTrigger->Setup() )
    {
        pTrigger->Release();
        return INVALID_TRIGGER_ID;
    }

    return addTrigger(pTrigger);
}

/**
@name            : 创建角色任务数据触发器 
@brief           : 
@param nSceneID  : 场景ID
@param szCallFunc: 回调函数名
@return          : 返回新创建的触发器ID
*/
int TriggerManager::createActorTaskTrigger(int nSceneID, const char *szCallFunc)
{
	ITrigger * pTrigger = new ActorTaskTrigger(nSceneID, szCallFunc);
	if ( !pTrigger->Setup() )
	{
		pTrigger->Release();
		return INVALID_TRIGGER_ID;
	}

	return addTrigger(pTrigger);
}

/**
@name            : 创建NPC触发器 
@brief           : 
@param nSceneID  : 场景ID
@param szCallFunc: 回调函数名
@return          : 返回新创建的触发器ID
*/
int TriggerManager::createNPCTrigger( int nSceneID, const char *szCallFunc )
{
	ITrigger* pTrigger = new NPCTrigger(nSceneID, szCallFunc);
	if ( !pTrigger->Setup() )
	{
		pTrigger->Release();
		return INVALID_TRIGGER_ID;
	}

	return addTrigger(pTrigger);
}

/**
@name            : 创建NPC功能触发器 
@brief           : 
@param nSceneID  : 场景ID
@param szCallFunc: 回调函数名
@return          : 返回新创建的触发器ID
*/
int TriggerManager::createNPCFuncTrigger(int nSceneID, const char *szCallFunc)
{
	ITrigger* pTrigger = new NPCFuncTrigger(nSceneID, szCallFunc);
	if ( !pTrigger->Setup() )
	{
		pTrigger->Release();
		return INVALID_TRIGGER_ID;
	}

	return addTrigger(pTrigger);
}
