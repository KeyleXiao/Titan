/*******************************************************************
** 文件名:	NetworkHandler.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	6/21/2014
** 版  本:	1.0
** 描  述:	

			处理网络相关环境
********************************************************************/

#pragma once

#include "net/net.h"
#include "net/AsynIoFrame.h"
#include "net/Reactor.h"
#include "TimerAxis.h"
#include "GameAPI.h"
#include "IFramework.h"
#include "IClientGlobal.h"


/**
	功能:
	1.初始化通讯层
	2.响应通讯层消息
	3.初始化定时器框架
*/
/*
class CNetworkHandler : public EventHandler, public IGameViewEventHandler
{
    // 网络收包超时
    enum ENET_DISPATCH_TIMEOUT
    {
        ND_DEFAULT_TIMEOUT      = 100,
        ND_MAX_TIMEOUT          = 200,
    };
    // 网络包走势
    enum ENetHandlePackState
    {
        NET_PACK_DOWN,
        NET_PACK_NORMAL = 0,
        NET_PACK_UP,
    };

public:
	bool initialize(TimerAxis* pAxis, bool bMultiThread)
	{
		m_pAxis = pAxis;

		int ret = InitializeNetwork();
		if ( ret!=1 )
		{
			ErrorLn("InitializeNetwork failed! error="<<ret << " last error:" << getLastError());
			return false;
		}

		if(!bMultiThread)
		{
			return true;
		}

		Reactor * pReacotr = gClientGlobal->getFrameWork()->get_reactor();
		if ( pReacotr==0 )
		{
			ErrorLn("GetAsynIoReactor failed! Reactor=null");
			return false;
		}

		// 挂接网络事件
		HANDLE hNetEvent = GetNetworkEventHandle();
		pReacotr->AddEvent(hNetEvent);
		if ( !pReacotr->RegisterEventHandler( hNetEvent,this) )
		{
			ErrorLn("RegisterEventHandler failed!hNetEvent="<<(long)hNetEvent);
			return false;
		}

		return true;
	}

	virtual void OnEvent( HANDLE e )
	{
        ILogicServiceProxy* pProxy = gClientGlobal->getLogicServiceProxy();
        if(pProxy)
        {
            pProxy->dispatchNetworkEvents();
        }
	}

	// 必须跟显示层在同一个线程，所以订阅显示层的帧update消息
	virtual bool onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam )
	{
        static ulong dwTick = getTickCountEx();

        // 定时器耗时
        ulong dwTimerTick = getTickCountEx();
		if ( m_pAxis ) m_pAxis->CheckTimer();
        ulong dwTimerRunTick = getTickCountEx() - dwTimerTick;

        // 收包参考数量
        ulong dwPendingCount = gClientGlobal->getPendingCount();
        // 网络耗时
        ulong dwNetTick = getTickCountEx();
		dispatchNetwork();
        ulong dwNetRunTick = getTickCountEx() - dwNetTick;

        // 调度耗时输出
        ulong dwCurTick = getTickCountEx();
        if(dwCurTick > dwTick + 100)
        {
            WarningLn("view update low performance, delay="<< (dwCurTick-dwTick) <<"ms, timer cost "<< dwTimerRunTick
                <<"ms, net cost "<< dwNetRunTick <<"ms, count="<<dwPendingCount );
        }
        dwTick = dwCurTick;

		return true;
	}


	CNetworkHandler() : m_pAxis(0), m_nTimeout(ND_DEFAULT_TIMEOUT), m_dwLashPendingCount(0)
	{
	}

	virtual ~CNetworkHandler()
	{
		UninitializeNetwork();
	}

private:

    inline void dispatchNetwork()
    {
        // 处理网络事件
        DispatchNetworkEvents(m_nTimeout, NetworkUserType_Default);

        //monitorNetwork();
    }

    // 网络包在累积时延长派包超时时间，网络包在减少时缩减超时时间
    inline void monitorNetwork()
    {
        int nPendingCount = gClientGlobal->getPendingCount();
        int delta = nPendingCount - m_dwLashPendingCount;

        const int DELTA_NUM = 20;

        ENetHandlePackState state = NET_PACK_NORMAL;
        if(delta > DELTA_NUM)               // 包累计在上升
            state = NET_PACK_UP;
        else if(delta < -DELTA_NUM)         // 包累计在下降
            state = NET_PACK_DOWN;

        switch(state)
        {
        case NET_PACK_UP:   // 包累计在上升
            {
                m_nTimeout += 5;		
                if (m_nTimeout > ND_MAX_TIMEOUT)
                    m_nTimeout = ND_MAX_TIMEOUT;

                m_dwLashPendingCount = nPendingCount;

                //WarningLn("network pack is raising! adjust network timeout : "<< m_nTimeout <<" ms, count="<<nPendingCount);
            }
            break;

        case NET_PACK_DOWN: // 包累计在下降
            {
                m_nTimeout -= 5;
                if (m_nTimeout < ND_DEFAULT_TIMEOUT)
                {
                    m_nTimeout = ND_DEFAULT_TIMEOUT;
                }
                m_dwLashPendingCount = nPendingCount;

                //WarningLn("network pack is reducing! adjust network timeout : "<< m_nTimeout <<" ms, count="<<nPendingCount);
            }
            break;

        default:
            break;
        }
    }


private:
	TimerAxis* m_pAxis;

    // DispatchNetworkEvents处理包的超时时间
    int         m_nTimeout;

    // 上次消息包数量
    ulong       m_dwLashPendingCount;

};
*/