/*******************************************************************
** 文件名: NetController.h
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李界华
** 日  期: 6/1/2017
** 版  本: 1.0
** 描  述: 网络控制器
           可动态调整收发包超时时间，用于均衡其他的性能节点
********************************************************************/
#ifndef __NET_CONTROLLER_H__
#define __NET_CONTROLLER_H__

#pragma once

#include "IClientGlobal.h"

class CNetController
{
    enum
    {
        DEFAULT_NET_TIMEOUT = 20,       // 默认Dispatch超时时间
        DELTA_THRESHOLD     = 10,       // 参考阀值
    };

public:
    CNetController(DWORD dwNetworkUserType=NetworkUserType_Default)
     : m_dwNetworkUserType(dwNetworkUserType)
     , m_dwTimeout(DEFAULT_NET_TIMEOUT)
     , m_nLastPendingCount(0)
     , m_dwLastRunTick(0)
    {

    }

    virtual ~CNetController()
    {

    }    

    int getPendCount()
    {
        return gClientGlobal->getPendingCount();
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

        DispatchNetworkEvents(m_dwTimeout, m_dwNetworkUserType);

        m_dwLastRunTick = getTickCountEx()-dwStartTick;

        int count = getPendCount();
        int delta = count - m_nLastPendingCount;

        // 调整收发包超时时间
        if (delta > DELTA_THRESHOLD) // 包累积数量上升
        {
            m_dwTimeout += 5;      

            if (m_dwTimeout > 100)
            {
                m_dwTimeout = 100;
            } 
        }
        else if (delta < -DELTA_THRESHOLD) // 包累积数量下降
        {
            m_dwTimeout -= 5;
            if (m_dwTimeout < 15)
            {
                m_dwTimeout = 15;
            }
        }
        
        m_nLastPendingCount = count;
    }

private:
    DWORD       m_dwNetworkUserType;
    DWORD       m_dwTimeout;
    int         m_nLastPendingCount;
    DWORD       m_dwLastRunTick;
};


#endif // __NET_CONTROLLER_H__