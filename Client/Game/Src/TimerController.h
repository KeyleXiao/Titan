/*******************************************************************
** 文件名: TimerController.h
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李界华
** 日  期: 6/1/2017
** 版  本: 1.0
** 描  述: 时间控制器
           可动态调整超时时间，用于均衡其他的性能节点
********************************************************************/
#ifndef __TIMER_CONTROLLER_H__
#define __TIMER_CONTROLLER_H__

#pragma once
#include "TimerAxis.h"

class CTimerController 
{
    enum
    {
        DEFAULT_TIMER_TIMEOUT = 10,     // 默认超时时间
        MIN_THRESHOLD      = 6,         // 左阀值
        MAX_THRESHOLD      = 20,        // 右阀值
    };

public:
    CTimerController()
     : m_dwTimeout(DEFAULT_TIMER_TIMEOUT)
     , m_dwLastRunTick(0)
    {

    }

    virtual ~CTimerController()
    {

    }

    TimerAxis* getTimerAxis()
    {
        return &m_timerAxis;
    }

    DWORD getRunTick()
    {
        return m_dwLastRunTick;
    }

    DWORD getTimeout()
    {
        return m_dwTimeout;
    }

    void update()
    {
        DWORD dwStartTick = getTickCountEx();

        m_timerAxis.CheckTimer(m_dwTimeout);

        DWORD delta = m_dwLastRunTick = getTickCountEx()-dwStartTick;

        // 调整超时时间
        if (delta >= m_dwTimeout)
        {
            m_dwTimeout += 2;      

            if (m_dwTimeout > MAX_THRESHOLD)
            {
                m_dwTimeout = MAX_THRESHOLD;
            } 
        }
        else if (delta < m_dwTimeout)
        {
            m_dwTimeout -= 2;
            if (m_dwTimeout < MIN_THRESHOLD)
            {
                m_dwTimeout = MIN_THRESHOLD;
            }
        }
    }

private:
    TimerAxis   m_timerAxis;
    DWORD       m_dwTimeout;
    DWORD       m_dwLastRunTick;
};


#endif // __TIMER_CONTROLLER_H__