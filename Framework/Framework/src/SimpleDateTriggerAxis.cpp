/*******************************************************************
** 文件名:	SimpleDateTriggerAxis.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	7/12/2016
** 版  本:	1.0
** 描  述:	触发器时间轴

			
********************************************************************/

#include "stdafx.h"
#include "SimpleDateTriggerAxis.h"
#include "Framework.h"
#include "DateHandler.h"

bool SimpleDateTriggerAxis::Create()
{
    return true;
}

bool SimpleDateTriggerAxis::Close()
{
    m_DateHanderList.clear();
    return true;
}

bool SimpleDateTriggerAxis::RegisterDateTrigger(int nTriggerID, IDateHandler* pDateHander, SDateTriggerInfo sTriggerInfo, const char * debugInfo/*=0 */)
{
    int year = sTriggerInfo.year;
    int month = sTriggerInfo.month;
    int day = sTriggerInfo.day;
    int hour = sTriggerInfo.hour;
    int min = sTriggerInfo.min;
    int wday = sTriggerInfo.wday;
    for (list<SDateHander>::iterator it = m_DateHanderList.begin(); it != m_DateHanderList.end(); ++ it)
    {
        // 触发器已经存在
        if (it->nTriggerID == nTriggerID && it->pDateHander == pDateHander)
        {
            
            SDateHander* pDateHanderEx = &(*it);
            pDateHanderEx->year = (year == -1) ? year : (year - 1900);
            pDateHanderEx->month = (month == -1) ? month : (month - 1);
            pDateHanderEx->day = day;
            pDateHanderEx->hour = hour;
            pDateHanderEx->min = min;
            pDateHanderEx->wday = wday;
            pDateHanderEx->m_dwLastCheck = 0;		

            return true;
        }
    }

    SDateHander dateHander;
    dateHander.year = (year == -1) ? year : (year - 1900);
    dateHander.month = (month == -1) ? month : (month - 1);
    dateHander.day = day;
    dateHander.hour = hour;
    dateHander.min = min;
    dateHander.wday = wday;
    dateHander.nTriggerID = nTriggerID;
    dateHander.pDateHander = pDateHander;
    dateHander.m_dwLastCheck = 0;

    m_DateHanderList.push_back(dateHander);
    return true;
}

bool SimpleDateTriggerAxis::UnRegisterDateTrigger(int nTriggerID, IDateHandler* pDateHander)
{
    list<SDateHander>::iterator it;
    for (it = m_DateHanderList.begin(); it != m_DateHanderList.end(); ++ it)
    {
        if (it->nTriggerID == nTriggerID && it->pDateHander == pDateHander)
        {
            m_DateHanderList.erase(it);
            return true;
        }
    }
    
    return true;
}

void SimpleDateTriggerAxis::CheckTimer(ulong timeout /*= 0*/)
{
    // 查找所有的触发器，看看有没有符合条件的
    time_t nCurTime = time(NULL);
    struct tm* tmCurTime1 = localtime(&nCurTime);
    if (tmCurTime1 == NULL)
    {
        return;
    }
    tm tmCurTime = *tmCurTime1;

    list<SDateHander>::iterator it;
    for (it = m_DateHanderList.begin(); it != m_DateHanderList.end(); ++ it)
    {
        SDateHander* pHandle = &(*it);
        // 精度1分钟，粒度调整60秒不要连续触发
        if (pHandle->m_dwLastCheck != 0 && GetTickCount() - pHandle->m_dwLastCheck < 1000 * 60)
        {
            continue;
        }

        if (pHandle->year >= 0 && tmCurTime.tm_year != pHandle->year)
        {
            continue;
        }

        if (pHandle->month >= 0 && tmCurTime.tm_mon != pHandle->month)
        {
            continue;
        }

        if (pHandle->day >= 0 && tmCurTime.tm_mday != pHandle->day)
        {
            continue;
        }

        if (pHandle->hour >= 0 && tmCurTime.tm_hour != pHandle->hour)
        {
            continue;
        }

        if (pHandle->wday >= 0 && tmCurTime.tm_wday != pHandle->wday)
        {
            continue;
        }

        if (pHandle->min  >= 0 && tmCurTime.tm_min != pHandle->min)
        {
            continue;
        }

        pHandle->m_dwLastCheck = GetTickCount();

        m_NotHandleList.push_back(pHandle);
        
    }

    // 考虑到粒度的问题有可能同时注册量过多 导致线程一个周期执行不完的情况把未执行的保存起来
    for (list<SDateHander*>::iterator itNotHandle = m_NotHandleList.begin(); itNotHandle != m_NotHandleList.end(); )
    {
        SDateHander* pNotHandle = *itNotHandle;

        if (pNotHandle->pDateHander)
        {
            pNotHandle->pDateHander->DateCome(pNotHandle->nTriggerID);
        }
        m_NotHandleList.erase(itNotHandle++);
    }
 
}