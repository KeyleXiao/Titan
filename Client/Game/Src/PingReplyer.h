/*******************************************************************
** 文件名:	PingReplyer.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	12/25/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __PING_REPLY_H__
#define __PING_REPLY_H__

#include "Singleton.h"
#include "DGlobalMessage.h"
#include "TimerHandler.h"
#include "IChatClient.h"
#include "Stage_ManagedDef.h"

using namespace Gateway;
using namespace Stage_ManagerDef;

/**
@name        : 客户端回复PING
@note        : PING工作的机制
@note        : 1.服务器发一个PING包给客户端,同时告诉客户端网络延时数据
@note        : 2.客户端回复服务器一个PING包,服务器根据发送和收到PING的时间间隔算出延时数据
@note        : 3.客户端回复时附带一个客户端的TICK数,该数据可作为客户端是否加速检测的一个参考数据
*/

#define CLIENT_DISCONNECT_INTERVAL 30000
#define CLIENT_DISCONNECT_TIMER_INTERVAL 3000

class PingReplyer :public TimerHandler, public Singleton<PingReplyer>
{
    enum
    {
        CLIENT_DISCONNECT_TIMER,              // 客户端检测
    };

public:
	
	void Start()
	{
        TimerAxis *pTimer = gClientGlobal->getTimerAxis();
        if (pTimer)
        {
            pTimer->KillTimer(CLIENT_DISCONNECT_TIMER, this);
            pTimer->SetTimer(CLIENT_DISCONNECT_TIMER, CLIENT_DISCONNECT_TIMER_INTERVAL, this);
        }
	}

	void Stop()
	{
        TimerAxis *pTimer = gClientGlobal->getTimerAxis();
        if (pTimer)
        {
            pTimer->KillTimer(CLIENT_DISCONNECT_TIMER, this);
        }
	}


	DWORD GetLatency() 
	{
		return m_dwLatency;
	}

    DWORD GetLatestPingTick()
    {
        return m_dwLastPingTick;
    }

    void ResetLatestPingTick()
    {
        m_dwLastPingTick = 0;
    }

	virtual void onMessage( SNetMsgHead* head, void* data, size_t len)
	{
		switch( head->byKeyAction )
		{
		case MSG_GATEWAY_PING:
			{
				HandlePing(data,len);
				break;
			}
		}
	}

	void HandlePing(void* data, size_t len)
	{
		if ( data!=0 && len==sizeof(SMsgGatewayPing))
		{
			SMsgGatewayPing * ping = (SMsgGatewayPing*)data;
			m_dwLatency = ping->dwLatency;
		}

		obuf ob;
		SMsgGatewayPingReply reply;
		reply.dwClientTick = getTickCount();

		SGameMsgHead msgHead;
		msgHead.SrcEndPoint = MSG_ENDPOINT_CLIENT;
		msgHead.DestEndPoint = MSG_ENDPOINT_GATEWAY;
		msgHead.byKeyModule = MSG_MODULEID_GATEWAY;
		msgHead.byKeyAction = MSG_GATEWAY_PING_REPLY;

		ob << msgHead << reply;
		
        net_senddata( ob.data(), ob.size() );

        m_dwLastPingTick = reply.dwClientTick;
	}

    void OnTimer( unsigned long dwTimerID )
    {
        switch(dwTimerID)
        {
        case CLIENT_DISCONNECT_TIMER:
            {
                checkDisConnect();
            }
            break;

        default:
            break;
        }
    }

    void checkDisConnect()
    {
        if (gClientGlobal->getCurState() == ESS_LOGIN)
        {
            PingReplyer::getInstance().ResetLatestPingTick();
            return;
        }

        if (GetLatestPingTick() == 0)
        {
            return;
        }

        DWORD dwNowTick = getTickCount();
        if (dwNowTick - GetLatestPingTick() >= CLIENT_DISCONNECT_INTERVAL)
        {
            WarningLn(__FUNCTION__": Client Socket is disconnected!");

            IChatClient *pChatClient = gClientGlobal->getChatClient();
            if (pChatClient == NULL)
            {
                ErrorLn(__FUNCTION__": pChatClient == NULL");
                return;
            }

            string strMsg = pChatClient->getSystemTipText("",CHAT_TIP_SYSTEM_DISCONNECT);
            cmd_open_servermsg_wnd cmdData;
            sstrcpyn(cmdData.szMsg,  strMsg.data(), sizeof(cmdData.szMsg));
            gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_LOGIN_OPEN_SERVERMSG_VIEW,0,0,&cmdData, sizeof(cmdData));
            PingReplyer::getInstance().ResetLatestPingTick();
        }
    }

	PingReplyer() : m_dwLatency(0),m_dwLastPingTick(0)
	{
        Start();
    }

	virtual ~PingReplyer() { }

private:
    DWORD           m_dwLastPingTick;
	DWORD			m_dwLatency;
};

#endif//__PING_REPLY_H__