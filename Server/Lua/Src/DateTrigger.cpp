/*******************************************************************
** 文件名:	DateTrigger.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/25/2016
** 版  本:	1.0
** 描  述:	定时器触发器
********************************************************************/
#include "stdafx.h"
#include "DateTrigger.h"
#include "TriggerManager.h"
#include "ExternalHelper.h"
#include "ILuaService.h"
#include "EntityHelper.h"

//////////////////////////////////////////////////////////////////////////
bool DateTrigger::Setup()
{
	return g_EHelper.SetTimer(0, 30*1000, this, INFINITY_CALL, m_szCallFunc.c_str());
}

void DateTrigger::Release()
{
	g_EHelper.KillTimer(0, this);
	delete this;
}

void DateTrigger::SetID(int nID)
{
	m_nTriggerID = nID;
}

int DateTrigger::GetID() const
{
	return m_nTriggerID;
}

int DateTrigger::GetTriggerType(void)
{
	return ETriggerType_Date;
}

std::string DateTrigger::GetTriggerCallBack(void)
{
	return m_szCallFunc;
}

int DateTrigger::GetSceneID()
{
    return m_nSceneID;
}

//////////////////////////////////////////////////////////////////////////
DateTrigger::DateTrigger(int nSceneID, const char *szCallFunc, int nYear, int nMonth, int nDay, int nHour, int nMinute, int nWDay) : 
    m_nSceneID(nSceneID),    
    m_szCallFunc(szCallFunc),
    m_nYear(nYear),
    m_nMonth(nMonth),
    m_nDay(nDay),
    m_nHour(nHour),
    m_nMinute(nMinute),
    m_nWDay(nWDay),
    m_nTriggerID(0),
    m_nCheckTimer(0),
    m_dwLastCheck(0)
{
}

DateTrigger::~DateTrigger()
{
}

//////////////////////////////////////////////////////////////////////////
void DateTrigger::OnTimer( unsigned long dwTimerID )
{
    time_t t = time(NULL); 

    // 精度1分钟，小于半小时不要连续触发
    if ( m_dwLastCheck!=0 && getTickCountEx()-m_dwLastCheck<60*1000*30)
        return;

    // 获取上次检测时间
    time_t tLaskCheckTimer = m_nCheckTimer; 

    // 更新下上次检测时间
    m_nCheckTimer = t;

    // 可能改了时间，或者时间发生了跳跃
    if ( (t-tLaskCheckTimer) > 60 && (t-tLaskCheckTimer) < 600 )
    {
        for ( time_t tick = tLaskCheckTimer; tick<t; tick+=10 )
        {
            if ( CheckTime(tick)==true )
            {
                WarningLn("可能改了时间，或者时间发生了跳跃");
                break;
            }
        }
    }
    else
    {
        CheckTime(t);
    }
}

bool DateTrigger::CheckTime(const time_t &t)
{
    tm *tt = localtime(&t);
    if ( tt==0 )
        return false;

    if ( m_nYear>=0 && tt->tm_year!=m_nYear )
        return false;

    if ( m_nMonth>=0 && tt->tm_mon!=m_nMonth )
        return false;

    if ( m_nDay>=0 && tt->tm_mday!=m_nDay )
        return false;

    if ( m_nWDay>=0 && tt->tm_wday!=m_nWDay )
        return false;

    if ( m_nHour>=0 && tt->tm_hour!=m_nHour )
        return false;

    if ( m_nMinute>=0 && tt->tm_min != m_nMinute)
        return false;

    ILuaService *pLuaService = NULL;
    SceneLogicHelper helper(m_nSceneID);
    if (helper.m_ptr)
    {
        pLuaService = helper.m_ptr->getLuaService();
    }
    if (pLuaService == NULL)
    {
        return false;
    }

    CLuaParam params[1];
    params[0] = m_nTriggerID;
    pLuaService->RunLuaFunction(m_szCallFunc.c_str(),params, _countof(params));

    m_dwLastCheck = getTickCountEx();

    return true;
}