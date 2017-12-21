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
using namespace rkt;

/**
	功能:
	1.初始化通讯层
	2.响应通讯层消息
	3.初始化定时器框架
*/
class CNetworkHandler : public EventHandler
{
private:
	HANDLE		m_hTimer;
	HANDLE		m_hTimer2;
	TimerAxis *	m_pAxis;

public:
	bool initialize( TimerAxis * pAxis,int nLatency)
	{
		m_pAxis = pAxis;

		int ret = InitializeNetwork();
		if ( ret!=1 )
		{
			ErrorLn("InitializeNetwork failed! error="<<ret);
			return false;
		}

		Reactor * pReacotr = GetAsynIoReactor();
		if ( pReacotr==0 )
		{
			ErrorLn("GetAsynIoReactor failed! Reactor=null");
			return false;
		}

		// 挂接网络事件
		if ( nLatency==0 )
		{
			HANDLE hNetEvent = GetNetworkEventHandle();
			pReacotr->AddEvent(hNetEvent);
			if ( !pReacotr->RegisterEventHandler( hNetEvent,this) )
			{
				ErrorLn("RegisterEventHandler failed!hNetEvent="<<(long)hNetEvent);
				return false;
			}
		}

		// 挂接定时器事件
		if ( m_pAxis )
		{
			m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
			LARGE_INTEGER DueTime;
			DueTime.QuadPart = 0;
			SetWaitableTimer(m_hTimer, &DueTime, 10, NULL, NULL, TRUE);

			pReacotr->AddEvent(m_hTimer);
			if ( !pReacotr->RegisterEventHandler(m_hTimer,this) )
			{
				ErrorLn("RegisterEventHandler failed!m_hTimer=" << (long)m_hTimer);
				return false;
			}

		}

		// 网络延时模拟
		if ( nLatency>0 )
		{
			m_hTimer2 = CreateWaitableTimer(NULL, FALSE, NULL);
			LARGE_INTEGER DueTime;
			DueTime.QuadPart = 0;
			SetWaitableTimer(m_hTimer2, &DueTime, nLatency, NULL, NULL, TRUE);

			pReacotr->AddEvent(m_hTimer2);
			if ( !pReacotr->RegisterEventHandler(m_hTimer2,this) )
			{
				ErrorLn("RegisterEventHandler failed!m_hTimer=" << (long)m_hTimer2);
				return false;
			}
		}

		return true;
	}

	virtual void OnEvent( HANDLE e )
	{
		if ( e==m_hTimer2 )
		{
			DispatchNetworkEvents();
		}

		if ( e==m_hTimer )
		{
			if ( m_pAxis ) m_pAxis->CheckTimer();
		}else
		{
			DispatchNetworkEvents();
		}
	}

	void Close()
	{
		if ( m_hTimer2 )
		{
			CloseHandle(m_hTimer2);
			m_hTimer2 = 0;
		}

		if ( m_hTimer )
		{
			CloseHandle(m_hTimer);
			m_hTimer = 0;
		
			UninitializeNetwork();
		}

		m_pAxis = 0;
	}

	CNetworkHandler() : m_hTimer(0),m_hTimer2(0)
	{
	}

	virtual ~CNetworkHandler()
	{
		Close();
	}

};