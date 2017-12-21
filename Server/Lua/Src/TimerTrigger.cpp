/*******************************************************************
** 文件名:	TimerTrigger.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/25/2016
** 版  本:	1.0
** 描  述:	定时器触发器
********************************************************************/
#include "stdafx.h"
#include "TimerTrigger.h"
#include "TriggerManager.h"
#include "ExternalHelper.h"
#include "ILuaService.h"
#include "EntityHelper.h"

//////////////////////////////////////////////////////////////////////////
bool TimerTrigger::Setup()
{
	if ( m_nFireTimes==0 )
	{
		m_nFireTimes = INFINITY_CALL;
	}

	return g_EHelper.SetTimer(0,m_nInterval,this,m_nFireTimes,m_szCallFunc.c_str());
}

void TimerTrigger::Release()
{
	g_EHelper.KillTimer(0, this);
	delete this;
}

void TimerTrigger::SetID(int nID)
{
	m_nTriggerID = nID;
}

int TimerTrigger::GetID() const
{
	return m_nTriggerID;
}

int TimerTrigger::GetTriggerType(void)
{
	return ETriggerType_Timer;
}

std::string TimerTrigger::GetTriggerCallBack(void)
{
	return m_szCallFunc;
}

int TimerTrigger::GetSceneID()
{
    return m_nSceneID;
}

//////////////////////////////////////////////////////////////////////////
TimerTrigger::TimerTrigger(int nSceneID, const char *szCallFunc, int nInterval, int nFireTimes) : 
    m_nSceneID(nSceneID),m_szCallFunc(szCallFunc),m_nInterval(nInterval),m_nFireTimes(nFireTimes),m_nTriggerID(0)
{
}

TimerTrigger::~TimerTrigger()
{
}

//////////////////////////////////////////////////////////////////////////
void TimerTrigger::OnTimer( unsigned long dwTimerID )
{
    ILuaService *pLuaService = NULL;
    SceneLogicHelper helper(m_nSceneID);
    if (helper.m_ptr)
    {
        pLuaService = helper.m_ptr->getLuaService();
    }
    if (pLuaService == NULL)
    {
        return;
    }

    // 调用CallFunc时,可以会调用DeleteTrigger删除自己,所以要千万小心
    int nTriggerID = m_nTriggerID;
    int nTimesLeft = m_nFireTimes;

	CLuaParam params[1];
	params[0] = m_nTriggerID;
	pLuaService->RunLuaFunction(m_szCallFunc.c_str(), params, _countof(params));

	if (nTimesLeft != INFINITY_CALL && --nTimesLeft <= 0)
	{
		TriggerManager::getInstance().deleteTrigger(nTriggerID);
	}
}
